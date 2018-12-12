#include <stdint.h>
#include <stdbool.h>

typedef struct result result;
typedef struct block block;

struct result {
  block * head;
  block * last;
  uint32_t num_blocks;
  uint64_t num_tuples;
};

struct block {
  tuple_ * tuples;
  uint32_t num_tuples;
  block * next;
};

void initResult(result **);
void destroyResult(result *);
void addToResult(result *, tuple_ *);
tuple_ * getNthResult(result *, int32_t);
void printResults(result *);

void initBlock(block **);
void destroyBlock(block *);
void addToBlock(block *, tuple_ *);
void setBlockNext(block *, block *);
bool isFullBlock(block *);
