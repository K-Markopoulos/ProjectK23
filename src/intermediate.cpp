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
 * Get Intermediate by id
 *
 * @params id
 * @returns Intermediate*
 */
Intermediate* IntermediateList::getIntermediate(int id){
  assert(id < getIntermediateCount());
  return &list[id];
}

/** -----------------------------------------------------
 * Get Intermediate by relation id
 *
 * @params id
 * @returns Intermediate*
 */
Intermediate* IntermediateList::getIntermediateByRel(int relId){
  uint64_t id = 0;
  for(Intermediate& intermediate : list){
    if(intermediate.isLoaded(relId)){
      return &intermediate;
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
  list.emplace_back(Intermediate(query));
  return &list[list.size()-1];
}

/** -----------------------------------------------------
 * Intermediate constructor
 *
 */
Intermediate::Intermediate(const Query& query){
  Relation* rel;
  int i = 0;
  while(rel = query.getRelation(i++)){
    relationsIds.push_back(rel->getId());
  }
  loaded.assign(i-1, false);
  rowIds.resize(i-1);
  LOG("\t\t+Creating new intermediate size:%lu\n", rowIds.size());
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
  LOG("\t\t^Updating intermediate based on column %d\n", col);
  assert(col < rowIds.size());
  assert(loaded[col]);

  if(!new_column->size()){
    LOG("\t\t!No matches for intermediate\n");
    for(uint64_t c = 0; c < relationsIds.size(); c++)
      if(loaded[c]) rowIds[c].clear();
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
}

/** ----------------------------------------------------- TODOOOOOOOOOO
 * Update intermediate based on col and struct result
 *
 * @params col
 * @params results, struct result (returned fron RadixHashJoin)
 */
void Intermediate::update(int col1, int col2, result* results){
  LOG("\t\t^Updating intermediate based on columns %d,%d and struct result\n", col1, col2);
  assert(col1 < rowIds.size());
  assert(col2 < rowIds.size());

  if(!results->num_tuples){
    LOG("\t\t!No matches for intermediate\n");
    for(uint64_t c = 0; c < relationsIds.size(); c++)
      if(loaded[c]) rowIds[c].clear();
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
    int col = loaded[col1] ? col1 : col2;
    vector<vector<uint64_t>> new_rowIds;
    new_rowIds.resize(rowIds.size());
    for(vector<uint64_t> v : new_rowIds)
      v.reserve(results->num_tuples);
    uint64_t r = 0;

    for(uint64_t t = 0; t < results->num_tuples; t++){
      tuple_* tuple = getNthResult(results, t);
      //r = 0;
      while(rowIds[col][r++] != (col==col1?tuple->key:tuple->payload));
      for(uint64_t c = 0; c < rowIds.size(); c++){
        if(loaded[c])
          new_rowIds[c].push_back(rowIds[c][r]);
        else if(c==col1)
          new_rowIds[c].push_back(tuple->key);
        else if(c==col1)
          new_rowIds[c].push_back(tuple->payload);
      }
    }
    rowIds = new_rowIds;
  }
  loaded[col1] = true;
  loaded[col2] = true;
  LOG("\t\t^Updated\n");
  //print();
}

/** -----------------------------------------------------
 * Update column in intermediate
 *
 * @params id
 * @params new_column, vector of uint64_t
 */
void Intermediate::updateColumn(int col, std::vector<uint64_t>* new_column){
  LOG("\t\t^Updating intermediate column %d\n", col);
  assert(col < rowIds.size());
  loaded[col] = true;
  if(new_column->size())
    rowIds[col] = *new_column;
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
relation* Intermediate::buildRelation(int id, int col){
  LOG("\t\tbuilding relation %d fromn intermediate\n", id);
  assert(id < rowIds.size());
  assert(isLoaded(id));

  Relation* rel = db->getRelation(id);
  relation* res = (relation*) malloc(sizeof(relation));
  res->tuples = (tuple_*) malloc(rowIds[id].size()*sizeof(tuple_));
  res->num_tuples = rowIds[id].size();
  for(int i = 0; i < rowIds[id].size(); i++){
    //LOG("\t\tgetting tuple (%lu)\n", rowIds[id][i]);
    res->tuples[i].key = rowIds[id][i];
    res->tuples[i].payload = rel->getTuple(col, rowIds[id][i]);
  }
  return res;
}

/** -----------------------------------------------------
 * Pretty print for debuging
 *
 */
void Intermediate::print(){
  printf("\n**** Intermediate Print *****\n");
  for(uint64_t id : relationsIds)
    printf("%lu-", id);
  printf("\n");
  for(bool load : loaded)
    printf("%c-", load ? 'Y':'N');
  printf("\n_____________________________\n");

  int64_t col = -1;
  for(uint64_t i = 0; i < loaded.size(); i++){
    if(loaded[i]){
      col = i;
      break;
    }
  }

  if(col == -1){
    printf("-- empty --\n");
    printf("**** /Intermediate Print *****\n\n");
    return;
  }

  for(uint64_t r = 0; r < rowIds[col].size(); r++){
    for(uint64_t c = 0; c < rowIds.size(); c++){
      if(loaded[c])
        printf("%lu|", rowIds[c][r]);
      else
        printf("-|");
    }
    printf("\n");
  }
  printf("**** /Intermediate Print *****\n\n");
}
