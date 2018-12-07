#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "../inc/query.hpp"
#include "../inc/relation.hpp"

class Database;

extern Database* db;

class Database{
  std::vector<Relation> relations;

  public:
    // add relation
    void addRelation(const std::string fileName);
    // get relation
    Relation* getRelation(int id);
    // get the count of relations
    size_t getRelationsCount();
    // run a query
    char* run(Query query);
};
