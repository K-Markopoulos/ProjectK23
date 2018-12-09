#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "../inc/query.hpp"
#include "../inc/radix.h"


class Intermediate{
  std::vector<uint64_t> relationsIds;
  std::vector<bool> loaded;
  std::vector<std::vector<uint64_t>> rowIds;
  public:
    Intermediate(const Query& query);
    // get column from intermediate
    std::vector<uint64_t>* getColumn(int id);
    // update column in intermediate
    void updateColumn(int col, std::vector<uint64_t>* new_column);
    // checks if is loaded in intermediate
    bool isLoaded(int id);
    // build relation with [rowId,value] tuples
    relation* buildRelation(int id, int col);
};

class IntermediateList{
  std::vector<Intermediate> list;
  const Query& query;
  public:
    IntermediateList(const Query& query_);
    // get Intermediate by id
    Intermediate* getIntermediate(int id);
    // get Intermediate by relation id
    Intermediate* getIntermediateByRel(int relId);
    // get Intermediate count
    uint64_t getIntermediateCount();
    // create new intermediate
    Intermediate* createIntermediate();
};
