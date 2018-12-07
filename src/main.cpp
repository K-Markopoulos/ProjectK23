#include <iostream>
#include "../inc/database.hpp"
#include "../inc/query.hpp"

Database* db;

using namespace std;

int main(int argc, char* argv[]) {
  db = new Database();
  string line;
  while (getline(cin, line)) {
    if (line == "Done") break;
    db->addRelation(line.c_str());
  }

  Query query;
  while (getline(cin, line)) {
    if (line == "F") {
      cout << endl;
      continue;
    }
    query.parseQuery(line);
    cout << db->run(query);
    query.clear();
  }
  return 0;
}
