#include "i2c_wh2.h"


    I2C_WH2::I2C_WH2() {
      Wire.begin();
    }

    I2C_WH2::I2C_WH2(uint16_t filter) {
      this->filter = filter;
      Wire.begin();
    }

    void I2C_WH2::read() {
      int cnt = readPkg();
      Serial.print("Read "); Serial.print(cnt); Serial.println(" packages");
      for (int i = 0; i < cnt - 1; i++) {
        readPkg();
      }
    }

    WH2 I2C_WH2::get() {
      WH2Data data = dataDict.removeLast();
      wh2.temperature = ((float)data.temperature)/10;
      wh2.humidity = data.humidity;
      return wh2;
    }

    byte I2C_WH2::count() {
      return dataDict.getCount();
    }
    
    void I2C_WH2::printWH2Data(WH2Data data) {
      Serial.print("Actual = ");
      Serial.print(data.actual);
      Serial.print(", ID = ");
      Serial.print(data.id);
      Serial.print(", Time = ");
      Serial.print(data.time);
      Serial.print(", Temp = ");
      Serial.print(data.temperature);
      Serial.print(", Hum = ");
      Serial.print(data.humidity);
      Serial.println();
    }

    void I2C_WH2::printDict() {
      byte count = dataDict.getCount();
      Serial.print("printDict START. count=");
      Serial.println(count);
      for (int i = 0; i < count; i++) {
        Serial.print(i);
        Serial.print(": ");
        printWH2Data(dataDict.removeLast());
      }
      Serial.println("printDict END.");
    }

    byte I2C_WH2::readPkg() {
      Wire.requestFrom(I2C_ADDRESS, WH2DATA_LENGTH);    // request WH2DATA_LENGTH bytes from slave device # I2C_ADDRESS

      int i = 0;
      while (Wire.available()) { // slave may send less than requested
        digitalWrite(LED_BUILTIN, HIGH);
        buffer[i++] = Wire.read();
        //Serial.print(" 0x");
        //Serial.print(buffer[i - 1], HEX);
        //Serial.print(buffer[i-1]);
        digitalWrite(LED_BUILTIN, LOW);
      }
      //Serial.println();
      //Serial.print("Read "); Serial.print(i); Serial.print(" bytes; Num pkgs = "); Serial.println(buffer[0]);
      if (buffer[0] > 0) {
        WH2Data data = getData(buffer);
        if (data.time < filter) {
          dataDict.push(data.id, data);
          //printWH2Data(data);
        }
        else 
          printf("%d (t:%d, h:%d) with time %d was filtered by %d filter", data.id, data.temperature, data.humidity, data.time, filter);
        
      }
      return buffer[0];
    }

    WH2Data I2C_WH2::getData(byte *buffer) {
      int temp;
      WH2Data data;
      data.actual = buffer[1];
      data.humidity = buffer[2];
      data.temperature = (buffer[4] << 8) + buffer[3];
      data.id = (buffer[6] << 8) + buffer[5];
      data.time = (buffer[8] << 8) + buffer[7];

      return data;
    }

