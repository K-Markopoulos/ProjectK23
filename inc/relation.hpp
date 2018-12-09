#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "../inc/radix.h"

class Relation{
  int _id;
  std::string sourceFileName;
  bool loaded = false;
  uint64_t num_cols;
  uint64_t num_tuples;
  void* memblock;
  size_t memsize;
  std::vector<void *> cols;
  public:
    Relation(std::string fileName);
    //  load relation in memory
    void loadRelation();
    // get a column from relation
    uint64_t getId() const;
    // get a column from relation
    void* getColumn(int col) const;
    // get a column count from relation
    uint64_t getColumnCount() const;
    // get a tuple from relation by id
    uint64_t getTuple(int col, int tpl) const;
    // get a tuple from relation by pointer
    uint64_t getTuple(const void* col, int tpl) const;
    // get a tuple count from relation
    uint64_t getTupleCount() const;
    // build relation of [rowId,value] tuples
    relation* buildRelation(int col);
    // destructor
    ~Relation();
};
