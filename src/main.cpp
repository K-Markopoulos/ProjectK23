#include <iostream>
#include "../inc/database.hpp"
#include "../inc/query.hpp"
#include "../inc/utils.hpp"

Database* db;

using namespace std;

int main(int argc, char* argv[]) {
  db = new Database();
  string line;

  //phase 1: reading relations' paths
  while (getline(cin, line)) {
    if (line == "Done" || line.empty()) break;
    db->addRelation("./workloads/small/"+line);
  }

  //phase 2: reading query batches
  LOG("\n\n\nREADING QUERIES\n\n\n");
  Query query;
  while (getline(cin, line)) {
    if (line == "F" || line.empty()) {
      continue;
    }
    if(!query.parseQuery(line)){
      cout << "Invalid query, skipping...\n";
      continue;
    }
    cout << db->run(query).c_str() << endl;
    query.clear();
  }
  cout << endl;
  return 0;
}
