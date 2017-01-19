#ifndef _DICTIONARY_h
#define _DICTIONARY_h

#include "Arduino.h"
#define UNDEF   255
template < byte count, typename KeyType, typename DataType > 
class Dictionary{
  private:
    KeyType keys[count];
    DataType data[count];
    DataType emptyData;
    byte dataCount = 0;

    byte getNumByKey(KeyType key);
    byte setNumByKey(KeyType key);
    DataType removeByNum(byte num);
    byte getLastNum();

  public:
    bool push (KeyType key, DataType data);
    DataType get (KeyType key);
    DataType remove(KeyType key);
    DataType removeLast() ;
    byte getCount();
    KeyType *getKeys();
    DataType *getData();

};

#endif //_DICTIONARY_h


