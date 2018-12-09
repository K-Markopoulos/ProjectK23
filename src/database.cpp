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
char* Database::run(const Query& query){
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

  // //  run selectors  (sums)


  //NOT IMPLEMENTED
  //shhhhhhhhhhhh
  cout << "Running ...\n";
  sleep(3);
  cout << "Done\n";
  return NULL;
}

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
 * @returns
 */
void Database::runFilter(const Filter* filter, IntermediateList& results){
  LOG("\tRunning filter ..%c%ld\n",filter->op, filter->value);

  relation* rel1, *rel2;
  Intermediate* intermediate = results.getIntermediateByRel(filter->relId);
  Relation* relation;
  if(intermediate){
    // pull data from intermediate
    vector<uint64_t>* column = intermediate->getColumn(filter->relId);
    vector<uint64_t> new_column;
    // compare
    for(int t = 0; t < column->size(); t++){
      int64_t value = filter->relation->getTuple(filter->col, (*column)[t]);
      if(op(filter->op, value, filter->value)){
        LOG("\t\trowId:%lu, value:%ld\n", (*column)[t], value);
        new_column.push_back((*column)[t]);
      }
      // else { LOG("\t\trowId:%lu, value:%ld NOOOOOOOOOOT\n", (*column)[t], value);}
    }
    // update intermediate
    intermediate->updateColumn(filter->relId, &new_column);
  } else {
    // create new intermediate
    intermediate = results.createIntermediate();
    vector<uint64_t> new_column;
    // pull data from relation
    for(int t = 0; t < filter->relation->getTupleCount(); t++){
      int64_t value = filter->relation->getTuple(filter->col, t);
      if(op(filter->op, value, filter->value)){
        LOG("\t\trowId:%d, value:%ld\n", t, value);
        // push to new intermediate
        new_column.push_back(t);
      }
      // else { LOG("\t\trowId:%d, value:%ld N000000000000T\n", t, value);}
    }
    intermediate->updateColumn(filter->relId, &new_column);
  }

}

/** -----------------------------------------------------
 * Runs the predicate and merges to Intermediate
 *
 * @params predicate, predicate to run
 * @params results, Intermediate results
 */
void Database::runPredicate(const Predicate* predicate, IntermediateList& results){
  LOG("\nRunning predicate %lu.%lu%c%lu.%lu\n", predicate->relId1, predicate->col1, predicate->op, predicate->relId2, predicate->col2);
  if (predicate->relId1 == predicate->relId2){
    //self join
  } else {
    // radix hash join
    relation* rel1, *rel2;
    Intermediate* intermediate;
    rel1 = (intermediate = results.getIntermediateByRel(predicate->relId1))?
      intermediate->buildRelation(predicate->relId1, predicate->col1):
      predicate->relation1->buildRelation(predicate->col1);
    rel2 = (intermediate = results.getIntermediateByRel(predicate->relId2))?
      intermediate->buildRelation(predicate->relId2, predicate->col2):
      predicate->relation2->buildRelation(predicate->col2);

    result* res = radixHashJoin(rel1, rel2);
    printResults(res);
    // merge res to intermediate results
  }
}
