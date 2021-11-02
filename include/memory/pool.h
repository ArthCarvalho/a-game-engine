/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#ifndef POOL_H
#define POOL_H

extern unsigned long * _pool_start_ptr; // start of the linear heap
extern unsigned long * _pool_end_ptr;

void Pool_Init(void * start, unsigned long size);

void Pool_Release();

void * Pool_Malloc(unsigned long size);

void Pool_Free(void * ptr);

#endif // POOL_H