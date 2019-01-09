#include <iostream>
#include "../inc/stats.hpp"

using namespace std;

class Cardinality{
  const Query* query;
  vector<uint64_t> relations;
  vector<vector<Stats>> stats;

public:
  Cardinality(Query*);
  // Get stats vector
  vector<vector<Stats>>& getStatsVector();
  // Get Relation stats
  vector<Stats>& getRelStats(uint64_t);
  // Get Column stats
  Stats& getStats(uint64_t, uint64_t);
  // Get existing Relations
  vector<uint64_t> getRelations();
  // assess cardinality using overloading
  // DUMMY ARGUEMENTS
  void assess(Filter*);
  void assess(1);
  void assess(2);
  void assess(3);
}
