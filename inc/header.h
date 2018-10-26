#include <iostream>
#include <stdint.h>

typedef struct tuple tuple;
typedef struct relation tuple;
typedef struct result tuple;

namespace db{

  struct tuple {
    int32_t key;
    int32_t payload;
  };

  struct relation {
    tuple *tuples;
    uint32_t num_tuples;
  };

  struct result {
    tuple *tuples;
  };

}

int considered(int32_t, tuple *);
int size(tuple *);
tuple * histogram(tuple *);
tuple * psum(tuple *);
int32_t getposition(int32_t, tuple*);
tuple * reorder(tuple *, tupne *, tuple *);
