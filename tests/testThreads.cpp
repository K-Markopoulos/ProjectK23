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
  Job* job1 = new CountingJob();
  Job* job2 = new PrintingJob();
  Job* job3 = new SleepingJob();

  if (!js->Init(num_threads)) {
    fprintf(stderr, "JobScheduler initialization error.\n");
    return 1;
  }

  for (int i = 0; i < num_jobs; i++){
    // Job* job = rand() % 2 ? job1: job2;
    js->Schedule(job3);
  }

  js->Barrier();

  for (int i = 0; i < num_jobs; i++){
    Job* job = rand() % 2 ? job1: job2;
    js->Schedule(job);
  }

  js->Stop();

  delete job1;
  delete job2;
  delete job3;

  if (!js->Destroy()) {
    fprintf(stderr, "JobScheduler failed to be destroyed.\n");
    return 1;
  }
  delete js;

  LOG("Test finished\n");
  pthread_exit(NULL);
}
