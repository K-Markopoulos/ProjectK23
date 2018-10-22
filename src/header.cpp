#include <iostream>
#include <string>
#include <math.h>
#include "header.h"

int  h1(int value, int n){
  lastbits = pow(2,n) - 1;
  return value & lastbits;
}

int considered(int32_t value, tuple * values){
  int i = 0;
  while(values[i] != NULL){
    if(values[i].key == value)
      return ++i;
    else
      i++;
  }
  return i;
}

int size(tuple * array){
  int i = 0, s = 0;
  while(array[i] != NULL){
    s++;
    i++;
  }
  return s;
}

tuple * histogram(tuple *R){
  int position, size = size(R);
  tuple * values, * new_values;
  for(int i = 0; i < size; i++){
    if(position = considered(R.payload, values)){
      values[position].payload++;
    }
    else{
      s = size(values);
      new_values = realloc(values, (s + 1)*sizeof(tuple));
      values = new_values;
      values[s].key = R.payload;
      value[s].payload = 1;
    }
  }
}
