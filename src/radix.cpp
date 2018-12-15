#include <iostream>
#include <stdint.h>
#include <string.h>
#include "../inc/radix.h"
#include "../inc/result.h"

// 1st hash function
#define H1_LAST_BITS 8
//#define PRIME_NUM 101
#define h1(X) (X & ((1 << H1_LAST_BITS) - 1))

uint64_t PRIME_NUM;

/**
 * pretty print a relation for debugging
 *
 * @params res, the result struct
 * @params name, a label for the relation
 */
void printRelation(relation * rel, char const * name){
  std::cout << "* Printing Relation " << name << std::endl;
  std::cout << "* key  | payload" << std::endl;
  std::cout << "* -----+--------" << std::endl;
  for(uint64_t i = 0; i < rel->num_tuples; i++)
    printf("* %5ld|%5ld\n", rel->tuples[i].key, rel->tuples[i].payload);
  std::cout << "* /Printing Relation " << name << std::endl << std::endl;
}

/**
 * H2 hash function
 *
 * @params num, hash this number
 * @returns hash value
 */
inline uint64_t h2(int64_t num) {
  return num % PRIME_NUM; //Later we are going to find the next prime from length
}

/**
 * Creates a Histogram from a relation
 *
 * @params rel, the relation
 * @returns Histogram
 */
array_int createHistogram(relation * rel){
  array_int hist;
  hist.length = (1 << H1_LAST_BITS);
  hist.data = (int64_t*) calloc(sizeof(int64_t), hist.length);

  for(uint64_t i = 0; i < rel->num_tuples; i++){
    hist.data[h1(rel->tuples[i].payload)]++;
  }
  return hist;
}

/**
 * Creates a Psum array from a Histogram
 *
 * @params hist, the histogram
 * @returns Psum
 */
array_int createPsum(array_int hist){
  array_int psum;
  int previous;
  bool count=true;
  int64_t index_mod=1;

  psum.length = hist.length;
  psum.data = (int64_t*) calloc(sizeof(int64_t),psum.length);
  psum.data[0]=0;
  for(int64_t i = 0; i < psum.length; i++){
   if(hist.data[i]!=0) {
    if(count) {
      psum.data[i]=0;
      count=false;
    }
    else {
      psum.data[i]=psum.data[previous] +hist.data[previous];
    }
    previous=i;
  }
  else {
    psum.data[i]=-1;
  }
}

  return psum;
}

/**
 * Creates a reordered relation using the original relation and the Psum array
 *
 * @params res, the result struct
 * @params psum_original, the Psum array
 * @returns relation, the reordered relation
 */
relation * createRelation(relation * rel, array_int psum_original){
  //make a psum copy
  array_int psum;
  psum.length = psum_original.length;
  psum.data = (int64_t*) malloc(psum.length * sizeof(int64_t));
  memcpy(psum.data, psum_original.data, psum.length * sizeof(int64_t));

  //init new_rel
  relation * new_rel = (relation*) malloc(sizeof(relation));
  new_rel->num_tuples = rel->num_tuples;
  new_rel->tuples = (tuple_*) malloc(rel->num_tuples * sizeof(tuple_));

  //insert tuples to new_rel
  for(int64_t i = 0; i < rel->num_tuples; i++){
    new_rel->tuples[psum.data[h1(rel->tuples[i].payload)]] = rel->tuples[i];
    psum.data[h1(rel->tuples[i].payload)]++;
  }

  free(psum.data);
  return new_rel;
}

/**
 * Creates a reordered relation using the original relation and the Psum array
 *
 * @params res, the result struct
 * @params psum_original, the Psum array
 * @returns hash_table
 */
hash_table * reorderRelation(relation * rel){
  hash_table * result = (hash_table*) malloc(sizeof(hash_table));

  array_int hist = createHistogram(rel);
  result->psum = createPsum(hist);
  result->rel = createRelation(rel, result->psum);

  free(hist.data);
  return result;
}

/**
 * Finds at which element a bucket ends
 *
 * @params ht, the hash table
 * @params index_start, the bucket we are currently
 */
int64_t set_high(hash_table* ht, uint64_t index_start){
  if(index_start == ht->psum.length)
    return ht->rel->num_tuples;
  for(uint64_t i = index_start; i<ht->psum.length; i++){
    if(ht->psum.data[i]!=-1)
      return ht->psum.data[i];
  }
  return ht->rel->num_tuples;
}

/**
 * Indexing in the smallest relation and comparing bucket by bucket
 *
 * @params small, hash table which has the 'small' relation
 * @params large, hash table which has the 'large' relation
 * @params isReversed, for printing the results in the order the 2 relations were given
 */


