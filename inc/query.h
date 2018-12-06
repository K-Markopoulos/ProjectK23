#include <iostream>
#include <vector>
#include <string>
#include <stdbool.h>

class Query{
  // std::vector<Relation> relations;
  // std::vector<Predicate> predicates;
  // std::vector<Filter> filters;
  // std::vector<Selector> selectors;
  public:
    // parse query in [FROM]|[WHERE]|[SELECT]
    bool parseQuery(const std::string line);
    // get Relation
    // Relation* getRelation(const uint64_t i);
    // // get Predicate
    // Predicate* getPredicate(const uint64_t i);
    // // get Filter
    // Filter* getFilter(const uint64_t i);
    // // get Selector
    // Selector* getSelector(const uint64_t i);



};
