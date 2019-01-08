#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "../inc/radix.h"
#include "../inc/result.h"


/**
 * Initialize result list
 *
 * @params res, the result struct
 */
void initResult(result ** res){
  if(res != NULL){
    *res = (result*) malloc(sizeof(result));
    initBlock(&(*res)->head);
    (*res)->last = (*res)->head;
    (*res)->num_blocks = 1;
    (*res)->num_tuples = 0;
  }
}

/**
 * Destroy the struct
 *
 * @params res, the result struct
 */
void destroyResult(result * res){
  block * block_current = res->head, * block_next = res->head;
  while(block_next != NULL){
    block_next = block_current->next;
    destroyBlock(block_current);
    block_current = block_next;
  }
  free(res);
}

/**
 * Add a tuple to result list
 *
 * @params res, the result struct
 * @params data, the tuple struct
 */
void addToResult(result * res, tuple_ * data){
  res->num_tuples++;
  if(isFullBlock(res->last)){
    res->num_blocks++;
    block * new_block;
    initBlock(&new_block);
    setBlockNext(res->last, new_block);
    res->last = new_block;
  }
  addToBlock(res->last, data);
}

/**
 * (Replaced by getResult)
 * Get Nth tuple from result list
 *
 * @params res, the result struct
 * @params n, index of tuple (starting at 0)
 */
tuple_ * getNthResult(result * res, uint64_t n){
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
    return (tuple_ *) &bl->tuples[num_tuple];
}

/**
 * Get current tuple in result and increment current to the next result
 *
 * @params res, the result struct
 */
tuple_ * getResult(result * res){
  tuple_* value = res->current_tuple;
  if (res->current_tuple_num < res->num_tuples) {
    res->current_tuple_num += 1;
    res->current_tuple_index += 1;
    // int32_t index_tuple = res->current_tuple_num % RESULT_BLOCK_MAX_TUPLES;
    if (res->current_tuple_index == res->current_block->num_tuples) {
      res->current_block = res->current_block->next;
      res->current_tuple = res->current_block->tuples;
      res->current_tuple_index = 0;
    }
    else {
      res->current_tuple += 1;
    }
  } else {
    return NULL;
  }

  return value;
}

/**
 * Initialize iterator(current_tuple)
 *
 * @params res, the result struct
 */
void initIterator(result* res){
  res->current_block = res->head;
  res->current_tuple = res->current_block->tuples;
  res->current_tuple_num = 0;
  res->current_tuple_index = 0;
}

/**
 * Set iterator(current_tuple) to the nth result
 *
 * @params res, the result struct
 * @params n, index of tuple (starting at 0)
 */
void setIterator(result* res, uint64_t n){
  int32_t num_block = n / RESULT_BLOCK_MAX_TUPLES;
  res->current_tuple_num = n % RESULT_BLOCK_MAX_TUPLES;
  int32_t block_counter = 0;
  res->current_block = res->head;
  while(block_counter++ != num_block){
    res->current_block = res->current_block->next;
    if(res->current_block == NULL)
      return;
  }
  if(num_block + 1 > res->current_block->num_tuples)
    return;
  res->current_tuple = &res->current_block->tuples[res->current_tuple_num];
}

/**
 * Squash a list of results in one, by linking their blocks
 *
 * @params res_list, list of results
 * @params list_size, size of list
 */
void squashResults(result** res_list, int list_size) {
  result* res = res_list[0];

  for(int i = 1; i < list_size; i++) {
    res->num_tuples += res_list[i]->num_tuples;
    res->num_blocks += res_list[i]->num_blocks;
    res->last->next = res_list[i]->head;
    res->last = res_list[i]->last;
  }
}

/**
 * Printing the tuples of the res_list
 *
 * @params res_list
 */
void printResults(result * res_list){
  int32_t res_counter = 1;
  block * curr_block = res_list->head;
  for(int32_t i = 0; i < res_list->num_blocks; i++){
    for(int32_t j = 0; j < curr_block->num_tuples; j++)
      std::cout << "Result " << res_counter++ << ": " << curr_block->tuples[j].key << " - " << curr_block->tuples[j].payload << std::endl;
    curr_block = curr_block->next;
  }
}


/********** BLOCK FUNCTIONS **********/

/**
 * Initialize a block strust
 *
 * @params bl, the block struct
 */
void initBlock(block ** bl){
  if(bl != NULL){
    *bl = (block*) malloc(sizeof(block));
    (*bl)->tuples = (tuple_*) malloc(RESULT_BLOCK_SIZE);
    (*bl)->num_tuples = 0;
    (*bl)->next = NULL;
  }
}

/**
 * Destroy a block struct
 *
 * @params bl, the block struct
 */
void destroyBlock(block * bl){
  free(bl->tuples);
  free(bl);
}

/**
 * Add a tuple to the end of block
 *
 * @params block, the block struct
 * @params data, the tuple struct
 */
void addToBlock(block * block, tuple_ * data){
  block->tuples[block->num_tuples++] = *data;
}

/**
 * Set the next block of a block
 *
 * @params bl, the block to update
 * @params next, the next block
 */
void setBlockNext(block * bl, block * next){
  bl->next = next;
}

/**
 * Checks if block is full of tuples
 *
 * @params block, the block struct
 */
bool isFullBlock(block * bl){
  return bl->num_tuples == RESULT_BLOCK_MAX_TUPLES;
}

/**
 * Test for result struct and functions
 *
 */
#ifdef TEST_RESULT
int main(int argc, char * argv[]){
  result * res;
  initResult(&res);

  tuple_ * data = (tuple_ * ) malloc(sizeof(tuple_));
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
