#include <iostream>
#include "../inc/database.hpp"
#include "../inc/relation.hpp"

int main(){
  Database dbase;
  dbase.addRelation("../workloads/small/r0");
  Intermediate* intermediate;
  dbase.runFilter(Filter("0.1<5"), intermediate);
  for(int i=0; i<intermediate->filter_rowIDs.size();i++)
    std:: cout << intermediate->filter_rowIDs[i];
}
