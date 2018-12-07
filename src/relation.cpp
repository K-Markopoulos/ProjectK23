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


/** -----------------------------------------------------
 * Relation constructor
 *
 */
Relation::Relation(std::string fileName): sourceFileName(fileName){}

/** -----------------------------------------------------
 * Loads relation in memory from file
 *
 */
void Relation::loadRelation(){
  if(this->loaded) return;

  char *memblock;
  int fd;
  struct stat sb;

  fd = open(this->sourceFileName.c_str(), O_RDONLY);
  fstat(fd, &sb);

  if((memblock = (char*) mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED){
    perror("Failed to load relation from file");
    exit(2);
  }
  char * addr = memblock;

  memcpy(&this->num_tuples, addr, sizeof(uint64_t));
  addr += sizeof(uint64_t);
  memcpy(&this->num_cols, addr, sizeof(uint64_t));
  addr += sizeof(uint64_t);

  this->cols.resize(num_cols);
  for(int col = 0; col < this->num_cols; col++){
    this->cols[col].resize(num_tuples);
    for(int tpl = 0; tpl < this->num_tuples; tpl++){
      this->cols[col][tpl].key = tpl;
      memcpy(&this->cols[col][tpl].payload, addr, sizeof(uint64_t));
      addr += sizeof(uint64_t);
    }
  }

  this->loaded = true;

  if(munmap(memblock, sb.st_size) < 0){
    perror("Failed to detach mapped memory");
    exit(2);
  }
  if(close(fd) < 0){
    perror("Failed close file");
    exit(2);
  }
  return;
}
