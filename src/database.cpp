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
