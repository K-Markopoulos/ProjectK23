#include <iostream>
#include "../inc/relation.hpp"

class Stats{
  Relation* relation;
  void * column;
  uint64_t l; //lower value of column A
  uint64_t u; //upper value of column A
  uint64_t f; //number of values in column A
  uint64_t d; //number of unique values in column A


  public:
  Stats(Relation&, int);
  // get relation reference
  Relation* getRelation();
  // get column reference
  void* getColumn();
  // get a certain tuple
  uint64_t getTuple(const void*, int);
  // get number of tuples in column
  uint64_t getTupleCount();
  // set the values of l,u and f that require one column access
  void setluf();
  // set the value of d
  void setd();
  // process a boolean vector to count distinct values
  uint64_t bool_process(true);
  uint64_t bool_process(false);
  // getters for each statistic value
  uint64_t getl();
  uint64_t getu();
  uint64_t getf();
  uint64_t getd();
  // setters for each statistic value
  void setl(uint64_t);
  void setu(uint64_t);
  void setf(uint64_t);
  void setd(uint64_t);
}
