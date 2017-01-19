/*
   I2C message format:
   First 1 byte - sensors count (byte type) from 0 to 5
   Serial WH2Data packages, each length 8 bytes
*/

//#include "WH2Sensor.h"
//#include "WH2Storage.h"
#include <Wire.h>


#define UNDEF             127
#define SENSORS_MAX_CNT   10
//#define BUFFER_LENGTH 1 + 8 * SENSORS_MAX_CNT

// Read data from 433MHz receiver on digital pin 2
#define RF_IN 2


// For better efficiency, the port is read directly
// the following two lines should be changed appropriately
// if the line above is changed.
#define RF_IN_RAW PIND2
#define RF_IN_PIN PIND

// Port that is hooked to LED to indicate a packet has been received
#define LED_PACKET A2

#define COUNTER_RATE 3200-1 // 16,000,000Hz / 3200 = 5000 interrupts per second, ie. 200us between interrupts
// 1 is indicated by 500uS pulse
// wh2_accept from 2 = 400us to 3 = 600us
#define IS_HI_PULSE(interval)   (interval >= 2 && interval <= 3)
// 0 is indicated by ~1500us pulse
// wh2_accept from 7 = 1400us to 8 = 1600us
#define IS_LOW_PULSE(interval)  (interval >= 7 && interval <= 8)
// worst case packet length
// 6 bytes x 8 bits x (1.5 + 1) = 120ms; 120ms = 200us x 600
#define HAS_TIMED_OUT(interval) (interval > 600)
// we expect 1ms of idle time between pulses
// so if our pulse hasn't arrived by 1.2ms, reset the wh2_packet_state machine
// 6 x 200us = 1.2ms
#define IDLE_HAS_TIMED_OUT(interval) (interval > 6)
// our expected pulse should arrive after 1ms
// we'll wh2_accept it if it arrives after
// 4 x 200us = 800us
#define IDLE_PERIOD_DONE(interval) (interval >= 4)
// Shorthand for tests
//#define RF_HI (digitalRead(RF_IN) == HIGH)
//#define RF_LOW (digitalRead(RF_IN) == LOW)
#define RF_HI (bit_is_set(RF_IN_PIN, RF_IN_RAW))
#define RF_LOW (bit_is_clear(RF_IN_PIN, RF_IN_RAW))

// wh2_flags
#define GOT_PULSE 0x01
#define LOGIC_HI  0x02


// 8 bytes length
struct WH2Data {
  bool actual;
  uint8_t humidity;
  int16_t temperature;
  uint16_t sensorId;
  uint32_t gotTime;
};

void setup() {
  Serial.begin(9600);
  Wire.begin(23);                // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event
  wh2_setup();
  Serial.println("wh2-i2c started");
}


byte dataSize = 8;
byte bufferSize = 9;

byte sensorsCount = 0;
WH2Data wh2data[SENSORS_MAX_CNT];
uint16_t sensorKeys[SENSORS_MAX_CNT];

byte lastNum = SENSORS_MAX_CNT;
byte buffer[9];

void requestEvent() {

  getI2CBuffer();
  Serial.print("requestEvent buffer=");
  printHex(buffer, bufferSize);
  Wire.write(buffer, bufferSize);

}

void printHex(byte *buffer, byte size) {
  for (int i = 0; i < size; i++) {
    Serial.print(" 0x");
    Serial.print(buffer[i], HEX);
  }
  Serial.println();
}

void loop() {
  wh2_loop();
}

void add(uint16_t sensorId, uint16_t gotTime, int16_t temperature, byte humidity) {
  Serial.print("ADD: dataSize=");
  Serial.print(dataSize);

  Serial.print("; bufferSize=");
  Serial.println(bufferSize);

  Serial.print("sensorId=");
  Serial.print(sensorId);

  Serial.print("; gotTime=");
  Serial.print(gotTime);

  Serial.print("; temperature=");
  Serial.print(temperature);

  Serial.print("; humidity=");
  Serial.println(humidity);

  Serial.println("storage.add ");
  byte num = getKey(sensorId);
  if (num == UNDEF) return;

  Serial.print("storage.add: key=");
  Serial.println(num);

  WH2Data data;
  data.actual = true;
  data.sensorId = sensorId;
  data.gotTime = gotTime;
  data.temperature = temperature;
  data.humidity = humidity;
  wh2data[num] = data;
}


void addRandom() { 
  byte cnt = random() % 3;
  for (int i = 0; i < cnt; i++)
    add(random() % 10, millis()/1000, random() % 350, 11);
}

