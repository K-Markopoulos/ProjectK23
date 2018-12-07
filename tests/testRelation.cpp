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
    db->addRelation("./workloads/small/"+line);
  }
  LOG("Loading OK!")

  LOG("Retreiving now..")
  Relation* rel;
  for(int i = 0; i < db->getRelationsCount(); i++){
    rel = db->getRelation(i);
    for(int c = 0; c < rel->getColumnCount(); c++){
      for(int t = 0; t < rel->getTupleCount(); t++){
        cout << rel->getTuple(c, t) << endl;
      }
      cout << endl;
    }
  }
  cout << endl;

   return 0;
}
