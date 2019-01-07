#include <iostream>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <string>
#include <assert.h>
#include "../inc/database.hpp"
#include "../inc/relation.hpp"
#include "../inc/query.hpp"
#include "../inc/radix.h"
#include "../inc/utils.hpp"
#include "../inc/stats.hpp"

/** -----------------------------------------------------
 * Relation constructor
 *
 */
Relation::Relation(std::string fileName): sourceFileName(fileName){
  static int id = 0;
  this->_id = id++;
  LOG("Relation ID: %d\n", this->_id);
  this->loadRelation();
}

/** -----------------------------------------------------
 * Relation destructor
 *
 */
Relation::~Relation(){
  LOG("Destructing relation %d\n", _id);
  // WARNING: Copies of Relation remove the memory

  if(this->loaded && munmap(this->memblock, this->memsize) < 0){
    perror("Failed to detach mapped memory");
    exit(2);
  }
}

/** -----------------------------------------------------
 * Loads relation in memory from file
 *
 */
void Relation::loadRelation(){
  if(this->loaded) return;
  LOG("Loading relation %s\n", sourceFileName.c_str());
  int fd;
  struct stat sb;

  fd = open(this->sourceFileName.c_str(), O_RDONLY);
  fstat(fd, &sb);
  this->memsize = sb.st_size;

  if((this->memblock = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED){
    perror("Failed to load relation from file");
    exit(2);
  }
  char * addr = (char*) this->memblock;

  memcpy(&this->num_tuples, addr, sizeof(uint64_t));
  addr += sizeof(uint64_t);
  memcpy(&this->num_cols, addr, sizeof(uint64_t));
  addr += sizeof(uint64_t);
  LOG("\t Cols:%lu Tuples:%lu\n", this->num_cols, this->num_tuples);

  this->cols.reserve(num_cols);
  //STATS
  this->stats.reserve(num_cols);

  for(int col = 0; col < this->num_cols; col++){
    LOG("\tAdding col starting with %lu\n", *(uint64_t*)addr);
    this->cols.push_back((void*)addr);
    //STATS
    this->stats.push_back(Stats(this, col));
    addr += sizeof(uint64_t) * num_tuples;
  }

  this->loaded = true;

  if(close(fd) < 0){
    perror("Failed close file");
    exit(2);
  }
  return;
}

/** -----------------------------------------------------
 * Get a column pointer from a relation
 *
 */
uint64_t Relation::getId() const{
  return this->_id;
}

/** -----------------------------------------------------
 * Get a column pointer from a relation
 *
 */
void* Relation::getColumn(int col) const{
  assert(col < this->getColumnCount());
  return this->cols[col];
}

/** -----------------------------------------------------
 * Get a column count from a relation
 *
 */
uint64_t Relation::getColumnCount() const{
  return this->num_cols;
}

/** -----------------------------------------------------
 * Get a tuple from a relation by column id
 *
 */
uint64_t Relation::getTuple(int col, int tpl) const{
  assert(col < this->num_cols);
  assert(tpl < this->num_tuples);
  return this->getTuple(this->getColumn(col), tpl);
}

/** -----------------------------------------------------
 * Get a tuple from a relation by column pointer
 *
 */
uint64_t Relation::getTuple(const void* col, int tpl) const{
  return *((uint64_t*)col + tpl);
  // return *((uint64_t*)((char*)col + tpl*sizeof(uint64_t)));
}

/** -----------------------------------------------------
 * Get a tuple count from a relation
 *
 */
uint64_t Relation::getTupleCount() const{
  return this->num_tuples;
}

/** -----------------------------------------------------
 * Build relation of [rowId,value] tuples
 *
 * @params id, id of relation
 * @returns relation*, (allocated) to be used in radixHashJoin
 */
relation* Relation::buildRelation(int col){
  assert(loaded);

  relation* res = (relation*) malloc(sizeof(relation));
  res->tuples = (tuple_*) malloc(num_tuples*sizeof(tuple_));
  res->num_tuples = num_tuples;
  for(int t = 0; t < num_tuples; t++){
    res->tuples[t].key = t;
    res->tuples[t].payload = this->getTuple(col, t);
  }
  return res;
}
