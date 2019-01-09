#include <iostream>
#include "../inc/database.hpp"
#include "../inc/utils.hpp"
#include "../inc/relation.hpp"

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
    vector<Stats*>* st = rel->getStats();
    cout<<"Relation "<<i<<endl;
    for(Stats* s : *st){
      cout<<"l = "<<s->getl()<<endl;
      cout<<"u = "<<s->getu()<<endl;
      cout<<"f = "<<s->getf()<<endl;
      cout<<"d = "<<s->getd()<<endl<<endl;
    }
    cout << endl;
  }

  return 0;
}
