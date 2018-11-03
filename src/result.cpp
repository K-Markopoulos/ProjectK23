#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "../inc/header.h"
#include "../inc/result.h"

#define RESULT_BLOCK_SIZE 1024*1024
#define RESULT_BLOCK_MAX_TUPLES (1024*1024 / sizeof(tuple))
//#define RESULT_BLOCK_MAX_TUPLES 20

/* Result Functions */
void initResult(result ** res){
  if(res != NULL){
    *res = (result*) malloc(sizeof(result));
    initBlock(&(*res)->head);
    (*res)->last = (*res)->head;
    (*res)->num_blocks = 1;
  }
}

void destroyResult(result * res){
  block * block_current = res->head, * block_next = res->head;
  while(block_next != NULL){
    block_next = block_current->next;
    destroyBlock(block_current);
    block_current = block_next;
  }
  free(res);
}

void addToResult(result * res, tuple * data){
  if(isFullBlock(res->last)){
    res->num_blocks++;
    block * new_block;
    initBlock(&new_block);
    setBlockNext(res->last, new_block);
    res->last = new_block;
  }
  addToBlock(res->last, data);
}

tuple * getNthResult(result * res, int32_t n){
    int32_t num_block = n / RESULT_BLOCK_MAX_TUPLES;
    int32_t num_tuple = n % RESULT_BLOCK_MAX_TUPLES;
    int32_t block_counter = 0;
    block * bl = res->head;
    while(block_counter++ != num_block){
      bl = bl->next;
      if(bl == NULL){
        return NULL;
      }
    }
    if(num_block + 1 > bl->num_tuples)
      return NULL;
    return (tuple *) &bl->tuples[num_tuple];
}

void printResults(result * res_list){
  int32_t res_counter = 1;
  block * curr_block = (block *) malloc(sizeof(block));
  curr_block = res_list->head;
  for(int32_t i = 0; i < res_list->num_blocks; i++){
    for(int32_t j = 0; j < curr_block->num_tuples; j++)
      std::cout << "Result " << res_counter++ << ": " << curr_block->tuples[j].key << " - " << curr_block->tuples[j].payload << std::endl;
    curr_block = curr_block->next;
  }
}

/* Block Functions */
void initBlock(block ** bl){
  if(bl != NULL){
    *bl = (block*) malloc(sizeof(block));
    (*bl)->tuples = (tuple*) malloc(RESULT_BLOCK_SIZE);
    (*bl)->num_tuples = 0;
    (*bl)->next = NULL;
  }
}

void destroyBlock(block * bl){
  free(bl->tuples);
  free(bl);
}

void addToBlock(block * block, tuple * data){
  block->tuples[block->num_tuples++] = *data;
}

void setBlockNext(block * bl, block * next){
  bl->next = next;
}

bool isFullBlock(block * bl){
  return bl->num_tuples == RESULT_BLOCK_MAX_TUPLES;
}



#ifdef TEST_RESULT
int main(int argc, char * argv[]){
  result * res;
  initResult(&res);

  tuple * data = (tuple * ) malloc(sizeof(tuple));

  std::cout << "------ Pushing -----\n";

  for(int i = 0; i < 100; i++){
      data->key = i;
      data->payload = i;
      addToResult(res, data);
      std::cout << " > (" << data->key << ',' << data->payload << ")\n";
  }

  free(data);

  std::cout << "------ Retreiving -----\n";

  for(int i = 0; i < 100; i++){
    if((data = getNthResult(res, i)) == NULL){
      std::cout << "Error: Index (" << i << ") out of borders\n";
      continue;
    }
    std::cout << " < (" << data->key << ',' << data->payload << ")\n";
  }

  destroyResult(res);
  return 0;
}
#endif
