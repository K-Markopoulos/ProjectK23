#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "../inc/query.hpp"
#include "../inc/relation.hpp"
#include "../inc/intermediate.hpp"

class Database;

extern Database* db;

class Database{
  std::vector<Relation*> relations;

  public:
    // add relation
    void addRelation(const std::string fileName);
    // get relation
    Relation* getRelation(int id);
    // get the count of relations
    size_t getRelationsCount();
    // run a query
    std::string run(Query& query);
    // run a filter
    void runFilter(const Filter* filter, IntermediateList& results);
    // run a predicate
    void runPredicate(const Predicate* predicate, IntermediateList& results);
    // run a selector
    std::string runSelector(const Selector* selector, IntermediateList& results);
    ~Database();
};
