#include <iostream>
#include "inc/query.h"

class Query{
  std::vector<Relation> relations;
  std::vector<Predicate> predicates;
  std::vector<Filter> filters;
  std::vector<Selector> selectors;

  //
  void parseQuery(const string line);
  // get Relation
  Relation* getRelation(const uint64_t i);
  // get Predicate
  Predicate* getPredicate(const uint64_t i);
  // get Filter
  Filter* getFilter(const uint64_t i);
  // get Selector
  Selector* getSelector(const uint64_t i);



}
