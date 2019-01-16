#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdbool.h>
#include <algorithm>
#include "../inc/database.hpp"
#include "../inc/query.hpp"
#include "../inc/utils.hpp"


/** -----------------------------------------------------
 * split a string based on the delimeter
 *
 * @params line, string to split
 * @params delim, character used as delimeter
 */
bool Query::isFilter(string pred){
  MUST(pred.find('.') == pred.rfind('.')) // only one dot
  return true;
}
/** -----------------------------------------------------
 * validates that sections are as expected
 *
 * @params sections, Query's sections to validate
 * @returns true if valid, else false
 */
bool Query::validateSections(const vector<string>& sections){
  MUST(sections.size() == 3)
  MUST(!sections[0].empty())
  MUST(!sections[1].empty())
  MUST(!sections[2].empty())
  return true;
}

/** -----------------------------------------------------
* validates that relation is as expected
*
* @params relation, Query's relation to validate
* @returns true if valid, else false
*/
bool Query::validateRelation(const string& relation){
  MUST(!relation.empty()) // not empty
  MUST(relation.find_first_not_of("0123456789") == string::npos) // only digits
  MUST(stoi(relation) < db->getRelationsCount()) // relation's index < total relations
  return true;
}

/** -----------------------------------------------------
* validates that predicate is as expected
*
* @params predicate, Query's predicate to validate
* @returns true if valid, else false
*/
bool Query::validatePredicate(const string& predicate){
  MUST(!predicate.empty()) // not empty
  size_t pos_operator, pos_dot;
  MUST((pos_operator=predicate.find_first_of("<>=")) != string::npos) // got an operator
  MUST((pos_operator=predicate.find_first_of("<>=")) == predicate.find_last_of("<>=")) // only one operator
  string part1 = predicate.substr(0, pos_operator);
  string part2 = predicate.substr(pos_operator+1);
  MUST(part1.find_first_not_of("-.0123456789") == string::npos) // only digits and dot
  MUST(part2.find_first_not_of("-.0123456789") == string::npos) // only digits and dot
  int relCount = relations.size();
  bool at_least_one_column = false;
  if((pos_dot=part1.find('.')) != string::npos){
    MUST(pos_dot == part1.rfind('.')) // only one dot
    MUST(pos_dot != 0 && pos_dot != part1.size()-1) // dot not first or last
    int rel = stoi(part1.substr(0,pos_dot));
    MUST(rel >= 0);
    int colCount = relations[rel]->getColumnCount();
    MUST(rel < relCount) // relation's index < total relations
    int col = stoi(part1.substr(pos_dot+1));
    MUST(col >= 0);
    MUST(col < colCount) // relation's column < total relation's columns
    at_least_one_column = true;
  }
  if((pos_dot=part2.find('.')) != string::npos){
    MUST(pos_dot == part2.rfind('.')) // only one dot
    MUST(pos_dot != 0 && pos_dot != part2.size()-1) // dot not first or last
    int rel = stoi(part2.substr(0,pos_dot));
    MUST(rel >= 0);
    int colCount = relations[rel]->getColumnCount();
    MUST(rel < relCount) // relation's index < total relations
    int col = stoi(part2.substr(pos_dot+1));
    MUST(col < colCount) // relation's column < total relation's columns
    at_least_one_column = true;
  }
  MUST(at_least_one_column)

  return true;
}

/** -----------------------------------------------------
* validates that selector is as expected
*
* @params predicate, Query's predicate to validate
* @returns true if valid, else false
*/
bool Query::validateSelector(const string& selector){
  size_t pos_dot;
  MUST(!selector.empty()) // not empty
  MUST(selector.find_first_not_of(".0123456789") == string::npos) // only digits and dot
  MUST((pos_dot=selector.find('.')) != string::npos) // got a dot
  MUST(pos_dot == selector.rfind('.')) // only one dot
  MUST(pos_dot != 0 && pos_dot != selector.size()-1) // dot not first or last
  int relCount = relations.size();
  int rel = stoi(selector.substr(0,pos_dot));
  int colCount = relations[rel]->getColumnCount();
  MUST(rel < relCount) // relation's index < total relations
  MUST(stoi(selector.substr(pos_dot+1)) < colCount) // relation's column < total relation's columns
  return true;
}

