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
    Relation* getRelation(const int i);
    // get Predicate
    Predicate* getPredicate(const int i);
    // get Filter
    Filter* getFilter(const int i);
    // get Selector
    Selector* getSelector(const int i);
    // clear query
    void clear();
};

class Predicate{
  Relation* relation1;
  uint64_t col1;
  Relation* relation2;
  uint64_t col2;
  char op;
  public:
    Predicate(std::string predicate);
};

class Filter{
  Relation* relation;
  uint64_t col;
  int64_t value;
  char op;
  public:
    Filter(std::string filter);
};

class Selector{
  Relation* relation;
  uint64_t col;
  public:
    Selector(std::string selector);
};
