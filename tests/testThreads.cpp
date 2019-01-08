#include <iostream>
#include <unistd.h>
#include "../inc/jobScheduler.hpp"
#include "../inc/utils.hpp"

class CountingJob: public Job {
  public:
    int Run() {
      for(int i = 0; i < 1000000; i++){
        i++;
        i--;
      }

      printf("Hello\n");
      return 0;
    };
};

class PrintingJob: public Job {
  public:
    int Run() {
      sleep(1);
      for(int i = 0; i < 1000000; i++){
        snprintf(NULL, 0, "Just a random string");
      }

      printf("Hey\n");
      return 0;
    };
};

class SleepingJob: public Job {
  public:
    int Run() {
      sleep(1);
      printf("Just woke up\n");
      return 0;
    };
};

int main(int argc, char* argv[]) {
  int num_threads, num_jobs;
  /* parsing argv */
  if(argc < 2 || (num_threads = atoi(argv[1])) < 1)
    num_threads = 4;
  if(argc < 3 || (num_jobs = atoi(argv[2])) < 1)
    num_threads = 10;

  LOG("Starting test with %d threads\n", num_threads);
  JobScheduler* js = new JobScheduler();

  if (!js->Init(num_threads)) {
    fprintf(stderr, "JobScheduler initialization error.\n");
    return 1;
  }

  for (int i = 0; i < num_jobs; i++){
    js->Schedule(new SleepingJob());
  }

  js->Barrier();
  printf("\n\n\nThis is after barrier\n\n\n");

  for (int i = 0; i < num_jobs; i++){
    if(rand() % 2)  js->Schedule(new CountingJob());
    else            js->Schedule(new PrintingJob());
  }

  js->Stop();

  if (!js->Destroy()) {
    fprintf(stderr, "JobScheduler failed to be destroyed.\n");
    return 1;
  }
  delete js;

  LOG("Test finished\n");
  pthread_exit(NULL);
}
