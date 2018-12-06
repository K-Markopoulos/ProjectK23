#include <iostream>
#include <string>
#include <vector>
#include <cstddef>

#define MUST(call)            \
  {                           \
    if (!(call)) {              \
      return false;           \
    }                         \
  }

#define DEBUG
#ifdef DEBUG
#define LOG(...) fprintf(stdout,"[DEBUB]: "); fprintf( stdout, __VA_ARGS__ );
#else
#define DEBUG_PRINT(...) do{ } while ( 0 )
#endif
using namespace std;

// split a string based on the delimeter
vector<string> split(const string line, const char delim);
// validates that sections are as expected
bool validateSections(const vector<string>& sections);
// validates that relation is as expected
bool validateRelation(const string& relation);
// validates that predicate is as expected
bool validatePredicate(const string& predicate);
// validates that selector is as expected
bool validateSelector(const string& selector);
