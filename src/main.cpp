#include <iostream>
#include "../inc/database.hpp"
#include "../inc/query.hpp"
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
  LOG("\nREADING QUERIES\n\n\n");
  Query query;
  while (getline(cin, line)) {
    LOG("got query %s\n", line.c_str());
    if (line == "F" || line.empty()) {
      cout << endl;
      continue;
    }
    if(!query.parseQuery(line)){
      cout << "Invalid query, skipping...\n";
      continue;
    }
    cout << db->run(query);
    query.clear();
  }
  cout << endl;
  return 0;
}
