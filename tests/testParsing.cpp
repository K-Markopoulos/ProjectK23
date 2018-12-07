#include <iostream>
#include "../inc/query.hpp"
#include "../inc/utils.hpp"

using namespace std;

int main(int argc, char* argv[]) {
  string line;
  Query query;
  int tests = 0, passed = 0;
  while (getline(cin, line)) {
    if(line == "F") continue;
    tests++;
    if(query.parseQuery(line)){
      passed++;
      cout << "OK!\n";
    }
    else
      cout << "INVALID!\n";
    cout << endl;
  }
  cout << "\n\n\n************\ntests:" << tests << "\npassed:" << passed << endl;
  if(tests == passed)
    return 0;
  return 1;
}
