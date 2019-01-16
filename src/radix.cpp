#include <iostream>
#include <stdint.h>
#include <string.h>
#include "../inc/radix.h"
#include "../inc/result.h"
#include "../inc/jobScheduler.hpp"
#include "../inc/jobs.hpp"
#include "../inc/utils.hpp"


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
 * Creates a Histogram from a relation
 *
 * @params rel, the relation
 * @returns Histogram
 */
array_int* createHistogram(relation * rel){
  array_int * hist;
  int low = 0, high = 0, chunk = rel->num_tuples / NUM_THREADS;
  hist = (array_int*) malloc(sizeof(array_int) * NUM_THREADS);

  for (int i = 0; i < NUM_THREADS; i++) {
    low = high;
    high = (i == NUM_THREADS-1)? rel->num_tuples: (i+1) * chunk;
    hist[i].length = (1 << H1_LAST_BITS);
    hist[i].data = (int64_t*) calloc(sizeof(int64_t), hist[i].length);
    jobScheduler->Schedule(new HistogramJob(low, high, &hist[i], rel));
  }

  jobScheduler->Barrier();
  return hist;
}

/**
 * Creates a Psum array from a Histogram
 *
 * @params hist, the histogram
 * @returns Psum
 */
array_int* createPsum(array_int* hist){
  array_int* psum, hist_total;
  psum = (array_int*) malloc(sizeof(array_int) * NUM_THREADS);
  hist_total.data = (int64_t*) calloc(hist[0].length, sizeof(int64_t));

  for(uint64_t i=0; i < hist[0].length; i++)
    for (int t = 0; t < NUM_THREADS; t++)
      hist_total.data[i] += hist[t].data[i];

  for (int t = 0; t < NUM_THREADS; t++) {
    psum[t].length = hist[t].length;
    psum[t].data = (int64_t*) malloc(sizeof(int64_t)*psum[t].length);
    if(!t){
      psum[t].data[0] = 0;
      for(uint64_t i=1; i < psum[t].length; i++)
        psum[t].data[i] = psum[t].data[i-1] + hist_total.data[i-1];
    } else {
      for(uint64_t i=0; i < psum[t].length; i++)
        psum[t].data[i] = psum[t-1].data[i] + hist[t-1].data[i];
    }
  }

  free(hist_total.data);
  return psum;
}

/**
 * Creates a reordered relation using the original relation and the Psum array
 *
 * @params res, the result struct
 * @params psum_original, the Psum array
 * @returns relation, the reordered relation
 */
relation * createRelation(relation * rel, array_int* psum){
  //init new_rel
  int low = 0, high = 0, chunk = rel->num_tuples / NUM_THREADS;
  relation * new_rel = (relation*) malloc(sizeof(relation));
  new_rel->num_tuples = rel->num_tuples;
  new_rel->tuples = (tuple_*) malloc(rel->num_tuples * sizeof(tuple_));

  for (int t = 0; t < NUM_THREADS; t++) {
    low = high;
    high = (t == NUM_THREADS-1)? rel->num_tuples: (t+1) * chunk;
    jobScheduler->Schedule(new PartitionJob(low, high, rel, new_rel, &psum[t]));
  }

  jobScheduler->Barrier();
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

  array_int* hist = createHistogram(rel);
  array_int* psum = createPsum(hist);

  result->psum.length = psum[0].length;
  result->psum.data = (int64_t*) malloc(psum[0].length * sizeof(int64_t));
  memcpy(result->psum.data, psum[0].data, psum[0].length * sizeof(int64_t));

  result->rel = createRelation(rel, psum);

  for (int t = 0; t < NUM_THREADS; t++){
    free(hist[t].data);
    free(psum[t].data);
  }
  free(psum);
  free(hist);
  return result;
}

/**
 * Indexing in the smallest relation and comparing bucket by bucket
 *
 * @params small, hash table which has the 'small' relation
 * @params large, hash table which has the 'large' relation
 * @params isReversed, for printing the results in the order the 2 relations were given
 */
void compareBuckets(bucket_hash *small,bucket_hash *large,b_chain *bc,result *res_list,bool isReversed) {
  int64_t lg_value, index;
  tuple_ res_tuple;
  tuple_* s_tuples = small->ht->rel->tuples;
  tuple_* l_tuples = large->ht->rel->tuples;
  int64_t sm_low=small->b->low;

  for(int64_t k=large->b->low; k<large->b->high; k++) {
    lg_value = l_tuples[k].payload;
    index = bc->Bucket[h2(lg_value, bc->b_size)];
    int64_t last_index = 0; //DEBUG
    while (index != -1) {
      if(s_tuples[index+sm_low].payload == lg_value) {
        if(isReversed) {
          res_tuple.key = l_tuples[k].key;
          res_tuple.payload = s_tuples[index+sm_low].key;
        }
        else {
          res_tuple.key = s_tuples[index+sm_low].key;
          res_tuple.payload = l_tuples[k].key;
        }
        addToResult(res_list,&res_tuple);
      }
      last_index = index;
      index = bc->Chain[index];
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
  b_chain * bc = new b_chain();
  uint64_t bucket_size = small->b->high - small->b->low;

  bc->Chain = new int64_t[bucket_size];
  bc->b_size = findNextPrime(bucket_size);
  bc->Bucket = new int64_t[bc->b_size];

  for(uint64_t i=0; i<bc->b_size; i++){
    bc->Bucket[i]=-1;
  }

  for(int64_t l=small->b->low; l<small->b->high; l++){
    bc->Chain[l-small->b->low] = bc->Bucket[h2(small->ht->rel->tuples[l].payload, bc->b_size)];
    bc->Bucket[h2(small->ht->rel->tuples[l].payload, bc->b_size)] = l - small->b->low;
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
  hash_table * hash_table_R = reorderRelation(rel_R);
  hash_table * hash_table_S = reorderRelation(rel_S);

  int list_size = hash_table_R->psum.length;
  result **res_list = (result**) malloc(list_size * sizeof(result*));

  // for each bucket
  for(uint64_t i=0; i<hash_table_R->psum.length; i++) {
    bucket* R_bucket, *S_bucket;
    bucket_hash small,large;
    bool isReversed;
    R_bucket = (bucket*) malloc(sizeof(bucket));
    S_bucket = (bucket*) malloc(sizeof(bucket));

    R_bucket->low=hash_table_R->psum.data[i];
    S_bucket->low=hash_table_S->psum.data[i];

    R_bucket->high = (i==hash_table_R->psum.length-1)? rel_R->num_tuples: hash_table_R->psum.data[i+1];
    S_bucket->high = (i==hash_table_S->psum.length-1)? rel_S->num_tuples: hash_table_S->psum.data[i+1];

    if(R_bucket->high-R_bucket->low <= S_bucket->high-S_bucket->low) {
      small.b=R_bucket;
      small.ht=hash_table_R;

      large.b=S_bucket;
      large.ht=hash_table_S;
      isReversed=false;
    }
    else {
      small.b=S_bucket;
      small.ht=hash_table_S;

      large.b=R_bucket;
      large.ht=hash_table_R;
      isReversed=true;
    }

    initResult(&res_list[i]);
    b_chain *bc = indexingSmallBucket(&small);

    jobScheduler->Schedule(new JoinJob(small, large, bc, res_list[i], isReversed));
    // jobScheduler->Barrier(); // DEBUG
  }

  jobScheduler->Barrier();

  freeHashTableAndComponents(hash_table_R);
  freeHashTableAndComponents(hash_table_S);

  LOG("Squashing result list\n");
  result* res = squashResults(res_list, list_size);

  free(res_list);
  return res;
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
