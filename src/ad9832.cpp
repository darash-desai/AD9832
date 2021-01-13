#include "AD9832.h"

AD9832::AD9832(uint8_t fsyncPin, uint8_t clockPin, uint8_t dataPin) {
  FSYNC = fsyncPin;
  CLK = clockPin;
  DATA = dataPin;
}

void AD9832::init(uint32_t masterClockFreq) {
  MCLK = masterClockFreq;

  pinMode(FSYNC, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(DATA, OUTPUT);

  digitalWrite(FSYNC, HIGH);
  digitalWrite(CLK, HIGH);

  writeData(0xF800);  // 1, 1, SLEEP = 1, RESET = 1, CLR = 1
  writeData(0xB000);  // 1, 0, SYNC = 1, SELSRC = 1
  writeData(0xC000);  // 1, 1, SLEEP = 0, RESET = 0, CLR = 0

  isInitialized = true;
}

void AD9832::setFrequency(uint32_t frequency) {
  //if (!isInitialized) return;

  // Calculate the multiplier
  uint32_t multiplier = (uint32_t)(((uint64_t)frequency << 32) / MCLK);

  // Break the multiplier into four chunks that specify each of the four
  // registers that must be written to
  uint16_t data_3 = 0x3300 | (0x00FF & (uint16_t)(multiplier >> 24));  // FREQ0 H-MSBs
  uint16_t data_2 = 0x2200 | (0x00FF & (uint16_t)(multiplier >> 16));  // FREQ0 L-MSBs
  uint16_t data_1 = 0x3100 | (0x00FF & (uint16_t)(multiplier >> 8));   // FREQ0 H-LSBs
  uint16_t data_0 = 0x2000 | (0x00FF & (uint16_t)(multiplier));        // FREQ0 L-LSBs

  writeData(data_3);
  writeData(data_2);
  writeData(data_1);
  writeData(data_0);

  writeData(0x1900);  // PHASE0 MSBs
  writeData(0x0800);  // PHASE0 LSBs
}

void AD9832::sleep(bool state) {
  //if (!isInitialized) return;

  (state)
    ? writeData(0xE000)
    : writeData(0xC000);
}

void AD9832::writeData(uint16_t data) {
  //if (!isInitialized) return;

  // Start the clock pin at HIGH since data is clocked into the DDS on each
  // falling edge.
  digitalWrite(CLK, HIGH);

  // Set the data synchronization signal pin LOW to signal the DDS that data
  // will now be written to the device.
  digitalWrite(FSYNC, LOW);

  // Write the high byte of the 16-bit data followed by the low byte
  shiftOut(DATA, CLK, MSBFIRST, data >> 8);
  shiftOut(DATA, CLK, MSBFIRST, data & 0xFF);

  // Signal that data writting is complete
  digitalWrite(FSYNC, HIGH);
}
