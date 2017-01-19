#include "i2c_wh2.h"
#include "WH2Data.h"

#include <Wire.h>

I2C_WH2 i2cwh2(600);


void setup() {
  Serial.begin(9600);  // start serial for output
  Serial.println("i2c_master_reader STARTED");
}



void loop() {

  i2cwh2.read();
  Serial.println();
  //i2cwh2.printDict();
  byte cnt = i2cwh2.count();
  for (int i = 0; i < cnt; i++) {
    WH2 data = i2cwh2.get();
    Serial.print(i); Serial.print(": ");
    print(data);
  }
  delay(10000);
}


void print(WH2 data) {
    Serial.print("id=");
    Serial.print(data.id);
    Serial.print(", temperature=");
    Serial.print(data.temperature);
    Serial.print(", humidity=");
    Serial.println(data.humidity);
}


