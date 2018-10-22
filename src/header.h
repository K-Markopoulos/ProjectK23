#include<iostream>

using namespace std;

struct tuple {
  int32_t key;
  int32_t payload;
};

struct relation {
  tuple *tuples;
  uint32_t num_tuples;
};

struct result {
  tuple *tuples;
};