void compareBuckets(bucket_hash *small,bucket_hash *large,b_chain *bc,result *res_list,bool isReversed) {
  int64_t lg_value,h2_res,index;
  tuple_ res_tuple;

  int64_t sm_low=small->b->low;
  for(int64_t k=large->b->low; k<large->b->high; k++) {
    lg_value=large->ht->rel->tuples[k].payload;
    h2_res=h2(lg_value);
    index=bc->Bucket[h2_res];
    while(index!=-1) {
      if(small->ht->rel->tuples[index+sm_low].payload==lg_value) {
        if(isReversed) {
          res_tuple.key=large->ht->rel->tuples[k].key;
          res_tuple.payload=small->ht->rel->tuples[index+sm_low].key;
        }
        else {
          res_tuple.key=small->ht->rel->tuples[index+sm_low].key;
          res_tuple.payload=large->ht->rel->tuples[k].key;
        }
        addToResult(res_list,&res_tuple);
      }
      index=bc->Chain[index];
    }
  }
}

/**
 * Finds if a number is prime
 *
 * @params n,the 'input' number
 * @returns true or false
 */
bool isPrime(uint64_t n) {
    for(uint64_t i=2; i*i<=n; i++) {
      if(n % i == 0) {
        return false;
      }
    }
    return true;
}

/**
 * Iterates through numbers bigger than n until it finds a prime
 *
 * @params res, the result struct
 * @returns next prime number
 */
inline uint64_t findNextPrime(uint64_t n) {
  while(!isPrime(++n));
  return n;
}

b_chain * indexingSmallBucket(bucket_hash *small) {
  b_chain * bc=new b_chain();
  uint64_t bucket_size=small->b->high-small->b->low;

  bc->Chain=new int64_t[bucket_size];
  PRIME_NUM=findNextPrime(bucket_size);
  bc->Bucket=new int64_t[PRIME_NUM];

  for(int64_t j=0; j<PRIME_NUM; j++){
    bc->Bucket[j]=-1;
  }

  for(int64_t l=small->b->low; l<small->b->high; l++){
    bc->Chain[l-small->b->low]=bc->Bucket[h2(small->ht->rel->tuples[l].payload)];
    bc->Bucket[h2(small->ht->rel->tuples[l].payload)]=l-small->b->low;
  }

  return bc;
}


/**
 * Join two relations
 *
 * @params rel_R, 1st relation as R
 * @params rel_S, 1st relation as S
 * @returns result list
 */
result * radixHashJoin(relation * rel_R, relation * rel_S){
  //printRelation(rel_R, "R");
  //printRelation(rel_S, "S");

  hash_table * hash_table_R = reorderRelation(rel_R);
  hash_table * hash_table_S = reorderRelation(rel_S);
  //DEBUG//std::cout << "reordering DONE\n";

  //printRelation(hash_table_R->rel, "R\'");
  //printRelation(hash_table_S->rel, "S\'");

  //Here should be the initialization of the 'list'//
  result *res_list;
  initResult(&res_list);

  for(int64_t i=0; i<hash_table_R->psum.length; i++) {
    bucket R_bucket,S_bucket;
    bucket_hash small,large;
    bool isReversed;

    if(hash_table_R->psum.data[i]!=-1 && hash_table_S->psum.data[i] !=-1) {
      R_bucket.low=hash_table_R->psum.data[i];
      S_bucket.low=hash_table_S->psum.data[i];

      R_bucket.high=set_high(hash_table_R,i+1);
      S_bucket.high=set_high(hash_table_S,i+1);

      if(R_bucket.high-R_bucket.low <= S_bucket.high-S_bucket.low) {
        small.b=&R_bucket;
        small.ht=hash_table_R;

        large.b=&S_bucket;
        large.ht=hash_table_S;
        isReversed=false;
      }
      else {
        small.b=&S_bucket;
        small.ht=hash_table_S;

        large.b=&R_bucket;
        large.ht=hash_table_R;
        isReversed=true;
      }
      b_chain *bc=indexingSmallBucket(&small);
      compareBuckets(&small,&large,bc,res_list,isReversed);

      delete[] bc->Chain;
      delete[] bc->Bucket;
      delete bc;
    }
  }

  freeHashTableAndComponents(hash_table_R);
  freeHashTableAndComponents(hash_table_S);
  return res_list;
}

/**
 *  Free the hash table
 *
 * @params ht, the hash table
 */
void freeHashTableAndComponents(hash_table * ht){
  free(ht->psum.data);
  free(ht->rel->tuples);
  free(ht->rel);
  free(ht);
}

/**
 *  Free the struct relation
 *
 * @params ht, the hash table
 */
void destroyRelation(relation* rel){
  free(rel->tuples);
  free(rel);
}
