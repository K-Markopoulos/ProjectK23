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
    int _id;
    Intermediate(const Query& query);
    // get column from intermediate
    std::vector<uint64_t>* getColumn(int id);
    // get columns size
    int cSize();
    // get rows size
    int rSize();
    // update intermediate based on new column
    void update(int col, std::vector<uint64_t>* new_column);
    // update intermediate based on struct result
    void update(int col1, int col2, result* results);
    // update column in intermediate
    void updateColumn(int col, std::vector<uint64_t>* new_column);
    // join two column in intermediate
    void join(int relId1, int col1, Relation* relation1, int relId2, int col2, Relation * relation2);
    // checks if is loaded in intermediate
    bool isLoaded(int c);
    // set column as loaded
    void setLoaded(int c);
    // Get id in intermediate result
    uint64_t get(int r, int c);
    // Set id in intermediate result
    void set(int r, int c, uint64_t value);
    // Push id in c-th column in intermediate result
    void push(int c, uint64_t value);
    // Reserve space in rowIds
    void reserve(int rSize);
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
    // merge two intermediates into new one
    void merge(Intermediate* i1, Intermediate* i2, int col1, int col2, result* results);
    ~IntermediateList();
};
