#include <iostream>
#include <string>
#include <vector>
#include "inc/database.h"
#include "inc/query.h"
#include "inc/utils.h"

/** -----------------------------------------------------
 * parse a query, populate relations, predicates, filters and selectors of query
 *
 * @params line, [FROM]|[WHERE]|[SELECT]
 */
void Query::parseQuery(const string line){
  //NOT IMPLEMENTED

  //split by '|'
  vector<string> sections = split(line, '|');

  //add relations
  for(string rel : split(sections[0].' ')){
    relations.emplace_back( Relation(rel) );
  }

  //add predicates and filters
  for(string pred : split(sections[1], '&')){
    if( isFilter(pred) )
      filters.emplace_back( Filter(pred) );
    else
      predicates.emplace_back( Predicate(pred) );
  }

  //add selectors
  for(string sel : split(sections[2], ' ')){
      selectors.emplace_back( Selector(sel) );
  }

  return;
}
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
