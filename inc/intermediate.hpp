#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "../inc/query.hpp"
#include "../inc/radix.h"


class Intermediate{
  int _id;
  std::vector<uint64_t> relationsIds;
  std::vector<bool> loaded;
  std::vector<std::vector<uint64_t>> rowIds;
  public:
    Intermediate(const Query& query);
    // get column from intermediate
    std::vector<uint64_t>* getColumn(int id);
    // update intermediate based on new column
    void update(int col, std::vector<uint64_t>* new_column);
    // update intermediate based on struct result
    void update(int col1, int col2, result* results);
    // update column in intermediate
    void updateColumn(int col, std::vector<uint64_t>* new_column);
    // join two column in intermediate
    void join(int relId1, int col1, Relation* relation1, int relId2, int col2, Relation * relation2);
    // checks if is loaded in intermediate
    bool isLoaded(int id);
    // build relation with [rowId,value] tuples
    relation* buildRelation(Relation* rel, int id, int col);
    // print intermediate for debuging
    void print();
};

class IntermediateList{
  std::vector<Intermediate*> list;
  std::vector<Intermediate>::iterator active;
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
    ~IntermediateList();
};
