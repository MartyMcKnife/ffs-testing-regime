#include <Arduino.h>
#include <SPIMemory.h>
#include <DiagnosticsRun.h>
#include <CommandList.h>
#include <FlashHandler.h>

// The library we are using for this demo DOES NOT support QSPI
// Adafruit's SPI library may be able to help with this
// SPIMemory works for a wider range of cores, which is why it is used for this demo
// Adafruit SPI Flash only works for a nRF52, SAMD, RP2040 or ESP32
// Neither of these libraries work with an STM32, so are NOT to be used on any production ready cubesat
// This code is to ONLY be used for testing and validation purposes

#define BAUD_RATE 115200

// our data.txt file is 100,304 bytes
//  reserve that in memory
const byte numChars = 100304;
char receivedChars[numChars];

SPIFlash flash;

void setup()
{
  // begin flash
  Serial.begin(BAUD_RATE);

  // 50 ms delay to let chip bootup
  delay(50);
  flash.begin();
  Serial.println("Device ID:");
  Serial.println(flash.getJEDECID());
  Serial.println("Capacity:");
  Serial.println(flash.getCapacity());

  // display main command list
  mainlist();
  // let python script know arduino is ready
  Serial.println("<Arduino is ready>");
}

void loop()
{
  // setup vars for reading serial data
  bool test_2 = false;
  char startMarker = '<';
  char endMarker = '>';
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char rc;

  while (Serial.available() > 0)
  {
    if (test_2)
    {
      // Parse data sent through serial
      // Looks for user defined start/end chars
      // Adapated from https://forum.arduino.cc/t/pc-arduino-comms-using-python-updated/574496
      rc = Serial.read();
      if (recvInProgress == true)
      {
        // Check if we've finished reading the string
        if (rc != endMarker)
        {
          // update our string
          receivedChars[ndx] = rc;
          ndx++;
          // if our data file is more than the amount of space we've reserved, start overwriting data
          if (ndx >= numChars)
          {
            ndx = numChars - 1;
          }
        }
        else
        {
          // terminate string
          receivedChars[ndx] = '\0';
          recvInProgress = false;
          ndx = 0;
        }
      }
      // only start parsing if we see our marker
      else if (rc == startMarker)
      {
        recvInProgress = true;
      }

      if (receivedChars == "STOP")
      {
        test_2 = false;
      }
      else
      {
        Serial.println("Writing text file to nand flash");
        uint32_t addr = writeString(flash, receivedChars);
        if (addr)
        {
          Serial.println("Reading back from address: ");
          Serial.println(addr);
          Serial.println(readString(flash, addr));
        }
      }
    }
    else
    {
      uint8_t number = Serial.parseInt();
      switch (number)
      {
        // Run diagnostics
      case 0:
      {
        runDiagnostics(flash);
        break;
      }

      case 1:
      { // Write hello world and read it back
        uint32_t addr = writeString(flash, "Hello World!");
        if (addr)
        {
          Serial.println("Reading back from address: ");
          Serial.println(addr);
          Serial.println(readString(flash, addr));
        }
        break;
      }
      case 2:
      {
        test_2 = true;
        Serial.println("Awaiting text dump. Please use writeData.py found in test folder!");
        Serial.println("Type '<STOP>' to quit test");
        break;
      }

      default:
      {
        Serial.println("Not a valid command!");
        mainlist();
      }
      }
    }
  }
}
