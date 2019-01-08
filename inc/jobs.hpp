#pragma once
#include <iostream>

class Job;

typedef struct array_int array_int;
typedef struct relation relation;
typedef struct bucket bucket;
typedef struct result result;
typedef struct b_chain b_chain;
typedef struct bucket_hash bucket_hash;


class HistogramJob: public Job {
  int low;
  int high;
  array_int* hist;
  relation* rel;
  public:
    HistogramJob(int low_, int high_, array_int* hist_, relation* rel);
    int Run();
};

class PartitionJob: public Job {
  int low;
  int high;
  relation* rel;
  relation* new_rel;
  array_int* psum;
  public:
    PartitionJob(int low_, int high_, relation* rel_, relation* new_rel_, array_int* psum);
    int Run();
};

class JoinJob: public Job {
  bucket_hash* small;
  bucket_hash* large;
  b_chain* bc;
  result* res;
  bool isReversed;
  public:
    JoinJob(bucket_hash *small,bucket_hash *large,b_chain *bc,result *res,bool isReversed);
    ~JoinJob();
    int Run();
};
