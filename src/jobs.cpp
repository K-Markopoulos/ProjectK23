#include <iostream>
#include "../inc/jobScheduler.hpp"
#include "../inc/radix.h"
#include "../inc/result.h"
#include "../inc/jobs.hpp"
#include "../inc/utils.hpp"

#ifdef LOGGER
#undef LOGGER
#define LOGGER "JOBS"
#endif


/**
 * HistogramJob Constructor
 *
 * @params low, low index in relation's tuples
 * @params high, high index in relation's tuples
 * @params hist, histogram to store results
 * @params relation, relation to scan
 */
HistogramJob::HistogramJob(int low_, int high_, array_int* hist_, relation* rel_):
  low(low_), high(high_), hist(hist_), rel(rel_) {
    LOG("New HistogramJob %d-%d\n", low, high);
}

/**
 * Scans relation from low to high and updates the histogram
 * @return true if everything done right false else.
 */
int HistogramJob::Run() {
  for(uint64_t i = low; i < high; i++){
    hist->data[h1(rel->tuples[i].payload)]++;
  }
  return true;
}

/**
 * PartitionJob Constructor
 *
 * @params relation, relation to scan
 * @params relation, new reordered relation
 * @params psum, psum array
 */
PartitionJob::PartitionJob(int low_, int high_, relation* rel_, relation* new_rel_, array_int* psum_):
  low(low_), high(high_), rel(rel_), new_rel(new_rel_), psum(psum_) {
    LOG("New PartitionJob %d-%d\n", low, high);
}

/**
 * Scans relation from low to high and updates the histogram
 * @return true if everything done right false else.
 */
int PartitionJob::Run() {
  int64_t index;
  LOG("Running partition job %d-%d rel size:%lu", low, high, rel->num_tuples);
  for(int64_t i = low; i < high; i++){
    index = h1(rel->tuples[i].payload);
    new_rel->tuples[psum->data[index]] = rel->tuples[i];
    psum->data[index]++;
  }
  return true;
}

/**
 * JoinJob Constructor
 *
 * @params small, relation to scan
 * @params large, new reordered relation
 * @params psum, psum array
 */
JoinJob::JoinJob(bucket_hash small_, bucket_hash large_ ,b_chain *bc_ ,result *res_,bool isReversed_):
  small(small_), large(large_), bc(bc_), res(res_), isReversed(isReversed_) {
    LOG("New JoinJob  ++> s:%p s.b:%p l:%p l.b:%p\n", &small, small.b, &large, large.b);
}

JoinJob::~JoinJob() {
  LOG("(%d)DELETE by thread %ld BC %p\n", id, pthread_self(), bc);
  delete[] bc->Chain;
  delete[] bc->Bucket;
  delete bc;
  free(small.b);
  free(large.b);
}

/**
 * Compare buckets and store results to res
 * @return true if everything done right false else.
 */
int JoinJob::Run() {
  int64_t lg_value, index;
  tuple_ res_tuple;
  tuple_* s_tuples = small.ht->rel->tuples;
  tuple_* l_tuples = large.ht->rel->tuples;
  int64_t sm_low=small.b->low;

  if(isReversed) {
    for(int64_t k=large.b->low; k<large.b->high; k++) {
      lg_value = l_tuples[k].payload;
      index = bc->Bucket[h2(lg_value, bc->b_size)];
      while (index != -1) {
        if(s_tuples[index+sm_low].payload == lg_value) {
          res_tuple.key = l_tuples[k].key;
          res_tuple.payload = s_tuples[index+sm_low].key;

          addToResult(res, &res_tuple);
        }
        index = bc->Chain[index];
      }
    }
  } else {
    for(int64_t k=large.b->low; k<large.b->high; k++) {
      lg_value = l_tuples[k].payload;
      index = bc->Bucket[h2(lg_value, bc->b_size)];
      while (index != -1) {
        if(s_tuples[index+sm_low].payload == lg_value) {
          res_tuple.key = s_tuples[index+sm_low].key;
          res_tuple.payload = l_tuples[k].key;

          addToResult(res, &res_tuple);
        }
        index = bc->Chain[index];
      }
    }
  }
  return true;
}
