#include "mbed.h"
#include "Arduino.h"
#include "AD9832.h"

/**
 * The output pin on which the master clock signal is drive to supply the DDS.
 */
const uint32_t MASTER_CLOCK_PIN = NRF_GPIO_PIN_MAP(0, 27);

/** Master clock frequency in hertz. */
const uint32_t MASTER_FREQUENCY = 8000000;

/** Desired output frequency in hertz. */
const uint32_t OUTPUT_FREQUENCY = 400;

/** The DDS instance used to produce the desired sinusoidal waveforms. */
AD9832 DDS(D2, D3, D4);

/**
 * Generates a master clock signal on the specified pin to be used to drive an
 * external device. This is accomplished by using the nRF52840 16 MHz high
 * frequency peripherals clock. This is used together with GPIO tasks and events
 * to toggle the signal on the specified pin. The output signal is of a
 * saw-tooth shape with a frequency of 8 MHz.
 *
 * @param clockPin    The nRF5x SDK pin on which the clock signal should be
 *                    generated.
 */
void initializeMasterClock(uint32_t clockPin) {
  nrf_gpio_cfg_output(clockPin); // Configure `clockPin` as an output pin

  NRF_CLOCK->TASKS_HFCLKSTART = 1; // Start high frequency clock
  while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0); // Wait for HFCLK to start

  NRF_CLOCK->EVENTS_HFCLKSTARTED = 0; // Clear event

  // Configure GPIOTE to toggle `clockPin`
  NRF_GPIOTE->CONFIG[0] = GPIOTE_CONFIG_MODE_Task << GPIOTE_CONFIG_MODE_Pos |
                          GPIOTE_CONFIG_POLARITY_Toggle << GPIOTE_CONFIG_POLARITY_Pos |
                          clockPin << GPIOTE_CONFIG_PSEL_Pos |
                          GPIOTE_CONFIG_OUTINIT_Low << GPIOTE_CONFIG_OUTINIT_Pos;

  // Configure timer
  NRF_TIMER1->PRESCALER = 0;
  NRF_TIMER1->CC[0] = 1;  // Adjust the output frequency by adjusting the CC.
  NRF_TIMER1->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos;
  NRF_TIMER1->TASKS_START = 1;

  // Configure PPI
  NRF_PPI->CH[0].EEP = (uint32_t) &NRF_TIMER1->EVENTS_COMPARE[0];
  NRF_PPI->CH[0].TEP = (uint32_t) &NRF_GPIOTE->TASKS_OUT[0];

  NRF_PPI->CHENSET = PPI_CHENSET_CH0_Enabled << PPI_CHENSET_CH0_Pos;
}

void setup() {
  initializeMasterClock(MASTER_CLOCK_PIN);
  DDS.init(MASTER_FREQUENCY);
  DDS.setFrequency(OUTPUT_FREQUENCY);
}

void loop() {
}
