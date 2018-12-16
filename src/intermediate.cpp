#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <assert.h>
#include "../inc/database.hpp"
#include "../inc/intermediate.hpp"
#include "../inc/relation.hpp"
#include "../inc/query.hpp"
#include "../inc/utils.hpp"
#include "../inc/radix.h"
#include "../inc/result.h"

using namespace std;

/** -----------------------------------------------------
 * IntermediateList constructor
 *
 */
IntermediateList::IntermediateList(const Query& query_): query(query_){}

/** -----------------------------------------------------
 * IntermediateList destructor
 *
 */
IntermediateList::~IntermediateList(){
  for(Intermediate* intermediate: list)
    delete intermediate;
}

/** -----------------------------------------------------
 * Get Intermediate by id
 *
 * @params id
 * @returns Intermediate*
 */
Intermediate* IntermediateList::getIntermediate(int id){
  assert(id < getIntermediateCount());
  return list[id];
}

/** -----------------------------------------------------
 * Get Intermediate by relation id
 *
 * @params id
 * @returns Intermediate*
 */
Intermediate* IntermediateList::getIntermediateByRel(int relId){
  uint64_t id = 0;
  for(Intermediate* intermediate : list){
    if(intermediate->isLoaded(relId)){
      return intermediate;
    }
    id++;
  }
  return NULL;
}

/** -----------------------------------------------------
 * Get Intermediate count
 *
 * @returns count
 */
uint64_t IntermediateList::getIntermediateCount(){
  return list.size();
}

/** -----------------------------------------------------
 * Create new intermediate
 *
 * @returns intermediate, the new one
 */
Intermediate* IntermediateList::createIntermediate(){
  list.push_back(new Intermediate(query));
  return list[list.size()-1];
}

/** -----------------------------------------------------
 * Intermediate constructor
 *
 */
Intermediate::Intermediate(const Query& query){
  Relation* rel;
  int i = 0;
  static int id = 0;
  _id = id++;
  while(rel = query.getRelation(i++)){
    relationsIds.push_back(rel->getId());
  }
  loaded.assign(i-1, false);
  rowIds.resize(i-1);
  LOG("\t\t+Creating new intermediate %d\n", _id);
}

/** -----------------------------------------------------
 * Get column fron intermediate
 *
 * @params id
 * @returns column
 */
std::vector<uint64_t>* Intermediate::getColumn(int id){
  assert(id < rowIds.size());
  return &rowIds[id];
}

/** -----------------------------------------------------
 * Update intermediate base on column
 *
 * @params id
 * @params new_column, vector of uint64_t
 */
void Intermediate::update(int col, std::vector<uint64_t>* new_column){
  LOG("\t\t^Updating intermediate %d based on column %d\n", _id, col);
  LOG("\t\tmatching %lu rows\n", new_column->size());
  assert(col < rowIds.size());
  assert(loaded[col]);

  if(!new_column->size()){
    LOG("\t\t!No matches for intermediate\n");
    for(uint64_t c = 0; c < relationsIds.size(); c++)
      if(loaded[c]) rowIds[c].clear();
    loaded[col] = true;
    return;
  }

  bool should_erase;

  for(uint64_t r = 0; r < new_column->size(); r++){
    should_erase = rowIds[col][r] == new_column->at(r);
    for(uint64_t c = 0; c < rowIds.size(); c++){
      if(loaded[c] && should_erase){
        rowIds[c].erase(rowIds[c].begin() + r--);
      }
    }
  }

  LOG("\t\t^Updated\n");
  print();
}

/** ----------------------------------------------------- TODOOOOOOOOOO
 * Update intermediate based on col and struct result
 *
 * @params col
 * @params results, struct result (returned fron RadixHashJoin)
 */
void Intermediate::update(int col1, int col2, result* results){
  LOG("\t\t^Updating intermediate %d based on columns %d,%d and struct result\n", _id, col1, col2);
  LOG("\t\tmatching %lu rows\n", results->num_tuples);
  assert(col1 < rowIds.size());
  assert(col2 < rowIds.size());

  if(!results->num_tuples){
    LOG("\t\t!No matches for intermediate\n");
    for(uint64_t c = 0; c < relationsIds.size(); c++)
      if(loaded[c]) rowIds[c].clear();
    loaded[col1] = true;
    loaded[col2] = true;
    return;
  }

  // if none loaded there its a new intermediate
  if (!loaded[col1] && !loaded[col2]){
    for(uint64_t r = 0; r < results->num_tuples; r++){
      tuple_* tuple = getNthResult(results, r);
      rowIds[col1].push_back(tuple->key);
      rowIds[col2].push_back(tuple->payload);
    }
  } else {
    vector<vector<uint64_t>> new_rowIds;
    new_rowIds.resize(rowIds.size());
    for(vector<uint64_t> v : new_rowIds)
      v.reserve(results->num_tuples);
    uint64_t r = 0;
    for(uint64_t t = 0; t < results->num_tuples; t++){
      tuple_* tuple = getNthResult(results, t);
      new_rowIds[col1].push_back(loaded[col1]? rowIds[col1][tuple->key]: tuple->key);
      new_rowIds[col2].push_back(loaded[col2]? rowIds[col2][tuple->payload]: tuple->payload);
      for(uint64_t c = 0; c < rowIds.size(); c++){
        if(loaded[c] && c!=col1 && c!=col2){
          new_rowIds[c].push_back(rowIds[c][tuple->key]);
        }
      }
    }
    rowIds = new_rowIds;
  }
  loaded[col1] = true;
  loaded[col2] = true;
  LOG("\t\t^Updated %lu rows\n", rowIds[col2].size());
  print();
}

