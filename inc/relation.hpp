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
  void* memblock;
  std::vector<void *> cols;
  std::vector<std::vector<tuple>> cols_; //  ***** MIGHT BE DISCARDED ******
  public:
    Relation(std::string fileName);
    //  load relation in memory
    void loadRelation();
    // get a column from relation
    void* getColumn(int col);
    // get a column count from relation
    uint64_t getColumnCount();
    // get a tuple from relation by id
    uint64_t getTuple(int col, int tpl);
    // get a tuple from relation by pointer
    uint64_t getTuple(void* col, int tpl);
    // get a tuple count from relation
    uint64_t getTupleCount();
};
