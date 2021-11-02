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
extern unsigned long * _arena_start_ptr; // start of the linear heap
extern unsigned long * _arena_end_ptr;

void Arena_Init(void * start, unsigned long size);

void Arena_Release();

void * Arena_Malloc(unsigned long size);

void Arena_Free(void * ptr);