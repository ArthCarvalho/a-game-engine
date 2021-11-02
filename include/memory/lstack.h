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
extern unsigned char * _linear_start_ptr; // start of the linear heap
extern unsigned char * _linear_end_ptr; // end address of the linear heap
extern unsigned char * _linear_tail; // address of the first unallocated memory
extern unsigned long _linear_size; // total heap size

extern void LStack_InitArea(void * start, unsigned long size);

extern void * LStack_GetTail();

extern void * LStack_Alloc(unsigned long size);

extern void LStack_FreeAll(); // Resets the stack

extern unsigned long LStack_GetFree();

void LStack_SaveCurrentPosition(); // Save current stack pointer

void LStack_RestoreLastPosition(); // Resets the stack back to a previous saved position