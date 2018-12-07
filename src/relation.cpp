#include <iostream>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <string>
#include "../inc/database.hpp"
#include "../inc/relation.hpp"
#include "../inc/query.hpp"
#include "../inc/radix.h"
#include "../inc/utils.hpp"


/** -----------------------------------------------------
 * Relation constructor
 *
 */
Relation::Relation(std::string fileName): sourceFileName(fileName){
  this->loadRelation();
}

/** -----------------------------------------------------
 * Loads relation in memory from file
 *
 */
void Relation::loadRelation(){
  if(this->loaded) return;
  LOG("Loading relation %s\n", sourceFileName.c_str())
  int fd;
  struct stat sb;

  fd = open(this->sourceFileName.c_str(), O_RDONLY);
  fstat(fd, &sb);

  if((memblock = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED){
    perror("Failed to load relation from file");
    exit(2);
  }
  char * addr = (char*) memblock;

  memcpy(&this->num_tuples, addr, sizeof(uint64_t));
  addr += sizeof(uint64_t);
  memcpy(&this->num_cols, addr, sizeof(uint64_t));
  addr += sizeof(uint64_t);

  this->cols.resize(num_cols);
  for(int col = 0; col < this->num_cols; col++){
    this->cols.push_back(addr);
    addr += sizeof(uint64_t) * num_tuples;
  }

  //  ***** MIGHT BE DISCARDED ********
  addr = (char*)memblock + 2*sizeof(uint64_t);
  //  copy from memory to vector
  this->cols_.resize(num_cols);
  for(int col = 0; col < this->num_cols; col++){
    this->cols_[col].resize(num_tuples);
    for(int tpl = 0; tpl < this->num_tuples; tpl++){
      this->cols_[col][tpl].key = tpl;
      memcpy(&this->cols_[col][tpl].payload, addr, sizeof(uint64_t));
      addr += sizeof(uint64_t);
    }
  }
  if(munmap(memblock, sb.st_size) < 0){
    perror("Failed to detach mapped memory");
    exit(2);
  }
  //  ***** /MIGHT BE DISCARDED ******

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
void* Relation::getColumn(int col){
  return this->cols[col];
}

/** -----------------------------------------------------
 * Get a column count from a relation
 *
 */
uint64_t Relation::getColumnCount(){
  return this->num_cols;
}

/** -----------------------------------------------------
 * Get a tuple from a relation by column id
 *
 */
uint64_t Relation::getTuple(int col, int tpl){
  return this->getTuple(this->getColumn(col), tpl);
}

/** -----------------------------------------------------
 * Get a tuple from a relation by column pointer
 *
 */
uint64_t Relation::getTuple(void* col, int tpl){
  return *(((uint64_t*)col) + tpl);
}

/** -----------------------------------------------------
 * Get a tuple count from a relation
 *
 */
uint64_t Relation::getTupleCount(){
  return this->num_tuples;
}
