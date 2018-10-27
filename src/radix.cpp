#include <iostream>
#include <stdint.h>
#include <string.h>
#include "../inc/header.h"

// 1st hash function
#define H1_LAST_BITS 8
#define h1(X) (X & ((1 << H1_LAST_BITS) - 1))


/********** SOURCE   **********/

/* pretty print a relation for debugging */
void printRelation(relation * rel, char const * name){
  std::cout << "* Printing Relation " << name << std::endl;
  std::cout << "* key  | payload" << std::endl;
  std::cout << "* -----+--------" << std::endl;
  for(int i = 0; i < rel->num_tuples; i++)
    printf("* %5d|%5d\n", rel->tuples[i].key, rel->tuples[i].payload);
  std::cout << "* /Printing Relation " << name << std::endl << std::endl;
}


array_int createHistogram(relation * rel){
  array_int hist;
  hist.length = (1 << H1_LAST_BITS);
  hist.data = (int32_t*) calloc(sizeof(int32_t), hist.length);

  for(int32_t i = 0; i < rel->num_tuples; i++){
    hist.data[h1(rel->tuples[i].payload)]++;
  }

  return hist;
}


array_int createPsum(array_int hist){
  array_int psum;
  psum.length = hist.length;
  psum.data = (int32_t*) calloc(sizeof(int32_t), psum.length);

  psum.data[0] = 0;
  for(int32_t i = 1; i < psum.length; i++)
    psum.data[i] = psum.data[i-1] + hist.data[i-1];

  return psum;
}


relation * createRelation(relation * rel, array_int psum_original){
  //make a psum copy
  array_int psum;
  psum.length = psum_original.length;
  psum.data = (int32_t*) malloc(psum.length * sizeof(int32_t));
  memcpy(psum.data, psum_original.data, psum.length * sizeof(int32_t));

  //init new_rel
  relation * new_rel = (relation*) malloc(sizeof(relation));
  new_rel->num_tuples = rel->num_tuples;
  new_rel->tuples = (tuple*) malloc(rel->num_tuples * sizeof(tuple));

  //insert tuples to new_rel
  for(int32_t i = 0; i < rel->num_tuples; i++){
    new_rel->tuples[psum.data[h1(rel->tuples[i].payload)]] = rel->tuples[i];
    psum.data[h1(rel->tuples[i].payload)]++;
  }

  //free(psum.data);
  return new_rel;
}


hash_table * reorderRelation(relation * rel){
  hash_table * result = (hash_table*) malloc(sizeof(hash_table));

  array_int hist = createHistogram(rel);
  result->psum = createPsum(hist);
  result->rel = createRelation(rel, result->psum);

  return result;
}


result * RadixHashJoin(relation * rel_R, relation * rel_S){

  printRelation(rel_R, "R");
  printRelation(rel_S, "S");

  hash_table * hash_table_R = reorderRelation(rel_R);
  hash_table * hash_table_S = reorderRelation(rel_S);
  std::cout << "reordering DONE\n";

  printRelation(hash_table_R->rel, "R\'");
  printRelation(hash_table_S->rel, "S\'");
  //init result

  //for( /* each bucket */ ){
    // choose min bucket to hash it
    // make bucket-chain hash for min
    // result "+=" compareBuckets(raw_bucket, hashed_bucket, ... bucket-chain if needed )
    // go to next bucket ??
  //}

  // return result
}
