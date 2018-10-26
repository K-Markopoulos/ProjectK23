#include <iostream>
#include <stdint.h>

/********** HEADER   **********/

typedef struct hash_table hash_table;
typedef struct array_int array_int;

struct hash_table{
  array_int psum;
  relation * rel;
};

struct array_int{
  int32_t * data;
  int32_t length;
};

/********** /HEADER   **********/


/********** SOURCE   **********/

array_int createHistogram(relation * rel){
  array_int hist;
  hist.length = (1 << n); // 2^n
  hist.data = calloc(sizeof(int32_t), hist.length);

  for(int32_t i = 0; i < rel.num_tuples; i++){
    hist.data[h1(rel.tuples[i].payload)]++;
  }

  return hist;
}

array_int createPsum(array_int hist){
  array_int psum;
  psum.length = hist.length;
  psum.data = calloc(sizeof(int32_t), psum.length);

  psum.data[0] = 0;
  for(int32_t i = 1; i < psum_size; i++){
    psum.data[i] = psum.data[i-1] + hist.data[i-1];
  }

  return psum;
}

relation * createRelation(relation rel, array_int psum_original){
  //make a psum copy
  array_int psum;
  psum.length = psum_original.length;
  psum.data = malloc(psum.length * sizeof(int32_t));
  memcpy(psum.data, psum_original.data, psum.length * sizeof(int32_t));

  //init new_rel
  relation * new_rel = malloc(sizeof(realtion));
  new_rel.num_tuples = rel.num_tuples;
  new_rel.tuples = malloc(rel.num_tuples * sizeof(tuple));

  //insert tuples to new_rel
  for(int32_t i = 0; i < rel.num_tuples; i++){
    new_rel.tuples[psum.data[h1(rel.tuples[i].payload]] = rel.tuples[i];
    psum.data[h1(rel.tuples[i].payload]++;
  }

  //free(psum.data);
  return new_rel;
}

hash_table * reorderRelation(relation * rel){
  hash_table * result = malloc(sizeof(hash_table));

  array_int hist = createHistogram(rel);
  result.psum = createPsum(hist);
  result.rel = createRelation(rel, result.psum);

  return result;
}


result * RadixHashJoin(relation * rel_R, relation * rel_S){
  relation ro_rel_R, ro_rel_S;

  hash_table * hash_table_R = reorderRelation(rel_R);
  hash_table * hash_table_S = reorderRelation(rel_S);

  //init result

  //for( /* each bucket */ ){
    // choose min bucket to hash it
    // make bucket-chain hash for min
    // result "+=" compareBuckets(raw_bucket, hashed_bucket, ... bucket-chain if needed )
    // go to next bucket ??
  }

  // return result
}
