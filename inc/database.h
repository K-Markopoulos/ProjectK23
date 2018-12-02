#include <iostream>
#include "inc/query.h"

class Database{
  std::vector<char*> relationSource;

  //add relation filename
  void addRelationSource(const char * fileName);
  //get relation filename
  char* getRelationSource(uint64_t id);
  //run a query
  uint64_t run(Query query);

}
