/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#include "memory/pool.h"

#define POOL_ALLOCATED 1
#define POOL_SIZE_MASK -2

unsigned long * _pool_start_ptr; // start of the pool heap
unsigned long * _pool_end_ptr;

typedef struct POOL_ALLOC {
  unsigned long size;
} POOL_ALLOC;

void Pool_Init(void * start, unsigned long size) {
  _pool_start_ptr = (unsigned long * )(((unsigned long)start >> 2) + 1 << 2);
  _pool_end_ptr = (unsigned long * )((unsigned long)_pool_start_ptr + ((size >> 2) + 1 << 2));
  *_pool_start_ptr = (size-4) & POOL_SIZE_MASK;
  *_pool_end_ptr = *_pool_start_ptr;
}

void Pool_Release() {
  _pool_start_ptr = 0;
  _pool_end_ptr = 0;
}

void * Pool_Malloc(unsigned long size) {

}

void Pool_Free(void * ptr) {

}