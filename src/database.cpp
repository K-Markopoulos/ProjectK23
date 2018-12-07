#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include "../inc/database.hpp"
#include "../inc/relation.hpp"
#include "../inc/query.hpp"
#include "../inc/utils.hpp"

using namespace std;

/** -----------------------------------------------------
 * Adds a new relation in DB, specifying path
 *
 * @params fileName, fileName where relation is stored
 */
void Database::addRelation(const string fileName){
  if(file_exists(fileName))
    return;
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
  if(id < this->getRelationsCount())
    return &this->relations[id];
  LOG("ERROR: Database::getRelation out of border\n");
  return NULL;
}

/** -----------------------------------------------------
 * Gets the relations count in DB
 *
 * @returns count, integer
 */
size_t Database::getRelationsCount(){
  return this->relations.size();
}

/** -----------------------------------------------------
 * Runs the query using RadixHashJoin and returns the result
 *
 * @params query
 * @returns char*, result from query
 */
char* Database::run(Query query){
  //  initialize Intermmediate results
  // Intermmediate results;
  // //  run filters
  // for(Filter filter : query.filters){
  //   runFilter(filter, &results);
  // }
  //
  // //  run predicates
  // for(Predicate pred : query.predicates){
  //   runPredicate(filter, &results);
  // }


  //NOT IMPLEMENTED
  //shhhhhhhhhhhh
  cout << "Running ...";
  sleep(3000);
  cout << "\nDone\n";
  return NULL;
}
