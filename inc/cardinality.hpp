#include <iostream>
#include "../inc/query.hpp"

using namespace std;

class Cardinality{
  const Query* query;
  vector<uint64_t> relations;
  vector<vector<Stats>> stats;

public:
  Cardinality(const Query*);
  // assess cardinality using overloading
  void assess(const Predicate*);
  void assess(const Filter*);
  // get stats vector
  vector<vector<Stats>> getStats();
};
