#include <iostream>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "../inc/header.h"
#include "../inc/result.h"

#define RESULT_BLOCK_SIZE 1024*1024
#define RESULT_BLOCK_MAX_TUPLES (1024*1024 / sizeof(tuple))

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
    while(block_counter != block_num){
      bl = bl->next;
      if(bl == NULL){
        return NULL;
      }
    }
    if(bl->tuple_num - 1 > num_block)
      return NULL;
    return bl->tuples[num_tuple];
}

/* Block Functions */
void initBlock(block ** bl){
  if(bl != NULL){
    *bl = (block*) malloc(sizeof(block));
    (*bl)->tuples = (tuple*) malloc(RESULT_BLOCK_SIZE);
    (*bl)->num_tuples = 1;
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
