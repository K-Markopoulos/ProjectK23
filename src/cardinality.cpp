#include <iostream>
#include <cmath>
#include <algorithm>
#include "../inc/stats.hpp"
#include "../inc/cardinality.hpp"
#include "../inc/relation.hpp"
#include "../inc/query.hpp"

vector<Stats> createCopy(Relation* rel){
  std::vector<Stats*> stat_pointers = rel->copyStatsVector();
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
  if(!exists(getRelations(), f->relId))
    stats.push_back(createCopy(f->relation));

  Stats* curr_stats = getStats(f->relId, f->col);
  uint64_t f_old = curr_stats->getf();
  uint64_t d_old = curr_stats->getd();

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
  uint64_t la_old, ua_old, fa_old, da_old;
  uint64_t lb_old, ub_old, fb_old, db_old;

  if(!exists(getRelations(), relId1))
    stats.push_back(createCopy(p->relation1));
  if(p->reId1!=p->relId2)
    if(!exists(getRelations(), p->relId2))
      stats.push_back(createCopy(p->relation2));

  Stats* curr_stats1 = getStats(p->relId1, p->col1);
  Stats* curr_stats2 = getStats(p->relId2, p->col2);

  fa_old = curr_stats1->getf();
  da_old = curr_stats1->getd();

  fb_old = curr_stats2->getf();
  db_old = curr_stats2->getd();

  if(p->relId1 == p->relId2){
    if(p->col1 != p->col2){
      la_old = curr_stats1->getl();
      ua_old = curr_stats1->getu();

      lb_old = curr_stats2->getl();
      ub_old = curr_stats2->getu();

      curr_stats1->setl((la_old > lb_old) ? la_old : lb_old);
      curr_stats1->setu((ua_old < ub_old) ? ua_old : ub_old);
      curr_stats1->setf(fa_old / (curr_stats1->getu() - curr_stats1->getl() + 1));
      curr_stats1->setd(da_old * (1 - pow(1 - curr_stats1->getf() / fa_old, fa_old / da_old)));

      curr_stats2->setl(curr_stats1->getl());
      curr_stats2->setu(curr_stats1->getu());
      curr_stats2->setf(curr_stats1->getf());
      curr_stats2->setd(curr_stats1->getd());

      for(uint64_t i=0; i<p->relation1->getColumnCount(); i++){
        if(i != p->col1 && i != p->col2){
          Stats* s = getStats(p->relId1,i);
          s->setd(s->getd() * (1 - pow(1 - curr_stats1->getf() / fa_old, s->getf() / s->getd())));
          s->setf(curr_stats1->getf());
        }
      }
    }
    else{
      curr_stats1->setf(fa_old * fa_old / (curr_stats1->getu() - curr_stats1->getl() + 1));

      for(uint64_t i=0; i<p->relation1->getColumnCount(); i++){
        if(i != p->col1){
          Stats* s = getStats(p->relId1,i);
          s->setf(curr_stats1->getf());
        }
      }
    }
  }
  else{
    curr_stats1->setl((curr_stats1->getl() > curr_stats2->getl()) ? curr_stats1->getl() : curr_stats2->getl());
    curr_stats1->setu((curr_stats1->getu() < curr_stats2->getu()) ? curr_stats1->getu() : curr_stats2->getu());
    uint64_t n = curr_stats1->getu() - curr_stats1->getl() + 1;
    curr_stats1->setf(curr_stats1->getf() * fa_old / n;
    curr_stats1->setd(curr_stats1->getd() * da_old / n;

    curr_stats2->setl(curr_stats1->getl());
    curr_stats2->setu(curr_stats1->getu());
    curr_stats2->setf(curr_stats1->getf());
    curr_stats2->setd(curr_stats1->getd());

    for(uint64_t i=0; i<p->relation1->getColumnCount(); i++){
      if(i != p->col1){
        Stats* s = getStats(p->relId1,i);
        s->setd(s->getd() * (1 - pow(1 - curr_stats1->getd() / da_old, s->getf() / s->getd())));
        s->setf(curr_stats1->getf();
      }
    }
    for(uint64_t i=0; i<p->relation2->getColumnCount(); i++){
      if(i != p->col2){
        Stats* s = getStats(p->relId2,i);
        s->setd(s->getd() * (1 - pow(1 - curr_stats2->getd() / db_old, s->getf() / s->getd())));
        s->setf(curr_stats2->getf();
      }
    }
  }
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
