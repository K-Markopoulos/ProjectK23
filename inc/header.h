#include <iostream>
#include <stdint.h>

typedef struct tuple tuple;
typedef struct relation relation;
typedef struct hash_table hash_table;
typedef struct array_int array_int;
typedef struct bucket bucket;
typedef struct result result;

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



result * RadixHashJoin(relation * rel_R, relation * rel_S);
result * indexingAndCompareBuckets(hash_table *small,hash_table *large);
inline int h2(int32_t num);
hash_table * reorderRelation(relation * rel);
relation * createRelation(relation * rel, array_int psum_original);
array_int createPsum(array_int hist);
array_int createHistogram(relation * rel);



/**** unused functions ****/
int considered(int32_t, tuple *);
int size(tuple *);
tuple * histogram(tuple *);
tuple * psum(tuple *);
int32_t getposition(int32_t, tuple*);
tuple * reorder(tuple *, tuple *, tuple *);
