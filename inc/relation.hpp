#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "../inc/radix.h"

class Relation{
  std::string sourceFileName;
  bool loaded = false;
  uint64_t num_cols;
  uint64_t num_tuples;
  std::vector<std::vector<tuple> > cols;
  public:
    Relation(std::string fileName);
    void loadRelation();
};
