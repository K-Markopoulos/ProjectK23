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

// #define PRINT_IR
// #define SKIP_DATA
#define DEBUG
#ifdef DEBUG
#define LOGGER "DEBUG"
#define LOG(...) fprintf(stdout,"[%s]: ", LOGGER); fprintf( stdout, __VA_ARGS__ );
#else
#define LOG(...) do{ } while ( 0 )
#endif
using namespace std;

// split a string based on the delimeter
vector<string> split(const string line, const char delim);
// checks if file exists
bool file_exists(const string& name);
