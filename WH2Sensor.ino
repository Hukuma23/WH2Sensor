/*
   I2C message format:
   First 1 byte - sensors count (byte type) from 0 to 5
   Serial WH2Data packages, each length 8 bytes
*/

//#include "WH2Sensor.h"
//#include "WH2Storage.h"
#include <Wire.h>
#include "WH2Data.h"
#include "WH2Read.h"

void setup() {
  Serial.begin(9600);
  Wire.begin(23);                // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event
  wh2_setup();
  Serial.println("wh2-i2c started");
}


byte buffer[WH2DATA_LENGTH];

void requestEvent() {

  getI2CBuffer(buffer);
  Serial.print("requestEvent buffer=");
  printHex(buffer, WH2DATA_LENGTH);
  Wire.write(buffer, WH2DATA_LENGTH);

}


void loop() {
  wh2_loop();
}


//extern WH2Storage *storage;











