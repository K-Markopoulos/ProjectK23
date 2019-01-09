#include <iostream>
#include <cmath>
#include <algorithm>
#include "../inc/stats.hpp"
#include "../inc/cardinality.hpp"
#include "../inc/relation.hpp"
#include "../inc/query.hpp"

vector<Stats> createCopy(Filter* f){
  std::vector<Stats*> stat_pointers = f->relation->copyStatsVector();
  std::vector<Stats> stats;
  uint64_t s = stat_pointers.size();
  stats.size(s);
  for(uint64_t i=0; i<s; i++){
    stats[i] = stat_pointers[i]->copyStats();
  }
  return stats;
}

bool exists(vector<uint64_t> rel, uint64_t v){
  for(uint64_t x : rel)
    if(x == v)
      return true;
  return false;
}

Cardinality(Query* q){
  this->query = q;
}

void Cardinality::mainAssess(){
  for(Filter* f : this->query->filters){
    this->assess(f);
  }
}

void Cardinality::assess(Filter* f){
  if(!exists(getRelations(), f->relation->getId())){
    stats.push_back(createCopy(f));
  }

  Stats* curr_stats = getStats(f->relId, f->col);
  f_old = curr_stats->getf();
  d_old = curr_stats->getd();

  // Current Column
  if(f->op == '='){
    if(f->value >= curr_stats->getl() && f->value <= curr_stats->getu()){
      curr_stats->setd(1);
      curr_stats->setf(f_old / d_old);
    }
    else{
      curr_stats->setd(0);
      curr_stats->setf(0);
    }
  }
  else{
    uint64_t k2 = f->value;
    uint64_t l_old = curr_stats->getl();
    uint64_t u_old = curr_stats->getu();
    k1 = (op == '<') ? l_old : u_old;
    (k1 > k2) ? swap(k1,k2);

    curr_stats->setl(k1);
    curr_stats->setu(k2);
    curr_stats->setd(d_old * (k2 - k1) / (u_old - l_old));
    curr_stats->setf(f_old * (k2 - k1) / (u_old - l_old));
  }
  // Rest Columns
  for(uint64_t i=0; i<f->relation->getColumnCount(); i++){
    if(i != f->col){
      Stats* s = getStats(f->relId,i);
      s->setd(s->getd() * (1 - pow(1 - curr_stats->getf() / f_old, s->getf() / s->getd())));
      s->setf(curr_stats->getf());
    }
  }
}

void Cardinality::assess(Predicate* p){
  uint64_t old_la = this->statsA->getl();
  uint64_t old_lb = this->statsB->getl();
  uint64_t old_ua = this->statsA->getu();
  uint64_t old_ub = this->statsB->getu();
  uint64_t old_fa = this->statsA->getf();
  uint64_t old_da = this->statsA->getd();

  this->la = this->lb = (old_la > old_lb) ? old_la : old_lb;
  this->ua = this->ub = (old_ua < old_ub) ? old_ua : old_ub;
  this->fa = this->fb = old_fa / (this->ua - this->la + 1);
  this->da = this->db = old_da * (1 - pow(1 - this->fa / old_fa, old_fa / old_da));

  Relations* rel = this->statsA->getRelation();
  void* col1 = this->statsA->getColumn();
  void* col2 = this->statsB->getColumn();
  for(Stats* s : rel->stats){
    if(s->getColumn() != col1 && s->getColumn() != col2){
      s->setd(s->getd() * (1 - pow(1 - this->fa / old_fa, s->getf() / s->getd())));
      s->setf(this->fa);
    }
  }
}

void Cardinality::assess(2){
  uint64_t old_la = this->statsA->getl();
  uint64_t old_lb = this->statsB->getl();
  uint64_t old_ua = this->statsA->getu();
  uint64_t old_ub = this->statsB->getu();
  uint64_t old_da = this->statsA->getd();
  uint64_t old_db = this->statsB->getd();

  if(old_la > old_lb){
    this->statsB->setl(old_la);
    this->la = this->lb = old_la;
  }
  else{
      this->statsA->setl(old_lb);
      this->la = this->lb = old_lb;
  }

  if(old_ua < old_ub){
    this->statsB->setl(old_ua);
    this->la = this->lb = old_ua;
  }
  else{
      this->statsA->setl(old_ub);
      this->la = this->lb = old_ub;
  }

  uint64_t n = this->statsA->getu() - this->statsA->getb() + 1;
  this->fa = this->fb = this->statsA->getf() * this->statsB->getf() / n;
  this->da = this->db = old_da * old_db / n;

  Relations* relA = this->statsA->getRelation();
  Relations* relB = this->statsB->getRelation();
  void* colA = this->statsA->getColumn();
  void* colB = this->statsB->getColumn();
  for(Stats* s : relA->stats){
    if(s->getColumn() != colA){
      s->setf(s->getd() * (1 - pow(1 - this->da / old_da, s->getf() / s->getd())));
      s->setf(this->fa);
    }
  }
  for(Stats* s : relB->stats){
    if(s->getColumn() != colB){
      s->setf(s->getd() * (1 - pow(1 - this->db / old_db, s->getf() / s->getd())));
      s->setf(this->fa);
    }
  }
}

void Cardinality::assess(3){
  uint64_t old_la = this->statsA->getl();
  uint64_t old_ua = this->statsA->getu();
  uint64_t old_fa = this->statsA->getf();

  this->la = old_la;
  this->ua = old_ua;
  this->fa = old_fa * old_fa / (old_ua - old_la + 1);
  this->da = this->statsA->getd();

  Relations* rel = this->statsA->getRelation();
  void* col = this->statsA->getColumn();
  for(Stats* s : rel->stats)
    if(s->getColumn() != col)
      s->setf(this->fa);
}

std::vector<vector<Stats>>& Cardinality::getStatsVector(){
  return &stats;
}

std::vector<Stats>& Cardinality::getRelStats(uint64_t i){
  return &stats[i];
}

Stats& Cardinality::getStats(uint64_t i, uint64_t j){
  return &stats[i][j];
}

std::vector<uint64_t> Cardinality::getRelations(){
  return relations;
}
