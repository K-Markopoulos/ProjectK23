#include <iostream>
#include "../inc/utils.h"

using namespace std;

int main(int argc, char* argv[]) {
   string line;
   vector<string> sections;
   while (getline(cin, line)) {
      sections = split(line, '|');
      for(string str : sections)
        cout << "-> " << str << endl;
      cout << endl;
   }

   return 0;
}
