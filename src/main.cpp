#include <iostream>
#include "inc/databse.h"
#include "inc/query.h"

using namespace std;

int main(int argc, char* argv[]) {
   Database db;
   string line;
   while (getline(cin, line)) {
      if (line == "Done") break;
      db.addRelationSource(line.c_str());
   }

   Query query;
   while (getline(cin, line)) {
      if (line == "F") {
        cout << endl;
        continue;
      }
      query.parseQuery(line);
      cout << db.run(query);
   }
   return 0;
}
