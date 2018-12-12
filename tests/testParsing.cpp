#include <iostream>
#include "../inc/query.hpp"
#include "../inc/utils.hpp"
#include "../inc/database.hpp"

Database* db;

using namespace std;

int main(int argc, char* argv[]) {
  db = new Database();
  string line;
  while (getline(cin, line)) {
    if (line == "Done" || line.empty()) break;
    db->addRelation("./workloads/small/"+line);
  }
  Query query;
  int tests = 0, passed = 0;
  while (getline(cin, line)) {
    if(line == "F" || line.empty()) continue;
    tests++;
    if(query.parseQuery(line)){
      passed++;
      cout << "OK!\n";
    }
    else
      cout << "INVALID!\n";
    cout << endl;
    query.clear();
  }
  cout << "\n\n\n************\ntests:" << tests << "\npassed:" << passed << endl;
  if(tests == passed)
    return 0;
  return 1;
}