/** -----------------------------------------------------
 * parse a query, populate relations, predicates, filters and selectors of query
 *
 * @params line, [FROM]|[WHERE]|[SELECT]
 */
bool Query::parseQuery(const string line){
  LOG("parsing query >%s\n", line.c_str());

  vector<string> sections = split(line, '|');
  MUST(validateSections(sections))
  LOG("sections OK!\n");

  //add relations
  for(string rel : split(sections[0], ' ')){
    MUST(validateRelation(rel))
    LOG("Relation '%s' OK!\n", rel.c_str());
    relations.push_back(db->getRelation(stoi(rel)));
  }

  //add predicates and filters
  for(string pred : split(sections[1], '&')){
    MUST(validatePredicate(pred))
    LOG("Predicate '%s' OK!\n", pred.c_str());
    if(isFilter(pred)){
      filters.push_back(new Filter(pred, this));
    }
    else{
      predicates.push_back(new Predicate(pred, this));
    }
  }

  //add selectors
  for(string sel : split(sections[2], ' ')){
    MUST(validateSelector(sel))
    LOG("Selector '%s' OK!\n", sel.c_str());
    selectors.push_back(new Selector(sel, this));
  }

  return true;
}

/** -----------------------------------------------------
* getter for Relation
*
* @params index
*/
Relation* Query::getRelation(const int index) const{
  if(index < relations.size())
    return relations[index];
  return NULL;
}

/** -----------------------------------------------------
* Get relations count
*
* @returns count
*/
uint64_t Query::getRelationsCount() const{
  return relations.size();
}

/** -----------------------------------------------------
* getter for Predicate
*
* @params index
*/
const Predicate* Query::getPredicate(const int index) const{
  if(index < predicates.size())
    return predicates[index];
  return NULL;
}

/** -----------------------------------------------------
* getter for Filter
*
* @params index
*/
const Filter* Query::getFilter(const int index) const{
  if(index < filters.size())
    return filters[index];
  return NULL;
}

/** -----------------------------------------------------
* getter for Selector
*
* @params index
*/
const Selector* Query::getSelector(const int index) const{
  if(index < selectors.size())
    return selectors[index];
  return NULL;
}

/** -----------------------------------------------------
 * Clears query data
 *
 */
