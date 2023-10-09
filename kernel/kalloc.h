#ifndef _KALLOC_H_
#define _KALLOC_H_

#include "types.h"
#include "spinlock.h"

struct run {
  struct run *next;
};

struct kmem_struct {
  struct spinlock lock;
  struct run *freelist;
  uint64 n_free_pages;
  void *start;
};

extern struct kmem_struct kmem;

#endif
