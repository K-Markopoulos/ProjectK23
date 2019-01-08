#include <stdint.h>
#include <stdbool.h>

#define RESULT_BLOCK_SIZE 128*1024
#define RESULT_BLOCK_MAX_TUPLES (RESULT_BLOCK_SIZE / sizeof(tuple_))

typedef struct result result;
typedef struct block block;

struct result {
  block * head;
  block * last;
  uint64_t num_blocks;
  uint64_t num_tuples;
  tuple_* current_tuple;
  block* current_block;
  uint64_t current_tuple_num;
  uint64_t current_tuple_index;
};

struct block {
  tuple_ * tuples;
  uint64_t num_tuples;
  block * next;
};

void initResult(result **);
void destroyResult(result *);
void addToResult(result *, tuple_ *);
tuple_ * getNthResult(result *, uint64_t);
tuple_ * getResult(result *);
void initIterator(result *);
void setIterator(result *, uint64_t);
void squashResults(result** res_list, int list_size);
void printResults(result *);

void initBlock(block **);
void destroyBlock(block *);
void addToBlock(block *, tuple_ *);
void setBlockNext(block *, block *);
bool isFullBlock(block *);
