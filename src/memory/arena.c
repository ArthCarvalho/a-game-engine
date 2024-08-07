/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

// Block - First Fit Allocator
// Only allocates ahead, can't free, only way is to reset the whole stack.
// used for static data that will be freed between maps
unsigned long * _arena_start_ptr; // start of the linear heap
unsigned long * _arena_end_ptr; // end address of the linear heap

#define ARENA_ALLOCATED_BIT 1
#define ARENA_SIZE_MASK -2

void Arena_Init(void * start, unsigned long size) {
  unsigned long size4 = (((size >> 2) + 1) << 2);
  _arena_start_ptr = (unsigned long *)(((((unsigned long)start) >> 2) + 1) << 2);
  _arena_end_ptr = (unsigned long *)((((unsigned long)_arena_start_ptr) + size4));
  for(long * itr = _arena_start_ptr; itr < _arena_end_ptr; itr++) {
    *itr = 0x0;
  }
  *_arena_start_ptr = *_arena_end_ptr = size4-4;
  //printf("Arena Start: 0x%08X Arena End:0x%08X\n", _arena_start_ptr, _arena_end_ptr);
}


/*void Arena_Init(void * start, unsigned long size) {
  _arena_start_ptr = (unsigned long *)(((unsigned long)start >> 2) + 1 << 2);
  _arena_end_ptr = (unsigned long *)((unsigned long)_arena_start_ptr + ((size >> 2) + 1 << 2));
  *_arena_start_ptr = ((size)-4) & ARENA_SIZE_MASK;
  *_arena_end_ptr = *_arena_start_ptr;
  for(long * itr = _arena_start_ptr; itr <= _arena_end_ptr; itr++) {
    *itr = 0x0;
  }
}

void Arena_Release() {
  _arena_start_ptr = 0;
  _arena_end_ptr = 0;
}*/


void * Arena_Malloc(unsigned long size) {
  // First Fit, start from beginning
  unsigned long * itr = _arena_start_ptr;
  unsigned long * result;
  unsigned long sz = ((size + 3) >> 2);
  unsigned long sz4 = sz << 2;
  unsigned long old_sz;
  while((itr < _arena_end_ptr)){
    ////printf("looking at %x\n",itr);
    if((*itr & ARENA_ALLOCATED_BIT) || (*itr < sz4 && *itr != 0)){
      ////printf("allocated block found at %x, size: %d bit: %x goint to next block\n",itr, *itr & ARENA_SIZE_MASK, *itr & ARENA_ALLOCATED_BIT);
      ////printf("allocated, conditions: alloc=%d size=%d\n",(*itr & ARENA_ALLOCATED_BIT),(*itr < sz4 && *itr != 0));
      itr = itr + (*itr >> 2) + 2;
      continue;
    }
    break;
  }
  ////printf("Pointer found at 0x%08X / 0x%08X\n", itr, _arena_end_ptr);
  // Get block current size
  old_sz = *itr & ARENA_SIZE_MASK;
  *itr = (sz4 & ARENA_SIZE_MASK | ARENA_ALLOCATED_BIT);
  result = itr;
  itr = (itr+sz+1);
  *itr = *result;
  if(old_sz > (sz4+8)){ // if old size is greater than current
    unsigned long splitsize = ((old_sz - (sz4+8)) & ARENA_SIZE_MASK);
    itr++;
    //*(itr+1) = ((old_sz - (sz4+8)) & ARENA_SIZE_MASK);
    *itr = splitsize;
    itr += (*itr >> 2) + 1;
    *itr = splitsize;

  }
  return result+1;
}

// Free and coalesce blocks.
void Arena_Free(void * ptr) {
  unsigned long * itr = ((unsigned long*)ptr)-1;
  unsigned long * start_ptr = itr;
  unsigned long * end_ptr;
  unsigned long size; 
  ////printf("Free: 0x%08X\n", ptr);
  ////printf("Free: 0x%08X (itr) = 0x%08X\n", itr, *itr);
  if(!(*itr & ARENA_ALLOCATED_BIT)) return; // Already freed/invalid
  size = *itr & ARENA_SIZE_MASK;
  ////printf("Allocated size being free'd: 0x%08X (itr value: 0x%08X)\n", size, *itr);
  *itr &= ARENA_SIZE_MASK; // Clear out the allocation flag

  itr = itr + (size >> 2) + 1;
  *itr = *itr & ARENA_SIZE_MASK;
  ////printf("Now itr(0x%08X) = %08X\n", itr, *itr);
  end_ptr = itr;
  // Look ahead
  /*itr++;
  //printf("Neighbor block itr(0x%08X) = %08X Alloc? %d\n", itr, *itr,  *itr & ARENA_ALLOCATED_BIT);
  if((itr < _arena_end_ptr) && (!(*itr & ARENA_ALLOCATED_BIT))) {
    //printf("Coalesce ahead s=0x%08X\n", *itr);
    unsigned long new_size = (*start_ptr + *itr) + 2; // Block size.
    *start_ptr = new_size; // Copy new block size to start
    itr = itr + (*itr >> 2) + 1;
    *itr = new_size;
    end_ptr = itr;
  }*/
  //end_ptr = itr++;
  // coalesce one ahead
  /*if((!(*itr & ARENA_ALLOCATED_BIT)) && (itr < _arena_end_ptr)) { // next block is allocated
    //printf("Coalesce next: 0x%08X\n", *itr);
    unsigned long next_size = *itr & ARENA_SIZE_MASK;
    end_ptr = itr + (next_size>>2) + 2;
    itr = ((unsigned long*)ptr) - 1;
    size = ((size + next_size) + 8);
    *itr = size;
    *end_ptr = *itr; // copy header to footer
  }
  // coalesce one behind
  itr = ((unsigned long*)ptr)-2; // set pointer to footer of previous block
  if((itr > _arena_start_ptr) && (!(*itr & ARENA_ALLOCATED_BIT))) { // previous block is allocated
    //printf("Coalesce prev: 0x%08X\n", *itr);
    unsigned long prev_size = *itr & ARENA_SIZE_MASK;
    itr = itr - (prev_size>>2) - 1;
    prev_size = size + prev_size + 8;
    *itr = prev_size & ARENA_SIZE_MASK;
    size = prev_size;
  }
  *end_ptr = size;*/
}