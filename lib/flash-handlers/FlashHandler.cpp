#include <Arduino.h>
#include <SPIMemory.h>
#include "FlashHandler.h"

uint32_t writeString(SPIFlash flash, String _string)
{
    uint32_t addr = flash.getAddress(flash.sizeofStr(_string));
    Serial.println("Writing to address: ");
    Serial.print(addr);
    if (flash.writeStr(addr, _string))
    {
        Serial.println("Wrote sucessfully!");
        return addr;
    }
    else
    {
        Serial.println("Data error");
        Serial.println(flash.error(true));
        return 0;
    }
}

String readString(SPIFlash flash, uint32_t addr)
{
    String output_str;
    if (!flash.readStr(addr, output_str))
    {
        Serial.println("Data error");
        Serial.println(flash.error(true));
    }
    return output_str;
}