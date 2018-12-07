#include <iostream>
#include <stdint.h>
#include <string.h>
#include "../inc/header.h"
#include "../inc/result.h"


int main(int argc, char * argv[]){

  // hardcoded 2 relations R.a and S.a from example
  relation relR, relS;

  //test 1, expecting no match
  int32_t num_test=0;

  if(argc > 1 && !strcmp(argv[1], "test1"))
    num_test = 1;
  else if (argc > 1 && !strcmp(argv[1], "test2"))
    num_test = 2;


  switch(num_test){
    case 1: //
            relR.num_tuples = 1000;
            relR.tuples = new tuple[relR.num_tuples];
            for(int i = 0; i < 1000; i++ )
              relR.tuples[i] = {i + 1, 2*i};

            relS.num_tuples = 1000;
            relS.tuples = new tuple[relS.num_tuples];
            for(int i = 0; i < 1000; i++ )
              relS.tuples[i] = {i + 1, 2*i};

            break;
    case 2:
            relR.num_tuples = 1000;
            relR.tuples = new tuple[relR.num_tuples];
            for(int i = 0; i < 1000; i++ )
              relR.tuples[i] = {i + 1, 2*i};

            relS.num_tuples = 1000;
            relS.tuples = new tuple[relS.num_tuples];
            for(int i = 0; i < 1000; i++ )
              relS.tuples[i] = {i + 1, 2*i+1};

            break;
    default:
            relR.num_tuples = 4;
            relR.tuples = new tuple[relR.num_tuples];
            relR.tuples[0] = {1, 1};
            relR.tuples[1] = {2, 2};
            relR.tuples[2] = {3, 3};
            relR.tuples[3] = {4, 1};

            relS.num_tuples = 3;
            relS.tuples = new tuple[relS.num_tuples];
            relS.tuples[0] = {1, 1};
            relS.tuples[1] = {2, 1};
            relS.tuples[2] = {3, 3};

            break;
  }

  result * res_list = RadixHashJoin(&relR, &relS);
  printResults(res_list);

  destroyResult(res_list);
  delete[] relR.tuples;
  delete[] relS.tuples;
  return 0;
}
