#include <iostream>
#include <queue>
#include <pthread.h>
#include "../inc/jobScheduler.hpp"
#include "../inc/utils.hpp"

#ifdef LOGGER
#undef LOGGER
#define LOGGER "JobScheduler"
#endif

#undef DEBUG

void* work_(void* This) {
  return ((JobScheduler*)This)->work(NULL);
}

bool JobScheduler::Init(int num_of_threads) {
  LOG("Initializing JobScheduler\n");
  if (num_of_threads <= 0)
    return false;
  num_threads = num_of_threads;

  shutdown = false;
  dont_accept = false;

  if (!(threads = (pthread_t*) malloc(sizeof(pthread_t) * num_threads))) {
    fprintf(stderr, "Out of memory creating threads.\n");
    return false;
  }
  if (pthread_mutex_init(&qlock, NULL)) {
    fprintf(stderr, "Mutex initialization error.\n");
    return false;
  }
  if (pthread_cond_init(&q_not_empty, NULL)) {
    fprintf(stderr, "CV initialization error.\n");
    return false;
  }
  if (pthread_cond_init(&q_empty, NULL)) {
    fprintf(stderr, "CV initialization error.\n");
    return false;
  }
  if (pthread_cond_init(&job_done, NULL)) {
    fprintf(stderr, "CV initialization error.\n");
    return false;
  }

  for (int i = 0; i < num_threads; i++) {
    if (pthread_create(&threads[i], NULL, work_, this)) {
      fprintf(stderr, "Thread initialization error.\n");
      return false;
    }
  }
  return true;
}

bool JobScheduler::Destroy() {
  LOG("Destroying JobScheduler\n");
  if(!shutdown){
    LOG("WARNING! Destroying without stopping threads first\n");
  }
  num_threads = 0;
  free(threads);
  pthread_mutex_destroy(&qlock);
  pthread_cond_destroy(&q_not_empty);
  pthread_cond_destroy(&q_empty);
  return true;
}

void* JobScheduler::work(void* vargs) {
  while(1) {
    LOG("Thread(%lu) ready to work\n", pthread_self());
    pthread_mutex_lock(&qlock);
    while (jobQueue.size() == 0) {
      if(shutdown){
        pthread_mutex_unlock(&qlock);
        LOG("Thread(%lu) shutting down\n", pthread_self());
        pthread_exit(NULL);
      }

      pthread_cond_wait(&q_not_empty, &qlock);

      if(shutdown){
        pthread_mutex_unlock(&qlock);
        LOG("Thread(%lu) shutting down\n", pthread_self());
        pthread_exit(NULL);
      }
    }

    active++;
    Job* job = jobQueue.front();
    jobQueue.pop();
    LOG("Thread(%lu) got a job from queue, remaining jobs %lu\n", pthread_self(), jobQueue.size());
    if (jobQueue.size() == 0 && !shutdown) {
      pthread_cond_signal(&q_empty);
    }
    pthread_mutex_unlock(&qlock);


    LOG("Running job\n");
    job->Run();
    pthread_mutex_lock(&qlock);
    active--;
    pthread_mutex_unlock(&qlock);
    pthread_cond_signal(&job_done);

    delete job;
  }
}


void JobScheduler::Barrier(){
  LOG("Waiting for queued jobs to execute\n");
  if(shutdown){
    LOG("WARNING! Barrier call after shutdown\n");
  }

  pthread_mutex_unlock(&qlock);
  while (jobQueue.size() || active) {
    pthread_cond_wait(&job_done, &qlock);
  }
  pthread_mutex_unlock(&qlock);

  LOG("All jobs were executed\n");
}

JobID JobScheduler::Schedule(Job* job) {
  if(dont_accept){
    return -1;
  }

  static JobID id = 0;
  LOG("Scheduling new job: %d\n", id);

  pthread_mutex_lock(&qlock);

  jobQueue.push(job);
  if(jobQueue.size() == 1){
    pthread_cond_signal(&q_not_empty);
  }

  pthread_mutex_unlock(&qlock);

  return id++;
}

void JobScheduler::Stop() {
  LOG("Waiting for all threads to stop\n");
  pthread_mutex_lock(&qlock);

  dont_accept = true;
  while (jobQueue.size()) {
    pthread_cond_wait(&q_empty, &qlock);
  }
  shutdown = true;

  pthread_mutex_unlock(&qlock);

  for (int i = 0; i < num_threads; i++) {
    pthread_cond_broadcast(&q_not_empty);
    pthread_join(threads[i], NULL);
  }
  LOG("All threads have stopped\n");
}
