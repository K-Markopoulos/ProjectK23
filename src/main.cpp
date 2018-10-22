#include <iostream>
<<<<<<< HEAD
#include <stdint.h>
#include "../inc/header.h"
=======
#include <string>
#include "header.h"
>>>>>>> b364fd09f21c4c596eb7c444eea919aef5a19f15

using namespace std;

int main(int argc, char * argv[]){

  // hardcoded 2 relations R.a and S.a from example
  struct db::relation reIR, reIS;

  reIR.num_tuples = 4;
  reIR.tuples = new struct db::tuple[reIR.num_tuples];
  reIR.tuples[0] = {1,1};
  reIR.tuples[1] = {2,2};
  reIR.tuples[2] = {3,3};
  reIR.tuples[3] = {4,4};

  reIS.num_tuples = 3;
  reIS.tuples = new struct db::tuple[reIS.num_tuples];
  reIS.tuples[0] = {1,1};
  reIS.tuples[1] = {2,1};
  reIS.tuples[2] = {3,3};

  //struct result * result = RadixHashJoin(relR, relS);
}