void Query::clear(){
  LOG("Clearing query\n");
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
Predicate::Predicate(string predicate, Query* query){
  size_t pos_op = predicate.find_first_of("<>="),
      pos_dot = predicate.find('.');

  relId1 = stoi(predicate.substr(0, pos_dot));
  this->relation1 = query->getRelation(relId1);
  this->col1 = stoi(predicate.substr(pos_dot+1, pos_op-pos_dot-1));

  this->op = predicate[pos_op];
  pos_dot = predicate.find('.', pos_op);

  relId2 = stoi(predicate.substr(pos_op+1, pos_dot-pos_op-1));
  this->relation2 =query->getRelation(relId2);
  this->col2 = stoi(predicate.substr(pos_dot+1));
}

/** -----------------------------------------------------
 * Filter constructor
 *
 */
Filter::Filter(string filter, Query* query){
  size_t pos_op = filter.find_first_of("<>="),
      pos_dot = filter.find('.');

  if(pos_dot < pos_op){
    relId = stoi(filter.substr(0, pos_dot));
    this->col = stoi(filter.substr(pos_dot+1, pos_op-pos_dot-1));
    this->value = stoi(filter.substr(pos_op+1));
  } else {
    relId = stoi(filter.substr(pos_op+1,pos_dot-pos_op-1));
    this->col = stoi(filter.substr(pos_dot+1));
    this->value = stoi(filter.substr(0, pos_op));
  }

  this->op = filter[pos_op];
  this->relation = query->getRelation(relId);
}

/** -----------------------------------------------------
 * Selector constructor
 *
 */
Selector::Selector(string selector, Query* query){
  size_t pos_dot = selector.find('.');
  relId = stoi(selector.substr(0, pos_dot));
  this->col = stoi(selector.substr(pos_dot+1));
  this->relation = query->getRelation(relId);
}

uint64_t Query::getPredicateCount() const{
  return predicates.size();
}

uint64_t Query::getFilterCount() const{
  return filters.size();
}

// ------------------------ SUBSET FUNCTIONS --------------------------- //
void subsetsUtil(vector<int>& A, vector<vector<int> >& res,
                 vector<int>& subset, int index)
{
    for (int i = index; i < A.size(); i++) {

        // include the A[i] in subset.
        subset.push_back(A[i]);
        res.push_back(subset);

        // move onto the next element.
        subsetsUtil(A, res, subset, i + 1);

        // exclude the A[i] from subset and triggers
        // backtracking.
        subset.pop_back();
    }

    return;
}

// below function returns the subsets of vector A.
vector<vector<int> > subsets(vector<int>& A)
{
    vector<int> subset;
    vector<vector<int> > res;

    // include the null element in the set.
    res.push_back(subset);

    // keeps track of current element in vector A;
    int index = 0;
    subsetsUtil(A, res, subset, index);

    return res;
}
// ------------------------------- END OF SUBSETS ---------------------- //

bool check_connected(uint64_t rel, std::vector<uint64_t> sub, std::vector<std::vector<bool>> connected){
  for(uint64_t i=0; i<sub.size();i++)
    if(connected[rel][sub[i]] == false && connected[sub[i]][rel] == false)
      return false;
  return true;
}

uint64_t Query::cost(std::vector<uint64_t> sequence, Cardinality cardinality){
  uint64_t cost = 0;
  for(uint64_t i=0; i<getPredicateCount(); i++){
    Predicate * p = getPredicate(i);
    for(uint64_t j=0; j<sequence.size()-1; j++)
      if(sequence[j] == p->relId1 && sequence[j+1] == p->relId2 || sequence[j] == p->relId2 && sequence[j+1] == p->relId1)
        cost += cardinality.assess(p);
  }
  return cost;
}

std::vector<uint64_t> createJoinTree(std::vector<uint64_t> t1, std::vector<uint64_t> t2, Cardinality cardinality){
  std::vector<uint64_t> bestTree = t1;
  bestTree.push_back(t2);
  do{
    <std::vector<uint64_t> tree = t2;
    tree.push_back(t1);
    if(cost(bestTree, cardinality) > cost(tree, cardinality))
      bestTree = tree;
  }while(std::next_permutation(t1));
  return bestTree;
}

std::vector<uint64_t> Query::joinEnumeration(){
  uint64_t rel_num = getRelationsCount();
  std::map<std::vector<uint64_t>, std::vector<uint64_t>> BestTree;
  std::vector<std::vector<bool>> connected;
  std::vector<uint64_t> relations;

  Cardinality originalCardinality = Cardinality(this);
  for (Filter* f : filters) {
    originalCardinality.assess(f);
  }

  relations.resize(rel_num);
  connected.resize(rel_num);
  for(uint64_t i=0; i<rel_num; i++){
    connected[i].resize(rel_num);
    relations[i] = i;
    BestTree.insert([i], [i]);
  }
  for(Predicate* p : predicates)
    connected[p->relId1][p->relId2] = true;

  for (uint64_t i = 0; i < rel_num-1; i++)
    for (std::vector<uint64_t> sub : subsets(relations))
      if(sub.size() == i+1)
        for(uint64_t rel : relations){
          if(find(sub.begin(), sub.end(), rel) || !check_connected(rel, sub, connected))
            continue;
          Cardinality cardinality = originalCardinality;
          std::vector<uint64_t> curr_tree = createJoinTree(BestTree[sub], rel, cardinality);
          std::vector<uint64_t> sub_new = sub;
          sub_new.push_back(rel);
          if(BestTree[sub_new] == NULL || cost(BestTree[sub_new], cardinality) > cost(curr_tree, cardinality))
            BestTree[sub_new] = CurrTree;
        }
  return BestTree[relations];
}
