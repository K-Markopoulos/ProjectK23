#include <iostream>
#include <string>
#include <vector>
#include <stdbool.h>
//#include "../inc/database.h"
#include "../inc/query.h"
#include "../inc/utils.h"


/** -----------------------------------------------------
 * parse a query, populate relations, predicates, filters and selectors of query
 *
 * @params line, [FROM]|[WHERE]|[SELECT]
 */
bool Query::parseQuery(const string line){
  //NOT IMPLEMENTED

  //split by '|'
  vector<string> sections = split(line, '|');

  MUST(validateSections(sections))
  LOG("sections OK!\n")
  //add relations
  for(string rel : split(sections[0], ' ')){
    MUST(validateRelation(rel))
    LOG("Relation '%s' OK!\n", rel.c_str())
    //relations.emplace_back( Relation(rel) );
  }

  //add predicates and filters
  for(string pred : split(sections[1], '&')){
    MUST(validatePredicate(pred))
    LOG("Predicate '%s' OK!\n", pred.c_str())
    // if(isFilter(pred))
    //   filters.emplace_back(Filter(pred));
    // else
    //   predicates.emplace_back(Predicate(pred));
  }

  //add selectors
  for(string sel : split(sections[2], ' ')){
    MUST(validateSelector(sel))
    LOG("Selector '%s' OK!\n", sel.c_str())
    //selectors.emplace_back( Selector(sel) );
  }

  return true;
}

#ifdef complete
/** -----------------------------------------------------
 * getter for Relation
 *
 * @params index
 */
Relation* Query::getRelation(const uint64_t index){
  return relations[index];
}
/** -----------------------------------------------------
 * getter for Predicate
 *
 * @params index
 */
Predicate* Query::getPredicate(const uint64_t index){
  return predicates[index];
}
/** -----------------------------------------------------
 * getter for Filter
 *
 * @params index
 */
Filter* Query::getfilter(const uint64_t index){
  return filters[index];
}
/** -----------------------------------------------------
 * getter for Selector
 *
 * @params index
 */
Selector* Query::getSelector(const uint64_t index){
  return selectors[index];
}
#endif
