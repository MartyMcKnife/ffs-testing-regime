#include <Arduino.h>
#include <SPIMemory.h>

uint32_t writeString(SPIFlash flash, String &_string);
void readStringToSerial(SPIFlash flash, uint32_t addr);
void readWriteString(SPIFlash flash, String &_string);