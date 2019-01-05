#include <iostream>
#include "../inc/stats.hpp"

class Cardinality{
  Stats* statsA;
  Stats* statsB;
  uint64_t la,ua,fa,da; // assessments for new statistic values of column A
  uint64_t lb,ub,fb,db; // assessments for new statistic values of column B

public:
  Cardinality(Stats&,Stats&);
  // assess cardinality using overloading
  void assess(uint64_t);
  void assess(uint64_t,uint64_t);
  void assess();
}
