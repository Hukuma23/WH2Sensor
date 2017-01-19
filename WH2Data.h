#ifndef _WH2DATA_h
#define _WH2DATA_h


#define WH2DATA_SIZE      8
#define WH2DATA_LENGTH    1 + WH2DATA_SIZE
struct WH2Data {
  bool actual;
  uint8_t humidity;
  int16_t temperature;
  uint16_t id;
  uint16_t time;
};

#endif //_WH2DATA_h
