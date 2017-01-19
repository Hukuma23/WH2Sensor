#ifndef _DICTIONARY_cpp
#define _DICTIONARY_cpp

#include "Dictionary.h"

template < byte count, typename KeyType, typename DataType >
byte Dictionary< count, KeyType, DataType > ::getNumByKey(KeyType key) {
  for (int i = 0; i < count; i++)  {
    if (key == keys[i]) return i;
  }
  return UNDEF;
}

template < byte count, typename KeyType, typename DataType >
byte Dictionary< count, KeyType, DataType >::setNumByKey(KeyType key) {
  byte num = getNumByKey(key);
  if (num != UNDEF) return num;

  if (dataCount < count) {
    keys[dataCount] = key;
    return dataCount++;
  } else {
    return UNDEF;
  }
}

template < byte count, typename KeyType, typename DataType >
DataType Dictionary< count, KeyType, DataType >::removeByNum(byte num) {
  if ((num >= dataCount) || (num < 0) || (num == UNDEF)) return emptyData;

  DataType removed = data[num];
  for (int i = num; i < dataCount; i++) {
    keys[i] = keys[i + 1];
    data[i] = data[i + 1];
  }
  dataCount--;
  return removed;
}

template < byte count, typename KeyType, typename DataType >
byte Dictionary< count, KeyType, DataType >::getLastNum() {
  byte num = dataCount - 1;
  if (num < 0)
    return UNDEF;
  else
    return num;
}


// Public part
template < byte count, typename KeyType, typename DataType >
DataType Dictionary< count, KeyType, DataType >::get(KeyType key) {
  byte num = getNumByKey(key);
  if (num != UNDEF)
    return data[num];
  else
    return NULL;
}

template < byte count, typename KeyType, typename DataType >
bool Dictionary< count, KeyType, DataType >::push(KeyType key, DataType data) {
  byte num = setNumByKey(key);
  if (num == UNDEF) return false;

  this->data[num] = data;
  return true;
}

template < byte count, typename KeyType, typename DataType >
DataType Dictionary< count, KeyType, DataType >::remove(KeyType key) {
  byte num = getNumByKey(key);
  return removeByNum(num);
}

template < byte count, typename KeyType, typename DataType >
DataType Dictionary< count, KeyType, DataType >::removeLast() {
  byte num = getLastNum();
  return removeByNum(num);
}

template < byte count, typename KeyType, typename DataType >
byte Dictionary< count, KeyType, DataType >::getCount() {
  return dataCount;
}

template < byte count, typename KeyType, typename DataType >
KeyType * Dictionary< count, KeyType, DataType >::getKeys() {
  return this->keys;
}

template < byte count, typename KeyType, typename DataType >
DataType * Dictionary< count, KeyType, DataType >::getData() {
  return this->data;
}

#endif //_DICTIONARY_cpp
