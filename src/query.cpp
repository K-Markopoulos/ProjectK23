#include <iostream>
#include <string>
#include <vector>
#include <stdbool.h>
#include "../inc/database.hpp"
#include "../inc/query.hpp"
#include "../inc/utils.hpp"


/** -----------------------------------------------------
 * parse a query, populate relations, predicates, filters and selectors of query
 *
 * @params line, [FROM]|[WHERE]|[SELECT]
 */
bool Query::parseQuery(const string line){

  //split by '|'
  vector<string> sections = split(line, '|');

  MUST(validateSections(sections))
  LOG("sections OK!\n");
  //add relations
  for(string rel : split(sections[0], ' ')){
    MUST(validateRelation(rel))
    LOG("Relation '%s' OK!\n", rel.c_str());
    //relations.push_back(db->getRelation(stoi(rel)));
  }

  //add predicates and filters
  for(string pred : split(sections[1], '&')){
    MUST(validatePredicate(pred))
    LOG("Predicate '%s' OK!\n", pred.c_str());
    // if(isFilter(pred))
    //   filters.emplace_back(Filter(pred));
    // else
    //   predicates.emplace_back(Predicate(pred));
  }

  //add selectors
  for(string sel : split(sections[2], ' ')){
    MUST(validateSelector(sel))
    LOG("Selector '%s' OK!\n", sel.c_str());
    //selectors.emplace_back( Selector(sel) );
  }

  return true;
}

/** -----------------------------------------------------
* getter for Relation
*
* @params index
*/
Relation* Query::getRelation(const int index){
  return relations[index];
}
/** -----------------------------------------------------
* getter for Predicate
*
* @params index
*/
Predicate* Query::getPredicate(const int index){
  return &predicates[index];
}
/** -----------------------------------------------------
* getter for Filter
*
* @params index
*/
Filter* Query::getFilter(const int index){
  return &filters[index];
}
/** -----------------------------------------------------
* getter for Selector
*
* @params index
*/
Selector* Query::getSelector(const int index){
  return &selectors[index];
}
/** -----------------------------------------------------
 * Clears query data
 *
 */
void Query::clear(){
  this->relations.clear();
  this->predicates.clear();
  this->filters.clear();
  this->selectors.clear();
  return;
}

/** -----------------------------------------------------
 * Predicate constructor
 *
 */
Predicate::Predicate(string predicate){
  size_t pos_op = predicate.find_first_of("<>="),
      pos_dot = predicate.find('.');

  uint64_t id1 = stoi(predicate.substr(0, pos_dot));
  this->relation1 = db->getRelation(id1);
  this->col1 = stoi(predicate.substr(pos_dot+1));

  this->op = predicate[pos_op];
  pos_dot = predicate.find('.', pos_op);

  uint64_t id2 = stoi(predicate.substr(pos_op+1, pos_dot));
  this->relation2 = db->getRelation(id2);
  this->col2 = stoi(predicate.substr(pos_dot));
}

/** -----------------------------------------------------
 * Filter constructor
 *
 */
Filter::Filter(string filter){
  size_t pos_op = filter.find_first_of("<>="),
      pos_dot = filter.find('.');
  uint64_t id;

  if(pos_dot < pos_op){
    id = stoi(filter.substr(0, pos_dot));
    this->col = stoi(filter.substr(pos_dot+1, pos_op-pos_dot-1));
    this->value = stoi(filter.substr(pos_op+1));
  } else {
    id = stoi(filter.substr(pos_op+1,pos_dot-pos_op-1));
    this->col = stoi(filter.substr(pos_dot+1));
    this->value = stoi(filter.substr(0, pos_op));
  }

  this->op = filter[pos_op];
  this->relation = db->getRelation(id);
}

/** -----------------------------------------------------
 * Selector constructor
 *
 */
Selector::Selector(string selector){
  size_t pos_dot = selector.find('.');
  uint64_t id = stoi(selector.substr(0, pos_dot));
  this->col = stoi(selector.substr(pos_dot+1));
  this->relation = db->getRelation(id);
}