/** -----------------------------------------------------
 * Update column in intermediate
 *
 * @params id
 * @params new_column, vector of uint64_t
 */
void Intermediate::updateColumn(int col, std::vector<uint64_t>* new_column){
  LOG("\t\t^Updating intermediate %d column %d\n", _id, col);
  LOG("\t\tmatching %lu rows\n", new_column->size());
  assert(col < rowIds.size());
  loaded[col] = true;
  if(new_column->size())
    rowIds[col] = *new_column;
  LOG("\t\t^Updated\n");
  print();
}

/** -----------------------------------------------------
 * Join two columns in intermediate
 *
 * @params col1
 * @params col2
 */
void Intermediate::join(int relId1, int col1, Relation* relation1, int relId2, int col2, Relation* relation2){
  LOG("\t\t>< Joining intermediate %d columns %d-%d\n", _id, col1, col2);
  assert(col1 < rowIds.size());
  assert(col2 < rowIds.size());

  vector<vector<uint64_t>> new_rowIds;
  new_rowIds.resize(rowIds.size());

  for(uint64_t r = 0; r < rowIds[relId1].size(); r++){
    if(relation1->getTuple(col1,rowIds[relId1][r]) == relation2->getTuple(col2,rowIds[relId2][r])){
      for(uint64_t c = 0; c < rowIds.size(); c++)
        if(loaded[c])
          new_rowIds[c].push_back(rowIds[c][r]);
    }
  }
  rowIds = new_rowIds;

LOG("\t\t^Updated %lu rows\n", rowIds[col1].size());
}

/** -----------------------------------------------------
 * Checks if relation is loaded in intermediate results
 *
 * @params id, id of relation
 * @returns true if it's loaded, else false
 */
bool Intermediate::isLoaded(int id){
  assert(id < loaded.size());
  return loaded[id];
}

/** -----------------------------------------------------
 * Build relation of [rowId,value] tuples
 *
 * @params id, id of relation
 * @returns relation*, (allocated) to be used in radixHashJoin
 */
relation* Intermediate::buildRelation(Relation* rel, int id, int col){
  LOG("\t\tbuilding relation %d(%lu) from intermediate %d\n", id, rel->getId(), _id);
  assert(id < rowIds.size());
  assert(isLoaded(id));

  relation* res = (relation*) malloc(sizeof(relation));
  res->tuples = (tuple_*) malloc(rowIds[id].size()*sizeof(tuple_));
  res->num_tuples = rowIds[id].size();
  for(uint64_t i = 0; i < rowIds[id].size(); i++){
    res->tuples[i].key = i;
    res->tuples[i].payload = rel->getTuple(col, rowIds[id][i]);
  }
  LOG("\t\tbuilt relation with size: %lu\n", res->num_tuples);
  return res;
}

/** -----------------------------------------------------
 * Pretty print for debuging
 *
 */
void Intermediate::print(){
  #ifdef PRINT_IR
  printf("\n\t**** Intermediate (%d) Print *****\n\t", _id);
  for(uint64_t id : relationsIds)
    printf("%lu-", id);
  printf("\n\t");
  for(bool load : loaded)
    printf("%c-", load ? 'Y':'N');
  printf("\n\t_____________________________\n\t");

  int64_t col = -1;
  for(uint64_t i = 0; i < loaded.size(); i++){
    if(loaded[i]){
      col = i;
      break;
    }
  }

  if(col == -1){
    printf("-- empty --\n\t");
    printf("**** /Intermediate Print *****\n\n");
    return;
  }

  #ifndef SKIP_DATA
  for(uint64_t r = 0; r < rowIds[col].size() && r < 5; r++){
    for(uint64_t c = 0; c < rowIds.size(); c++){
      if(loaded[c])
        printf("%lu|", rowIds[c][r]);
      else
        printf("-|");
    }
    printf("\n\t");
  }
  #endif
  printf("**** /Intermediate Print *****\n\n");
  #endif
}
