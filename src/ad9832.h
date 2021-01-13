#include "Arduino.h"

#ifndef AD9832_H
#define AD9832_H

class AD9832 {
public:
  AD9832(uint8_t fsyncPin, uint8_t clockPin, uint8_t dataPin);

  /**
   * Initializes the AD9832 with the specified master clock frequency. The DDS
   * output frequencies are all binary fractions of this clock frequency. Note
   * that this method must be called before using any other instance method.
   *
   * @param masterClockFreq   The master clock frequency to set for the DDS.
   */
  void init(uint32_t masterClockFreq);

  /**
   * Sets the output frequency for the DDS. Note that the actual frequency that
   * will be set will be the closet equivalent whole binary decimal of the
   * master clock frequency. The maximum frequency that can be set is half of
   * the master clock frequency.
   *
   * @param frequency   The frequency to set for the output signal.
   */
  void setFrequency(uint32_t frequency);

  /**
   * Sets the DDS to a low-power state that consumes 5 mW (5V) or 3 mW (3V).
   *
   * @param state   True to set the DDS to a low-power state and false
   *                otherwise.
   */
  void sleep(bool state);

private:
  bool isInitialized = false;
  uint32_t MCLK;
  uint8_t FSYNC;
  uint8_t CLK;
  uint8_t DATA;

  /**
   * Writes a 16-bit data chunk to the device.
   */
  void writeData(uint16_t data);
};

#endif