byte *getI2CBuffer() {
  Serial.println("getI2CBuffer.start");
  //byte buffer[bufferSize];
  for (int i = 0; i < bufferSize; i++) {
    buffer[i] = 0;
  }
  byte cnt = getActualCount();
  buffer[0] = cnt;
  printHex(buffer, bufferSize);

  byte *buf = getNextDataBuffer();

  Serial.print("buf=");
  printHex(buf, dataSize);

  for (int j = 0; j < dataSize; j++) {
    buffer[1 + j] = buf[j];
  }
  printHex(buffer, bufferSize);

  Serial.println("getI2CBuffer.end");
  return buffer;

}


byte getKey(uint16_t key) {
  for (int i = 0; i < sensorsCount; i++)  {
    if (key == sensorKeys[i]) return i;
  }

  if (sensorsCount < SENSORS_MAX_CNT) {
    sensorKeys[sensorsCount] = key;
    return sensorsCount++;
  } else {
    return UNDEF;
  }
}

void clearAll() {
  for (int i = 0; i < SENSORS_MAX_CNT; i++) {
    wh2data[i].actual = false;
  }
}

void clearLast() {
  wh2data[lastNum].actual = false;
}

byte getActualCount() {
  byte cnt = 0;
  for (int i = 0; i < SENSORS_MAX_CNT; i++) {
    if (wh2data[i].actual) cnt += 1;
  }

  return cnt;
}

WH2Data getNextData() {
  //if (currentNum >= getActualCount()) currentNum = 0;
  for (int i = 0; i < sensorsCount; i++) {
    if (wh2data[i].actual) {
      lastNum = i;
      return wh2data[i];
    }
  }
}

byte *getNextDataBuffer() {
  WH2Data data = getNextData();

  Serial.print("getNextDataBuffer(): sensorId=");
  Serial.print(data.sensorId);

  Serial.print("; gotTime=");
  Serial.print(data.gotTime);

  Serial.print("; temperature=");
  Serial.print(data.temperature);

  Serial.print("; humidity=");
  Serial.println(data.humidity);

  byte buf[dataSize];

  buf[0] = data.actual;
  buf[1] = data.humidity;

  buf[2] = data.temperature;
  buf[3] = data.temperature >> 8;

  buf[4] = data.sensorId;
  buf[5] = data.sensorId >> 8;

  uint16_t timesGone = (millis()/1000) - data.gotTime;

  buf[6] = timesGone;
  buf[7] = timesGone >> 8;

  Serial.print("getNextDataBuffer() buf = ");
  printHex(buf, dataSize);
  clearLast();
  return buf;
}

volatile byte wh2_flags = 0;
volatile byte wh2_packet_state = 0;
volatile int wh2_timeout = 0;
byte wh2_packet[5];
byte wh2_calculated_crc;
//extern WH2Storage *storage;

ISR(TIMER1_COMPA_vect)
{
  static byte sampling_state = 0;
  static byte count;
  static boolean was_low = false;

  switch (sampling_state) {
    case 0: // waiting
      wh2_packet_state = 0;
      if (RF_HI) {
        if (was_low) {
          count = 0;
          sampling_state = 1;
          was_low = false;
        }
      } else {
        was_low = true;
      }
      break;
    case 1: // acquiring first pulse
      count++;
      // end of first pulse
      if (RF_LOW) {
        if (IS_HI_PULSE(count)) {
          wh2_flags = GOT_PULSE | LOGIC_HI;
          sampling_state = 2;
          count = 0;
        } else if (IS_LOW_PULSE(count)) {
          wh2_flags = GOT_PULSE; // logic low
          sampling_state = 2;
          count = 0;
        } else {
          sampling_state = 0;
        }
      }
      break;
    case 2: // observe 1ms of idle time
      count++;
      if (RF_HI) {
        if (IDLE_HAS_TIMED_OUT(count)) {
          sampling_state = 0;
        } else if (IDLE_PERIOD_DONE(count)) {
          sampling_state = 1;
          count = 0;
        }
      }
      break;
  }

  if (wh2_timeout > 0) {
    wh2_timeout++;
    if (HAS_TIMED_OUT(wh2_timeout)) {
      wh2_packet_state = 0;
      wh2_timeout = 0;
    }
  }
}

void wh2_setup() {
  pinMode(LED_PACKET, OUTPUT);
  pinMode(RF_IN, INPUT);

  TCCR1A = 0x00;
  TCCR1B = 0x09;
  TCCR1C = 0x00;
  OCR1A = COUNTER_RATE;
  TIMSK1 = 0x02;

  // enable interrupts
  sei();
}

