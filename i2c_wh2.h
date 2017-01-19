/*
   I2C_WH2 - библиотека для чтения данных о температуре и влажности
   с беспроводных датчиков WH2 по протоколу I2C с Arduino собирающей
   данные.

   by Nikita Litvinov
   date 2017.01.19
*/

#ifndef _I2CWH2_h
#define _I2CWH2_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#elif defined(ENERGIA) // LaunchPad, FraunchPad and StellarPad specific
#include "Energia.h"
#elif defined(RPI) // Raspberry Pi
#define RaspberryPi

// Include libraries for RPi:
#include <string.h> /* memcpy */
#include <stdlib.h> /* abs */
#include <wiringPi.h>
#elif defined(SPARK)
#include "application.h"
#else
#include "WProgram.h"
#endif

#include "WH2Data.h"
#include <stdint.h>
#include <Wire.h>
#include "Dictionary.h"
#include "Dictionary.cpp"


#define MAX_WH2_COUNT         20
#define I2C_ADDRESS           23
#define DEFAULT_TIME_FILTER   65000

struct WH2 {
  float temperature;
  uint8_t humidity;
};

class I2C_WH2 {

  private:
    WH2 wh2;
    Dictionary <MAX_WH2_COUNT, uint16_t, WH2Data> dataDict;
    //uint8_t const dataSize = sizeof(WH2Data) + 1;
    uint8_t buffer[WH2DATA_LENGTH];
    uint16_t filter = DEFAULT_TIME_FILTER;

    byte readPkg();
    WH2Data getData(byte *buffer);
  
  public:
    I2C_WH2();
    I2C_WH2(uint16_t filter);

    void read();
    WH2 get();
    byte count();   
    void printWH2Data(WH2Data data);
    void printDict();
};


#endif //<I2CWH2_h>
