#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include "../inc/database.hpp"
#include "../inc/query.hpp"

using namespace std;

/** -----------------------------------------------------
 * Adds a new relation in DB, specifying path
 *
 * @params fileName, fileName where relation is stored
 */
void Database::addRelation(const string fileName){
  this->relations.emplace_back(Relation(fileName));
  return;
}

/** -----------------------------------------------------
 * Gets a relation from DB
 *
 * @params id, the histogram
 * @returns relation
 */
Relation* Database::getRelation(int id){
  return &this->relations[id];
}

/** -----------------------------------------------------
 * Runs the query using RadixHashJoin and returns the result
 *
 * @params query
 * @returns char*, result from query
 */
char* Database::run(Query query){
  //NOT IMPLEMENTED
  //shhhhhhhhhhhh
  cout << "Running ...";
  sleep(3000);
  cout << "\nDone\n";
  return NULL;
}

/** -----------------------------------------------------
 * Runs the filter using RadixHashJoin and returns the result
 *
 * @params filter, results
 * @returns
 */
void Database::runFilter(Filter filter, Intermediate* results){
    /*size_t pos_operator, pos_dot, rel, col, comp;
    char operator = filter.at(pos_operator = filter.find_first_of("<>="));
    string part1 = filter.substr(0, pos_operator);
    string part2= filter.substr(pos_operator+1);

    pos_dot = part1.find(".");
    rel = atoi(part1.substr(0,pos_dot));
    col = atoi(part1.substr(pos_dot+1));
    comp = stoi(part2);
    for(int i=0; i<this.relations[rel].size(); i++){
      switch(operator){
      case '>':
        if(this.relations[rel][i] > comp)
          results.filter_rowIDs.push_back(this.relations[rel][i]);
      case '<':
        if(this.relations[rel][i] < comp)
          results.filter_rowIDs.push_back(this.relations[rel][i]);
      case '=':
        if(this.relations[rel][i] == comp)
          results.filter_rowIDs.push_back(this.relations[rel][i]);
      }*/
      for(int i = 0; i <= filter->relation.num_tuples; i++){
        switch(filter.op){
        case '>':
          if(filter->relation[filter.col][i] > filter.value)
            results.filter_rowIDs.push_back(filter->relation[filter.col][i]);
        case '<':
          if(filter->relations[col][i] < filter.value)
            results.filter_rowIDs.push_back(filter->relation[filter.col][i]);
        case '=':
          if(filter->relations[col][i] == filter.value)
            results.filter_rowIDs.push_back(filter->relation[filter.col][i]);
      }
}