void wh2_loop() {
  static unsigned long old = 0, packet_count = 0, bad_count = 0, average_interval;
  unsigned long spacing, now;
  byte i;

  if (wh2_flags) {
    if (wh2_accept()) {
      // calculate the CRC
      wh2_calculate_crc();

      now = millis();
      spacing = now - old;
      old = now;
      
      packet_count++;
      average_interval = now / packet_count;
      if (!wh2_valid()) {
        bad_count++;
      }

      // flash green led to say got packet
      digitalWrite(LED_PACKET, HIGH);
      delay(100);
      digitalWrite(LED_PACKET, LOW);

      Serial.print(packet_count, DEC);
      Serial.print(" | ");
      Serial.print(bad_count, DEC);
      Serial.print(" | ");
      Serial.print(spacing, DEC);
      Serial.print(" | ");
      Serial.print(average_interval, DEC);
      Serial.print(" | ");

      for (i = 0; i < 5; i++) {
        Serial.print("0x");
        Serial.print(wh2_packet[i], HEX);
        Serial.print("/");
        Serial.print(wh2_packet[i], DEC);
        Serial.print(" ");
      }
      Serial.print("| Sensor ID: 0x");
      Serial.print(wh2_sensor_id(), HEX);
      Serial.print(" | ");
      Serial.print(wh2_humidity(), DEC);
      Serial.print("% | ");
      Serial.print(wh2_temperature(), DEC);
      Serial.print(" | ");
      Serial.println((wh2_valid() ? "OK" : "BAD"));

      if (wh2_valid()) {
        add(wh2_sensor_id(), millis() / 1000, wh2_temperature(), wh2_humidity());
      }

      //addRandom();
      
    }

    wh2_flags = 0x00;
  }
}

/*
  WH2Data getData() {
  WH2Data wh2;
  wh2.actual = true;
  wh2.temperature = wh2_temperature();
  wh2.humidity = wh2_humidity();
  isData = false;
  return wh2;
  }*/

// processes new pulse
boolean wh2_accept()
{
  static byte packet_no, bit_no, history;

  // reset if in initial wh2_packet_state
  if (wh2_packet_state == 0) {
    // should history be 0, does it matter?
    history = 0xFF;
    wh2_packet_state = 1;
    // enable wh2_timeout
    wh2_timeout = 1;
  } // fall thru to wh2_packet_state one

  // acquire preamble
  if (wh2_packet_state == 1) {
    // shift history right and store new value
    history <<= 1;
    // store a 1 if required (right shift along will store a 0)
    if (wh2_flags & LOGIC_HI) {
      history |= 0x01;
    }
    // check if we have a valid start of frame
    // xxxxx110
    if ((history & B00000111) == B00000110) {
      // need to clear packet, and counters
      packet_no = 0;
      // start at 1 becuase only need to acquire 7 bits for first packet byte.
      bit_no = 1;
      wh2_packet[0] = wh2_packet[1] = wh2_packet[2] = wh2_packet[3] = wh2_packet[4] = 0;
      // we've acquired the preamble
      wh2_packet_state = 2;
    }
    return false;
  }
  // acquire packet
  if (wh2_packet_state == 2) {

    wh2_packet[packet_no] <<= 1;
    if (wh2_flags & LOGIC_HI) {
      wh2_packet[packet_no] |= 0x01;
    }

    bit_no ++;
    if (bit_no > 7) {
      bit_no = 0;
      packet_no ++;
    }

    if (packet_no > 4) {
      // start the sampling process from scratch
      wh2_packet_state = 0;
      // clear wh2_timeout
      wh2_timeout = 0;
      return true;
    }
  }
  return false;
}


void wh2_calculate_crc()
{
  wh2_calculated_crc = crc8(wh2_packet, 4);
}

bool wh2_valid()
{
  return (wh2_calculated_crc == wh2_packet[4]);
}

int wh2_sensor_id()
{
  return (wh2_packet[0] << 4) + (wh2_packet[1] >> 4);
}

byte wh2_humidity()
{
  return wh2_packet[3];
}

/* Temperature in deci-degrees. e.g. 251 = 25.1 */
int wh2_temperature()
{
  int temperature;
  temperature = ((wh2_packet[1] & B00000111) << 8) + wh2_packet[2];
  // make negative
  if (wh2_packet[1] & B00001000) {
    temperature = -temperature;
  }
  return temperature;
}

uint8_t crc8( uint8_t *addr, uint8_t len)
{
  uint8_t crc = 0;

  // Indicated changes are from reference CRC-8 function in OneWire library
  while (len--) {
    uint8_t inbyte = *addr++;
    for (uint8_t i = 8; i; i--) {
      uint8_t mix = (crc ^ inbyte) & 0x80; // changed from & 0x01
      crc <<= 1; // changed from right shift
      if (mix) crc ^= 0x31;// changed from 0x8C;
      inbyte <<= 1; // changed from right shift
    }
  }
  return crc;
}




