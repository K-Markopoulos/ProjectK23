#pragma once
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

// /#define PRINT_IR
// #define SKIP_DATA
// #define DEBUG
#define LOGGER "DEBUG"
#ifdef DEBUG
#define LOG(...) fprintf(stdout,"[%s]: ",LOGGER); fprintf( stdout, __VA_ARGS__ );
#else
#define LOG(...) do{ } while ( 0 )
#endif

using namespace std;

typedef struct elapsed_timer{
  double loading;
  double running;
  double radix;
  double filters;
  double predicates;
  double selectors;
  double intermediate_build;
  double intermediate_update;
  double optimizer;
}elapsed_timer;

extern elapsed_timer elapsed;

// split a string based on the delimeter
vector<string> split(const string line, const char delim);
// checks if file exists
bool file_exists(const string& name);
