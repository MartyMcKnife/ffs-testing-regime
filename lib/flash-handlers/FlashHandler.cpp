#include <Arduino.h>
#include <SPIMemory.h>
#include <chrono>
#include "FlashHandler.h"

uint32_t writeString(SPIFlash flash, String &_string)
{
    unsigned long t1 = millis();
    uint32_t addr = flash.getAddress(flash.sizeofStr(_string));
    Serial.println("Writing to address: ");
    Serial.print(addr);
    if (flash.writeStr(addr, _string))
    {
        unsigned long t2 = millis();
        Serial.println("Wrote sucessfully!");
        Serial.println("Call time (in ms):");
        Serial.print(t2 - t1);
        return addr;
    }
    else
    {
        Serial.println("Data error");
        Serial.println(flash.error(true));
        return 0;
    }
}

void readStringToSerial(SPIFlash flash, uint32_t addr)
{
    unsigned long t1 = millis();

    String output_str;
    if (!flash.readStr(addr, output_str))
    {
        Serial.println("Data error");
        Serial.println(flash.error(true));
    }
    else
    {
        unsigned long t2 = millis();
        Serial.print(output_str);
        Serial.println("Call time (in ms):");
        Serial.print(t2 - t1);
    }
}

void readWriteString(SPIFlash flash, String &_string)
{
    uint32_t addr = writeString(flash, _string);
    if (addr)
    {
        Serial.println("Reading back from address: ");
        Serial.println(addr);
        readStringToSerial(flash, addr);
    }
}