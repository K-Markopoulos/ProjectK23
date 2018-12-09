#include <iostream>
#include "../inc/database.hpp"
#include "../inc/utils.hpp"

Database* db;

using namespace std;

int main(int argc, char* argv[]) {
  db = new Database();
  string line;
  while (getline(cin, line)) {
    if (line == "Done" || line.empty()) break;
    LOG("Adding relation %s\n", line.c_str());
    db->addRelation("./workloads/small/"+line);
  }
  LOG("Loading OK!\n");

  LOG("Retreiving now..\n");
  Relation* rel;
  for(int i = 0; i < db->getRelationsCount(); i++){
    rel = db->getRelation(i);
      for(int t = 0; t < rel->getTupleCount(); t++){
        for(int c = 0; c < rel->getColumnCount(); c++){
          LOG("\nGet tuple [%d][%d] %ld\n", c, t, rel->getTuple(c, t));
          cout << rel->getTuple(c, t) << '|';
      }
      cout << endl;
    }
  }

  return 0;
}
