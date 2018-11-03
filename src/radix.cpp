#include <iostream>
#include <stdint.h>
#include <string.h>
#include "../inc/header.h"
#include "../inc/result.h"

// 1st hash function
#define H1_LAST_BITS 3
#define PRIME_NUM 101
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

/*print an array for debugging,needs some fixing
void printArray(int32_t *arr,int32_t length) {
  for(int32_t i=0; i<length; i++) {
    std::cout<<i<<":";
    std::cout<<arr[i];

    std::cout<<std::endl;
  }
}
*/

inline int32_t h2(int32_t num) {
  return num % PRIME_NUM; //Later we are going to find the next prime from length
}

array_int createHistogram(relation * rel){
  array_int hist;
  hist.length = (1 << H1_LAST_BITS);
  hist.data = (int32_t*) calloc(sizeof(int32_t), hist.length);

  for(int32_t i = 0; i < rel->num_tuples; i++){
    hist.data[h1(rel->tuples[i].payload)]++;
  }
  std::cout<<"Hist\n";
  for(int32_t i=0; i< hist.length;i++){
    std::cout << hist.data[i]<< std::endl;
  }
  return hist;
}

array_int createHistIndex(array_int hist){
  array_int hist_index;
  int32_t count = 0;
  for(int32_t i=0;i<hist.length;i++)
    if(hist.data[i] != 0){
      count ++;
      if(count == 1)
        hist_index.data = (int32_t*) malloc(count * sizeof(int32_t));
      else
        hist_index.data = (int32_t*) realloc(hist_index.data, count * sizeof(int32_t));
      hist_index.data[count-1]=i;
    }
  hist_index.length = count;
  return hist_index;
}

array_int createPsum(array_int hist){
  array_int psum, hist_index;
  int32_t index_mod=1;
  hist_index = createHistIndex(hist);

  std::cout<<"hist_index\n";
  for(int32_t i=0; i< hist_index.length;i++){
    std::cout << hist_index.data[i]<< std::endl;
  }

  psum.length = hist.length;
  psum.data = (int32_t*) malloc(sizeof(int32_t) * psum.length);
  for(int32_t i = 0; i < psum.length; i++){
    psum.data[i]=-1;
  }
  psum.data[hist_index.data[0]] = 0;
  for(int32_t i = 1; i<hist_index.length;i++){
    psum.data[hist_index.data[i]] = psum.data[hist_index.data[i-1]] + hist.data[hist_index.data[i-1]];
  }
  std::cout<<"Psum\n";
  for(int32_t i=0; i< psum.length;i++){
    std::cout << psum.data[i]<< std::endl;
  }
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

int32_t set_high(hash_table* ht, int32_t index_start){
  for(int32_t i = index_start; i<ht->psum.length; i++){
    if(ht->psum.data[i]!=-1)
      return ht->psum.data[i];
  }
  return ht->rel->num_tuples;
}

result * indexingAndCompareBuckets(hash_table *small,hash_table *large) {
  int32_t *chain,*Bucket, index, lg_value, h2_res;
  bucket sm_b,lg_b;
  result * res_list;
  tuple res_tuple;
  initResult(&res_list);
  for(int32_t i=0; i<small->psum.length; i++){
    if(small->psum.data[i] != -1 && large->psum.data[i] != -1){
      sm_b.low=small->psum.data[i];
      lg_b.low=large->psum.data[i];

      sm_b.high = set_high(small, i+1);
      lg_b.high = set_high(large, i+1);

      std::cout<<"SM LOW = "<< sm_b.low << " SM HIGH = "<< sm_b.high << std::endl;
      std::cout<<"LG LOW = "<< lg_b.low << " LG HIGH = "<< lg_b.high << std::endl;

      chain=new int32_t[sm_b.high - sm_b.low];
      Bucket=new int32_t[PRIME_NUM];          //This will be changed with the next prime number

      for(int32_t j=0; j<PRIME_NUM; j++){
        Bucket[j]=-1;
      }

      for(int32_t l=sm_b.low; l<sm_b.high; l++){
        chain[l-sm_b.low]=Bucket[h2(small->rel->tuples[l].payload)];
        Bucket[h2(small->rel->tuples[l].payload)]=l;
      }

      /*** Until here we have the indexing part of the algorithm where we construct the chain and Bucket structures ***/

      //HERE IS THE COMPARING PART//
      for(int32_t k = lg_b.low; k < lg_b.high; k++){
        lg_value = large->rel->tuples[k].payload;
        h2_res = h2(lg_value);
        index = Bucket[h2_res];
        while(index != -1){
          if(small->rel->tuples[index].payload == lg_value){
            res_tuple.key = small->rel->tuples[index].key;
            res_tuple.payload = large->rel->tuples[k].key;
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


result * RadixHashJoin(relation * rel_R, relation * rel_S){

  printRelation(rel_R, "R");
  printRelation(rel_S, "S");

  hash_table * hash_table_R = reorderRelation(rel_R);
  hash_table * hash_table_S = reorderRelation(rel_S);
  std::cout << "reordering DONE\n";

  printRelation(hash_table_R->rel, "R\'");
  printRelation(hash_table_S->rel, "S\'");

  //Here should be the initialization of the 'list'//
  result *res_list;
  if (rel_R->num_tuples < rel_S->num_tuples)
    res_list = indexingAndCompareBuckets(hash_table_R,hash_table_S);
  else
    res_list = indexingAndCompareBuckets(hash_table_S,hash_table_R);
  return res_list;
}
