#include <iostream>
#include <cmath>
#include <algorithm>
#include "../inc/stats.hpp"
#include "../inc/cardinality.hpp"
#include "../inc/relation.hpp"

Cardinality(Stats& sA){
  this->statsA = sA;
}

Cardinality(Stats& sA, Stats& sB){
  this->statsA = sA;
  this->statsB = sB;
}

void Cardinality::assess(uint64_t k){
  this->la = k;
  this->ua = k;
  if(k <= this->statsA->getd()){
    this->da = 1;
    this->fa = this->statsA->getf() / this->statsA->getd();
  }
  else{
    this->da = 0;
    this->fa = 0;
  }

  Relation* rel = this->statsA->getRelation();
  void* col = this->statsA->getColumn();
  for(Stats* s : rel->stats){
    if(s->getColumn() != col){
      s->setd(s->getd() * (1 - pow(1 - this->fa / this->statsA->getf(), s->getf() / s->getd())));
      s->setf(this->fa);
    }
  }
}

void Cardinality::assess(char op, uint64_t k2){
  uint64_t k1;
  uint64_t l = this->statsA->getl();
  uint64_t u = this->statsA->getu();

  (op == '<') ? k1 = l;
  (op == '>') ? k1 = u;
  (k1 > k2) ? swap(k1,k2);

  this->la = k1;
  this->ua = k2;
  this->da = this->statsA->getd() * (k2 - k1) / (u - l);
  this->fa = this->statsA->getf() * (k2 - k1) / (u - l);

  Relation* rel = this->statsA->getRelation();
  void* col = this->statsA->getColumn();
  for(Stats* s : rel->stats){
    if(s->getColumn() != col){
      s->setd(s->getd() * (1 - pow(1 - this->fa / this->statsA->getf(), s->getf() / s->getd())));
      s->setf(this->fa);
    }
  }
}

void Cardinality::assess(uint64_t k1, uint64_t k2){
  uint64_t l = this->statsA->getl();
  uint64_t u = this->statsA->getu();

  (k1 < l) ? k1 = l;
  (k2 > u) ? k2 = u;

  this->la = k1;
  this->ua = k2;
  this->da = this->statsA->getd() * (k2 - k1) / (u - l);
  this->fa = this->statsA->getf() * (k2 - k1) / (u - l);

  Relation* rel = this->statsA->getRelation();
  void* col = this->statsA->getColumn();
  for(Stats* s : rel->stats){
    if(s->getColumn() != col){
      s->setd(s->getd() * (1 - pow(1 - this->fa / this->statsA->getf(), s->getf() / s->getd())));
      s->setf(this->fa);
    }
  }
}

void Cardinality::assess(1){
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
