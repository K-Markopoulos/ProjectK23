#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <assert.h>
#include <time.h>
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
 * Merge intermediates based on col and struct result
 *
 * @params i1, first intermediate result
 * @params i2, second intermediate result
 * @params col1
 * @params col2
 * @params results, struct result (returned from RadixHashJoin)
 */
void IntermediateList::merge(Intermediate* i1, Intermediate* i2, int col1, int col2, result* results){
  LOG("\t\t^Merging intermediates %d-%d based on columns %d,%d and struct result\n", i1->_id, i2->_id, col1, col2);
  LOG("\t\tmatching %lu rows\n", results->num_tuples);
  assert(col1 < i1->cSize());
  assert(col2 < i2->cSize());
  clock_t start = clock();


  list.push_back(new Intermediate(query));
  Intermediate* mergedI = list[list.size()-1];

  if(!results->num_tuples) {
    for(uint64_t c = 0; c < i1->cSize(); c++) {
      if(i1->isLoaded(c)) mergedI->setLoaded(c);
      if(i2->isLoaded(c)) mergedI->setLoaded(c);
    }
    mergedI->setLoaded(col1);
    mergedI->setLoaded(col2);

    LOG("\t\t!No matches for intermediate\n");
    for(int i = 0; i < list.size(); i++) {
      if(list[i] == i1 || list[i] == i2){
        LOG("Erasing intermediate %d\n",i);
        list.erase(list.begin() + i--);
      }
    }
    return;
  }

  mergedI->reserve(results->num_tuples);

  uint64_t r = 0;
  tuple_* tuple;
  initIterator(results);
  while(tuple=getResult(results)){
    mergedI->push(col1, i1->get(col1, tuple->key));
    mergedI->push(col2, i2->get(col2, tuple->payload));
    for(uint64_t c = 0; c < i1->cSize(); c++) {
      if(i1->isLoaded(c) && c!=col1 && c!=col2) {
        mergedI->push(c, i1->get(c, tuple->key));
      }
      if(i2->isLoaded(c) && c!=col1 && c!=col2) {
        mergedI->push(c, i2->get(c, tuple->payload));
      }
    }
  }

  for(uint64_t c = 0; c < i1->cSize(); c++) {
    if(i1->isLoaded(c) || i2->isLoaded(c))
      mergedI->setLoaded(c);
  }
  mergedI->setLoaded(col1);
  mergedI->setLoaded(col2);

  for(int i = 0; i < list.size(); i++) {
    if(list[i] == i1 || list[i] == i2){
      LOG("Erasing intermediate %d\n",i);
      list.erase(list.begin() + i--);
    }
  }
  elapsed.intermediate_update += (double)(clock() - start) / CLOCKS_PER_SEC;
  LOG("\t\t^Merged %d rows list size %lu\n", mergedI->rSize(), list.size());
  mergedI->print();
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
 * Get column from intermediate
 *
 * @params id
 * @returns column
 */
std::vector<uint64_t>* Intermediate::getColumn(int id){
  assert(id < rowIds.size());
  return &rowIds[id];
}

/** -----------------------------------------------------
 * Get column size
 *
 * @returns cSize
 */
int Intermediate::cSize(){
  return rowIds.size();
}

/** -----------------------------------------------------
 * Get rows size
 *
 * @returns rSize
 */
int Intermediate::rSize(){
  return rowIds[0].size();
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
  clock_t start = clock();

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
  elapsed.intermediate_update += (double)(clock() - start) / CLOCKS_PER_SEC;
}

/** -----------------------------------------------------
 * Update intermediate based on col and struct result
 *
 * @params col
 * @params results, struct result (returned fron RadixHashJoin)
 */
void Intermediate::update(int col1, int col2, result* results){
  LOG("\t\t^Updating intermediate %d based on columns %d,%d and struct result\n", _id, col1, col2);
  LOG("\t\tmatching %lu rows\n", results->num_tuples);
  clock_t start = clock();

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
    tuple_* tuple;
    initIterator(results);
    rowIds[col1].reserve(results->num_tuples);
    rowIds[col2].reserve(results->num_tuples);
    while(tuple=getResult(results)){
      rowIds[col1].push_back(tuple->key);
      rowIds[col2].push_back(tuple->payload);
    }
  } else {
    vector<vector<uint64_t>> new_rowIds;
    new_rowIds.resize(rowIds.size());
    for(int i = 0; i < new_rowIds.size(); i ++)
      new_rowIds[i].reserve(results->num_tuples);
    uint64_t r = 0;
    tuple_* tuple;
    initIterator(results);
    while(tuple=getResult(results)){
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
  elapsed.intermediate_update += (double)(clock() - start) / CLOCKS_PER_SEC;
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
  clock_t start = clock();
  assert(col < rowIds.size());
  loaded[col] = true;
  if(new_column->size())
    rowIds[col] = *new_column;
  LOG("\t\t^Updated\n");
  elapsed.intermediate_update += (double)(clock() - start) / CLOCKS_PER_SEC;
}

/** -----------------------------------------------------
 * Join two columns in intermediate
 *
 * @params col1
 * @params col2
 */
void Intermediate::join(int relId1, int col1, Relation* relation1, int relId2, int col2, Relation* relation2){
  LOG("\t\t>< Joining intermediate %d columns %d-%d\n", _id, col1, col2);
  clock_t start = clock();
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
elapsed.intermediate_update += (double)(clock() - start) / CLOCKS_PER_SEC;
}

/** -----------------------------------------------------
 * Checks if relation is loaded in intermediate results
 *
 * @params id, id of relation
 * @returns true if it's loaded, else false
 */
bool Intermediate::isLoaded(int c){
  assert(c < loaded.size());
  return loaded[c];
}

/** -----------------------------------------------------
 * Set column as loaded
 *
 * @params c, num of column
 */
inline void Intermediate::setLoaded(int c){
  assert(c < loaded.size());
  loaded[c] = true;
}

/** -----------------------------------------------------
 * Get id in intermediate result
 *
 * @params r, num of row
 * @params c, num of column
 * @returns id, id in r-th row and c-th column
 */
uint64_t Intermediate::get(int c, int r){
  assert(c < rowIds.size());
  assert(r < rowIds[c].size());
  return rowIds[c][r];
}

/** -----------------------------------------------------
 * Set id in intermediate result
 *
 * @params r, num of row
 * @params c, num of column
 * @params value, value to set in r-th row and c-th column
 */
inline void Intermediate::set(int c, int r, uint64_t value){
  assert(c < rowIds.size());
  assert(r < rowIds[c].size());
  rowIds[c][r] = value;
}

/** -----------------------------------------------------
 * Push id in c-th column in intermediate result
 *
 * @params c, num of column
 * @params value, value to push back in c-th column
 */
inline void Intermediate::push(int c, uint64_t value){
  // assert(c < rowIds.size());
  rowIds[c].push_back(value);
}

/** -----------------------------------------------------
 * Reserve id in c-th column in intermediate result
 *
 * @params rSize, num of rows to reserve
 */
void Intermediate::reserve(int rSize){
  for(int c = 0; c < rowIds.size(); c++)
    rowIds[c].reserve(rSize);
}

/** -----------------------------------------------------
 * Build relation of [rowId,value] tuples
 *
 * @params id, id of relation
 * @returns relation*, (allocated) to be used in radixHashJoin
 */
relation* Intermediate::buildRelation(Relation* rel, int id, int col){
  LOG("\t\tbuilding relation %d(%lu) from intermediate %d\n", id, rel->getId(), _id);
  clock_t start = clock();

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
  elapsed.intermediate_build += (double)(clock() - start) / CLOCKS_PER_SEC;
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
