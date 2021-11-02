/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

// Linear Stack Allocator
// Only allocates ahead, can't free, only way is to reset the whole stack.
// used for static data that will be freed between maps
unsigned char * _linear_start_ptr; // start of the linear heap
unsigned char * _linear_end_ptr; // end address of the linear heap
unsigned char * _linear_tail; // address of the first unallocated memory
unsigned char * _linear_prev_tail; // Restores the stack to a previous state
unsigned long _linear_size; // total heap size

void LStack_InitArea(void * start, unsigned long size) {
  _linear_start_ptr = (unsigned char * )(((unsigned long)start >> 2) + 1 << 2);
  _linear_end_ptr = (unsigned char * )((unsigned long)_linear_start_ptr + ((size >> 2) + 1 << 2));
  _linear_tail = _linear_start_ptr;
  _linear_size = size;
}

void * LStack_GetTail() {
  return _linear_tail;
}

void * LStack_Alloc(unsigned long size) {
  void * current = _linear_tail;
  _linear_tail = (unsigned char * )((unsigned long)_linear_tail + ((size >> 2) + 1 << 2));
  return current;
}

void LStack_FreeAll() {
  _linear_tail = _linear_start_ptr;
  _linear_prev_tail = _linear_start_ptr;
}

unsigned long LStack_GetFree() {
  return (unsigned long)_linear_end_ptr - (unsigned long)_linear_tail;
}

void LStack_SaveCurrentPosition() {
  _linear_prev_tail = _linear_tail;
}

void LStack_RestoreLastPosition() {
  _linear_tail = _linear_prev_tail;
}