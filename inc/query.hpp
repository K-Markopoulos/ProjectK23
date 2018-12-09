#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <stdbool.h>
#include "../inc/relation.hpp"

class Query;
class Predicate;
class Filter;
class Selector;

class Query{
  std::vector<Relation*> relations;
  std::vector<Predicate> predicates;
  std::vector<Filter> filters;
  std::vector<Selector> selectors;
  public:
    // parse query in [FROM]|[WHERE]|[SELECT]
    bool parseQuery(const std::string line);
    //get Relation
    Relation* getRelation(const int i) const;
    // get Predicate
    const Predicate* getPredicate(const int i) const;
    // get Filter
    const Filter* getFilter(const int i) const;
    // get Selector
    const Selector* getSelector(const int i) const;
    // clear query
    void clear();
};

class Predicate{
  public:
    Relation* relation1;
    uint64_t relId1; // relative realtion id
    uint64_t col1;
    Relation* relation2;
    uint64_t relId2; // relative realtion id
    uint64_t col2;
    char op;
    Predicate(std::string predicate);
};

class Filter{
  public:
    Relation* relation;
    uint64_t relId; // relative realtion id
    uint64_t col;
    int64_t value;
    char op;
    Filter(std::string filter);
};

class Selector{
  public:
    Relation* relation;
    uint64_t relId; // relative realtion id
    uint64_t col;
    Selector(std::string selector);
};
