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
  std::vector<Predicate*> predicates;
  std::vector<Filter*> filters;
  std::vector<Selector*> selectors;
  // if predicate match filter format
  bool isFilter(std::string pred);
  // validates that sections are as expected
  bool validateSections(const std::vector<std::string>& sections);
  // validates that relation is as expected
  bool validateRelation(const std::string& relation);
  // validates that predicate is as expected
  bool validatePredicate(const std::string& predicate);
  // validates that selector is as expected
  bool validateSelector(const std::string& selector);
  public:
    // parse query in [FROM]|[WHERE]|[SELECT]
    bool parseQuery(const std::string line);
    //get Relation
    Relation* getRelation(const int i) const;
    // get Relations Count
    uint64_t getRelationsCount() const;
    // get Predicate
    const Predicate* getPredicate(const int i) const;
    // get Filter
    const Filter* getFilter(const int i) const;
    // get Selector
    const Selector* getSelector(const int i) const;
    // get number of predicates
    uint64_t getPredicateCount() const;
    // get number of filters
    uint64_t getFilterCount() const;
    // Join Enumeration function
    void joinEnumeration();
    // Cost functions
    uint64_t cost();
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
    Predicate(std::string predicate, Query* query);
};

class Filter{
  public:
    Relation* relation;
    uint64_t relId; // relative realtion id
    uint64_t col;
    int64_t value;
    char op;
    Filter(std::string filter, Query* query);
};

class Selector{
  public:
    Relation* relation;
    uint64_t relId; // relative realtion id
    uint64_t col;
    Selector(std::string selector, Query* query);
};
