#include <iostream>
#include <stdint.h>
#include "../inc/header.h"
#include "../inc/result.h"


int main(int argc, char * argv[]){

  // hardcoded 2 relations R.a and S.a from example
  relation relR, relS;

  relR.num_tuples = 4;
  relR.tuples = new tuple[relR.num_tuples];
  relR.tuples[0] = {1,1};
  relR.tuples[1] = {2,2};
  relR.tuples[2] = {3,3};
  relR.tuples[3] = {4,1};

  relS.num_tuples = 3;
  relS.tuples = new tuple[relS.num_tuples];
  relS.tuples[0] = {1,1};
  relS.tuples[1] = {2,1};
  relS.tuples[2] = {3,3};

  result * res_list = RadixHashJoin(&relR, &relS);
  printResults(res_list);

  destroyResult(res_list);
  delete[] relR.tuples;
  delete[] relS.tuples;
  return 0;
}
