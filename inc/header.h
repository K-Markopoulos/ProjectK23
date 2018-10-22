#include <iostream>
#include <stdint.h>

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
