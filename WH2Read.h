/* 
 * File:   WH2Sensor.h
 * Author: Nikita
 *
 * Created on 11 Января 2017 г., 14:32
 * The code based on https://github.com/lucsmall/BetterWH2
 * 
 */
 


#ifndef WH2READ_H
#define WH2READ_H

#include "Arduino.h"
#include "WH2Data.h"
#include "Dictionary.h"
#include "Dictionary.cpp"

// Read data from 433MHz receiver on digital pin 2
#define RF_IN 2
#define SENSORS_MAX_CNT   20

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

byte getNextDataBuffer(byte buffer[]);
byte getI2CBuffer(byte buffer[]);
void printHex(byte buffer[], byte size);
void add(uint16_t id, uint16_t gotTime, int16_t temperature, byte humidity);
void addRandom();

void wh2_setup();
void wh2_loop();
boolean wh2_accept();
void wh2_calculate_crc();
bool wh2_valid();
int wh2_sensor_id();
byte wh2_humidity();
int wh2_temperature();
uint8_t crc8( uint8_t *addr, uint8_t len);


#endif  /* WH2READ_H */

