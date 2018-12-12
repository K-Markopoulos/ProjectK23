#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include "../inc/database.hpp"
#include "../inc/intermediate.hpp"
#include "../inc/relation.hpp"
#include "../inc/query.hpp"
#include "../inc/utils.hpp"
#include "../inc/result.h"

using namespace std;

/** -----------------------------------------------------
 * Adds a new relation in DB, specifying path
 *
 * @params fileName, fileName where relation is stored
 */
void Database::addRelation(const string fileName){
  if(!file_exists(fileName)){
    LOG("File not found: %s\n", fileName.c_str());
    return;
  }

  this->relations.emplace_back(Relation(fileName));
  return;
}

/** -----------------------------------------------------
 * Gets a relation from DB
 *
 * @params id, the histogram
 * @returns relation
 */
Relation* Database::getRelation(int id){
  if(id < this->getRelationsCount())
    return &this->relations[id];
  LOG("ERROR: Database::getRelation out of border\n");
  return NULL;
}

/** -----------------------------------------------------
 * Gets the relations count in DB
 *
 * @returns count, integer
 */
size_t Database::getRelationsCount(){
  return this->relations.size();
}

/** -----------------------------------------------------
 * Runs the query using RadixHashJoin and returns the result
 *
 * @params query
 * @returns char*, result from query
 */
string Database::run(const Query& query){
  //  initialize Intermmediate results
  LOG("Running query \n");
  IntermediateList intermediateList = IntermediateList(query);

  //  run filters
  const Filter* filter;
  int fCount = 0;
  while(filter = query.getFilter(fCount++)){
    runFilter(filter, intermediateList);
  }

  //  run predicates
  const Predicate* predicate;
  int pCount = 0;
  while(predicate = query.getPredicate(pCount++)){
    runPredicate(predicate, intermediateList);
  }

  string response;
  //  run selectors (sums)
  const Selector* selector;
  int sCount = 0;
  while(selector = query.getSelector(sCount++)){
    string temp_response = runSelector(selector, intermediateList);
    response += (temp_response.empty()? string("NULL"):temp_response) + ' ';
  }

  return response;

  // LOG("DONE! ----------- SHOWING INTERMEDIATES -----------\n");
  // for(int i = 0; i < intermediateList.getIntermediateCount(); i++){
  //   intermediateList.getIntermediate(i)->print();
  // }
}

/** -----------------------------------------------------
 * Compares 2 values based on operator.
 * This is a helper for runFilter function.
 *
 * @params op, character within ['>','<','=']
 * @params v1, first value
 * @params v2, second value
 */
inline bool op(char op, uint64_t v1, uint64_t v2){
  switch(op){
    case '>':
      return v1 > v2; break;
    case '<':
      return v1 < v2; break;
    case '=':
      return v1 == v2; break;
  }
}

/** -----------------------------------------------------
 * Runs the filter using RadixHashJoin and returns the result
 *
 * @params filter, results
 * @params results, IntermediateList
 */
void Database::runFilter(const Filter* filter, IntermediateList& results){
  LOG("\tRunning filter ..%c%ld\n",filter->op, filter->value);

  Intermediate* intermediate = results.getIntermediateByRel(filter->relId);
  if(intermediate){
    // pull data from intermediate
    vector<uint64_t>* column = intermediate->getColumn(filter->relId);
    vector<uint64_t> new_column;
    // compare
    for(int t = 0; t < column->size(); t++){
      int64_t value = filter->relation->getTuple(filter->col, (*column)[t]);
      if(op(filter->op, value, filter->value)){
        new_column.push_back((*column)[t]);
      }
      // else { LOG("\t\trowId:%lu, value:%ld NOOOOOOOOOOT\n", (*column)[t], value);}
    }
    // update intermediate
    intermediate->update(filter->relId, &new_column);
  } else {
    // create new intermediate
    intermediate = results.createIntermediate();
    vector<uint64_t> new_column;
    // pull data from relation
    for(int t = 0; t < filter->relation->getTupleCount(); t++){
      int64_t value = filter->relation->getTuple(filter->col, t);
      if(op(filter->op, value, filter->value)){
        // push to new intermediate
        new_column.push_back(t);
      }
      // else { LOG("\t\trowId:%d, value:%ld N000000000000T\n", t, value);}
    }
    intermediate->updateColumn(filter->relId, &new_column);
  }

}

