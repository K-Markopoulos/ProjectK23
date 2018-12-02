#include <iostream>
#include "inc/databse.h"
#include "inc/query.h"

/** -----------------------------------------------------
 * Adds the path of a relation in DB
 *
 * @params fileName, fileName where relation is stored (binary)
 */
void Database::addRelationSource(const char * fileName){
  //NOT IMPLEMENTED
  return;
}
/** -----------------------------------------------------
 * Gets the path of a relation from DB
 *
 * @params id, the histogram
 * @returns char*, relation's path
 */
char* Database::getRelationSource(uint64_t id){
  //NOT IMPLEMENTED
  return NULL;
}
/** -----------------------------------------------------
 * Runs the query using RadixHashJoin and returns the result
 *
 * @params query
 * @returns char*, result from query
 */
char* Database::run(Query query){
  //NOT IMPLEMENTED
  return NULL;
}
