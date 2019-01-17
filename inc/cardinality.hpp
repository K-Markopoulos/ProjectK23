#include <iostream>
#include "../inc/query.hpp"

using namespace std;

class Cardinality{
  vector<uint64_t> relations;
  vector<vector<Stats>> stats;

public:
  Cardinality(const Query*);
  // assess cardinality using overloading
  uint64_t assess(const Predicate*);
  void assess(const Filter*);
  // get stats vector
  vector<vector<Stats>> getStats();
};
