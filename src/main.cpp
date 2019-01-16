#include <iostream>
#include <string.h>
#include "../inc/database.hpp"
#include "../inc/query.hpp"
#include "../inc/utils.hpp"
#include "../inc/jobScheduler.hpp"

Database* db;
JobScheduler* jobScheduler;
elapsed_timer elapsed;

using namespace std;

int main(int argc, char* argv[]) {
  db = new Database();
  jobScheduler = new JobScheduler();
  jobScheduler->Init(NUM_THREADS);
  string line;
  elapsed = {0,0,0,0,0,0,0,0};

  clock_t start = clock();

  //phase 1: reading relations' paths
  while (getline(cin, line)) {
    if (line == "Done" || line.empty()) break;
    db->addRelation("./workloads/small/"+line);
  }

  elapsed.loading += (double)(clock() - start) / CLOCKS_PER_SEC;

  //phase 2: reading query batches
  LOG("\n\n\nREADING QUERIES\n\n\n");
  Query query;
  start = clock();

  while (getline(cin, line)) {
    if (line == "F" || line.empty()) {
      continue;
    }
    if(!query.parseQuery(line)){
      cout << "Invalid query, skipping...\n";
      continue;
    }
    cout << db->run(query).c_str() << endl;
    query.clear();
  }
  cout << endl;

  elapsed.running += (double)(clock() - start) / CLOCKS_PER_SEC;

  jobScheduler->Stop();
  jobScheduler->Destroy();
  delete db;
  delete jobScheduler;

  // print times
  if(argc > 1 && !strcmp(argv[1], "-t")) {
    printf("Total times:\n \
      -Loading:   \t%f\n \
      -Running:   \t%f\n \
      --Radix:     \t%f\n \
      --Filters:   \t%f\n \
      --Predicates:\t%f\n \
      --Selectors: \t%f\n \
      --I. build:  \t%f\n \
      --I. update: \t%f\n",
      elapsed.loading, elapsed.running, elapsed.radix, elapsed.filters, elapsed.predicates,
      elapsed.selectors, elapsed.intermediate_build, elapsed.intermediate_update);
  }
  return 0;
}
