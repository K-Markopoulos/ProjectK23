#include <iostream>
#include <math.h>
#include "../inc/header.h"

/* hash function */
int h1(int value, int n){
  lastbits = (1 << n) - 1;
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

tuple * psum(tuple * hist){
  int s = size(hist);
  tuple Psum[s];
  for(int i=0;i<s;i++){
    Psum[i].key = hist[i].key;
    if(i==0)
      Psum[i].payload = 0;
    else
      Psum[i].payload = Psum.[i-1].payload + hist[i-1].payload;
  }
  return Psum;
}

int32_t getposition(int32_t value, tuple* pos_list){
  int i=0;
  int32_t position;
  while(pos_list[i] != NULL){
    if(pos_list[i].key == value){
      position = pos_list[i].payload;
      pos_list[i].payload++;
      return position;
    }
    i++;
  }
}

tuple * reorder(tuple *R, tuple *newR, tuple * psum){
  int s = size(R), spsum = size(psum);
  int32_t j;
  tuple * dictionary = (dictionary*) malloc(sizeof(psum));
  memcpy(dictionary,psum,sizeof(psum);
  for(int i = 0; i<s; i++){
    j = getposition(R[i].payload, dictionary);
    memcpy(newR[j],R[i], sizeof(struct tuple));
  }
}
