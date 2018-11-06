#include <iostream>
#include <stdint.h>
#include <string.h>
#include "../inc/header.h"
#include "../inc/result.h"

// 1st hash function
#define H1_LAST_BITS 8
#define PRIME_NUM 101
#define h1(X) (X & ((1 << H1_LAST_BITS) - 1))


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
  for(int i = 0; i < rel->num_tuples; i++)
    printf("* %5d|%5d\n", rel->tuples[i].key, rel->tuples[i].payload);
  std::cout << "* /Printing Relation " << name << std::endl << std::endl;
}

/**
 * H2 hash function
 *
 * @params num, hash this number
 * @returns hash value
 */
inline int32_t h2(int32_t num) {
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
  hist.data = (int32_t*) calloc(sizeof(int32_t), hist.length);

  for(int32_t i = 0; i < rel->num_tuples; i++){
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
  int32_t index_mod=1;

  psum.length = hist.length;
  psum.data = (int32_t*) calloc(sizeof(int32_t),psum.length);
  psum.data[0]=0;
  for(int32_t i = 0; i < psum.length; i++){
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
int32_t set_high(hash_table* ht, int32_t index_start){
  if(index_start == ht->psum.length)
    return ht->rel->num_tuples;
  for(int32_t i = index_start; i<ht->psum.length; i++){
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
result * indexingAndCompareBuckets(hash_table *small,hash_table *large,bool isReversed) {
  int32_t *chain,*Bucket, index, lg_value, h2_res;
  //hash_table *temp;
  //bucket temp_b;
  bucket sm_b,lg_b;
  result * res_list;
  tuple res_tuple;
  initResult(&res_list);

  for(int32_t i=0; i<small->psum.length; i++){
    if(small->psum.data[i] != -1 && large->psum.data[i] != -1){
      sm_b.low=small->psum.data[i];
      lg_b.low=large->psum.data[i];

      
      sm_b.high = set_high(small,i+1);
      lg_b.high = set_high(large,i+1);


      /* Seeing about how correct is 
      if(lg_b.high-lg_b.low <= sm_b.high-sm_b.low) {
        temp_b=sm_b;
        sm_b=lg_b;
        lg_b=temp_b;
        temp=small;
        small=large;
        large=temp;
      } */
      
      //DEBUG//std::cout<<"SM LOW = "<< sm_b.low << " SM HIGH = "<< sm_b.high << std::endl;
      //DEBUG//std::cout<<"LG LOW = "<< lg_b.low << " LG HIGH = "<< lg_b.high << std::endl;

      chain=new int32_t[sm_b.high-sm_b.low];
      Bucket=new int32_t[PRIME_NUM];          //This will be changed with the next prime number

      for(int32_t j=0; j<PRIME_NUM; j++){
        Bucket[j]=-1;
      }

      for(int32_t l=sm_b.low; l<sm_b.high; l++){
        chain[l-sm_b.low]=Bucket[h2(small->rel->tuples[l].payload)];
        Bucket[h2(small->rel->tuples[l].payload)]=l-sm_b.low;
      }


      /*** Until here we have the indexing part of the algorithm where we construct the chain and Bucket structures ***/

      //HERE IS THE COMPARING PART//
      for(int32_t k = lg_b.low; k < lg_b.high; k++){
        lg_value = large->rel->tuples[k].payload;
        h2_res = h2(lg_value);
        index = Bucket[h2_res];
        while(index != -1){
          if(small->rel->tuples[index+sm_b.low].payload == lg_value){
            if(isReversed) {
              res_tuple.key = large->rel->tuples[k].key;
              res_tuple.payload = small->rel->tuples[index+sm_b.low].key;
            }
            else {
              res_tuple.key = small->rel->tuples[index+sm_b.low].key;
              res_tuple.payload = large->rel->tuples[k].key;
            }
            addToResult(res_list, &res_tuple);
          }
          index = chain[index];
        }
      }
      delete[] chain;
      delete[] Bucket;
    }
  }
  return res_list;
}

/**
 * Join two relations
 *
 * @params rel_R, 1st relation as R
 * @params rel_S, 1st relation as S
 * @returns result list
 */
result * RadixHashJoin(relation * rel_R, relation * rel_S){

  //printRelation(rel_R, "R");
  //printRelation(rel_S, "S");

  hash_table * hash_table_R = reorderRelation(rel_R);
  hash_table * hash_table_S = reorderRelation(rel_S);
  //DEBUG//std::cout << "reordering DONE\n";

  //printRelation(hash_table_R->rel, "R\'");
  //printRelation(hash_table_S->rel, "S\'");

  //Here should be the initialization of the 'list'//
  result *res_list;
  if (rel_R->num_tuples < rel_S->num_tuples)
    res_list = indexingAndCompareBuckets(hash_table_R,hash_table_S,false);
  else
    res_list = indexingAndCompareBuckets(hash_table_S,hash_table_R,true);

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
