#include <iostream>
#include <cmath>
#include <algorithm>
#include "../inc/cardinality.hpp"
#include "../inc/query.hpp"

Cardinality::Cardinality(const Query* q){
  for (int r = 0; r < q->getRelationsCount(); r++) {
    Relation* rel = q->getRelation(r);
    relations.push_back(rel->getId());
    stats.push_back(rel->getStats());
  }
}

void Cardinality::assess(const Filter* f){
  Stats* curr_stats = &stats[f->relId][f->col];
  uint64_t f_old = curr_stats->getf();
  uint64_t d_old = curr_stats->getd();

  // Current Column
  if(f->op == '='){
    curr_stats->setl(f->value);
    curr_stats->setu(f->value);
    if(f->value >= curr_stats->getl() && f->value <= curr_stats->getu()){
      curr_stats->setd(1);
      curr_stats->setf(d_old ? f_old / d_old : 0);
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
    uint64_t k1 = (f->op == '<') ? l_old : u_old;
    if(k1 > k2) swap(k1,k2);

    curr_stats->setl(k1);
    curr_stats->setu(k2);
    curr_stats->setd((u_old - l_old)? d_old * (k2 - k1) / (u_old - l_old): 0);
    curr_stats->setf((u_old - l_old)? f_old * (k2 - k1) / (u_old - l_old): 0);
  }
  // Rest Columns
  for(uint64_t i=0; i<stats[f->relId].size(); i++){
    if(i != f->col){
      Stats* s = &stats[f->relId][i];
      if(s->getd() == 0 || f_old == 0)
        s->setd(0);
      else
        s->setd(s->getd() * (1 - pow(1 - curr_stats->getf() / f_old, s->getf() / s->getd())));
      s->setf(curr_stats->getf());
    }
  }
}

uint64_t Cardinality::assess(const Predicate* p){
  uint64_t la_old, ua_old, fa_old, da_old;
  uint64_t lb_old, ub_old, fb_old, db_old;;

  Stats* curr_stats1 = &stats[p->relId1][p->col1];
  Stats* curr_stats2 = &stats[p->relId2][p->col2];

  fa_old = curr_stats1->getf();
  da_old = curr_stats1->getd();

  fb_old = curr_stats2->getf();
  db_old = curr_stats2->getd();

  if(p->relId1 == p->relId2){
    if(p->col1 != p->col2){ //DIFFERENT COLUMNS
      la_old = curr_stats1->getl();
      ua_old = curr_stats1->getu();

      lb_old = curr_stats2->getl();
      ub_old = curr_stats2->getu();

      curr_stats1->setl((la_old > lb_old) ? la_old : lb_old);
      curr_stats1->setu((ua_old < ub_old) ? ua_old : ub_old);
      curr_stats1->setf(fa_old / (curr_stats1->getu() - curr_stats1->getl() + 1));
      if(da_old == 0)
        curr_stats1->setd(0);
      else
        curr_stats1->setd(da_old * (1 - pow(1 - curr_stats1->getf() / fa_old, fa_old / da_old)));

      curr_stats2->setl(curr_stats1->getl());
      curr_stats2->setu(curr_stats1->getu());
      curr_stats2->setf(curr_stats1->getf());
      curr_stats2->setd(curr_stats1->getd());

      for(uint64_t i=0; i<stats[p->relId1].size(); i++){
        if(i != p->col1 && i != p->col2){
          Stats* s = &stats[p->relId1][i];
          if(s->getd() == 0)
            s->setd(0);
          else
            s->setd(s->getd() * (1 - pow(1 - curr_stats1->getf() / fa_old, s->getf() / s->getd())));
          s->setf(curr_stats1->getf());
        }
      }
    }
    else{ // SAME COLUMN
      curr_stats1->setf(fa_old * fa_old / (curr_stats1->getu() - curr_stats1->getl() + 1));

      for(uint64_t i=0; i<stats[p->relId1].size(); i++){
        if(i != p->col1){
          Stats* s = &stats[p->relId1][i];
          s->setf(curr_stats1->getf());
        }
      }
    }
  }
  else{// DIFFERENT RELATIONS
    curr_stats1->setl((curr_stats1->getl() > curr_stats2->getl()) ? curr_stats1->getl() : curr_stats2->getl());
    curr_stats1->setu((curr_stats1->getu() < curr_stats2->getu()) ? curr_stats1->getu() : curr_stats2->getu());
    uint64_t n = curr_stats1->getu() - curr_stats1->getl() + 1;
    curr_stats1->setf(fa_old * fb_old / n);
    curr_stats1->setd(da_old * db_old / n);

    curr_stats2->setl(curr_stats1->getl());
    curr_stats2->setu(curr_stats1->getu());
    curr_stats2->setf(curr_stats1->getf());
    curr_stats2->setd(curr_stats1->getd());

    for(uint64_t i=0; i<stats[p->relId1].size(); i++){
      if(i != p->col1){
        Stats* s = &stats[p->relId1][i];
        if(s->getd() == 0)
          s->setd(0);
        else if(da_old != 0)
          s->setd(s->getd() * (1 - pow(1 - curr_stats1->getd() / da_old, s->getf() / s->getd())));
        s->setf(curr_stats1->getf());
      }
    }
    for(uint64_t i=0; i<stats[p->relId2].size(); i++){
      if(i != p->col2){
        Stats* s = &stats[p->relId2][i];
        if(s->getd() == 0)
          s->setd(0);
        else if(db_old != 0)
          s->setd(s->getd() * (1 - pow(1 - curr_stats2->getd() / db_old, s->getf() / s->getd())));
        s->setf(curr_stats2->getf());
      }
    }
  }
  return curr_stats1->getf();
}

vector<vector<Stats>> Cardinality::getStats(){
  return stats;
}
