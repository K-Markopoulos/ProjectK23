#include <iostream>
#include <stdint.h>

typedef struct tuple tuple;
typedef struct relation relation;
typedef struct hash_table hash_table;
typedef struct array_int array_int;
typedef struct bucket bucket;
typedef struct result result;
typedef struct b_chain b_chain;
typedef struct bucket_hash bucket_hash;

struct tuple {
  int32_t key;
  int32_t payload;
};

struct relation {
  tuple * tuples;
  uint32_t num_tuples;
};

struct array_int{
  int32_t * data;
  int32_t length;
};

struct hash_table{
  array_int psum;
  relation * rel;
};

struct bucket{
  int32_t low;
  int32_t high;
};

struct b_chain {
  int32_t *Bucket;
  int32_t *Chain;
};

struct bucket_hash {
  bucket *b;
  hash_table *ht;
};




result * RadixHashJoin(relation * rel_R, relation * rel_S);
void compareBuckets(bucket_hash *sm,bucket_hash *lg,b_chain *bc,result *res_list,bool isReversed);
b_chain * indexingSmallBucket(bucket_hash *small);
bool isPrime(uint32_t n);
inline int findNextPrime(uint32_t n);
inline int h2(int32_t num);
hash_table * reorderRelation(relation * rel);
relation * createRelation(relation * rel, array_int psum_original);
array_int createPsum(array_int hist);
array_int createHistogram(relation * rel);
void freeHashTableAndComponents(hash_table *);