/** -----------------------------------------------------
 * Runs the predicate and merges the result into Intermediate results
 *
 * @params predicate, Predicate to run
 * @params results, IntermediateList
 */
void Database::runPredicate(const Predicate* predicate, IntermediateList& results){
  LOG("Running predicate %lu.%lu%c%lu.%lu\n", predicate->relId1, predicate->col1, predicate->op, predicate->relId2, predicate->col2);
  if (predicate->relId1 == predicate->relId2){
    LOG("\tIts a self join\n");
    //self join
    Intermediate* intermediate = results.getIntermediateByRel(predicate->relId1);
    if(intermediate){
      vector<uint64_t>* column = intermediate->getColumn(predicate->relId1);
      vector<uint64_t> new_column;

      for(uint64_t t = 0; t < column->size(); t++)
          if(predicate->relation1->getTuple(predicate->col1, (*column)[t]) ==
          predicate->relation2->getTuple(predicate->col2, (*column)[t])){
            //LOG("\tmatching row %lu\n", (*column)[t]);
            new_column.push_back((*column)[t]);
          }
      intermediate->update(predicate->relId1, &new_column);
    } else {
      intermediate = results.createIntermediate();
      vector<uint64_t> new_column;

      for(uint64_t t = 0; t < predicate->relation1->getTupleCount(); t++)
          if(predicate->relation1->getTuple(predicate->col1, t) ==
          predicate->relation2->getTuple(predicate->col2, t)){
            //LOG("\tmatching row %lu\n", t);
            new_column.push_back(t);
          }
      intermediate->updateColumn(predicate->relId1, &new_column);
    }
  } else {
    // radix hash join
    LOG("\tIts a join\n");
    relation* rel1, *rel2;
    Intermediate* intermediate1, * intermediate2;

    rel1 = (intermediate1 = results.getIntermediateByRel(predicate->relId1))?
      intermediate1->buildRelation(predicate->relation1, predicate->relId1, predicate->col1):
      predicate->relation1->buildRelation(predicate->col1);
    rel2 = (intermediate2 = results.getIntermediateByRel(predicate->relId2))?
      intermediate2->buildRelation(predicate->relation2, predicate->relId2, predicate->col2):
      predicate->relation2->buildRelation(predicate->col2);

    result* res = radixHashJoin(rel1, rel2);

    LOG("\tmatching %lu rows\n", res->num_tuples);

    if (!intermediate1 && !intermediate2){
      LOG("\tno existing intermediate\n");
      intermediate1 = results.createIntermediate();
      intermediate1->update(predicate->relId1, predicate->relId2, res);
    } else if (intermediate1 && !intermediate2) {
      LOG("\tfound intermediate1, updating 1\n");
      intermediate1->update(predicate->relId1, predicate->relId2, res);
    } else if (!intermediate1 && intermediate2) {
      LOG("\tfound intermediate2, updating 2\n");
      intermediate2->update(predicate->relId1, predicate->relId2, res);
    } else{
      LOG("\tfound both intermediate1 and intermediate2, updating both (should I?)\n");
      intermediate1->update(predicate->relId1, predicate->relId2, res);
      intermediate2->update(predicate->relId1, predicate->relId2, res);
    }
  }
}

string Database::runSelector(const Selector* selector, IntermediateList& results){
  LOG("Running selector %lu.%lu\n", selector->relId, selector->col);

  Intermediate* intermediate = results.getIntermediateByRel(selector->relId);
  uint64_t sum = 0;

  if(intermediate){
    vector<uint64_t>* column = intermediate->getColumn(selector->relId);
    if(column->size() == 0)
      return "";
    for(int t = 0; t < column->size(); t++)
      sum += selector->relation->getTuple(selector->col, (*column)[t]);
  } else {
    LOG("\tOOPS retreiving raw relation from db (not need to have run predicates on that relation)\n");
    for(int t = 0; t < selector->relation->getTupleCount(); t++)
      sum += selector->relation->getTuple(selector->col, t);
  }
  LOG("\t>sum:%lu\n", sum);
  return to_string(sum);
}
