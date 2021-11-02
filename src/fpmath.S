/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

/*.set push
.set noat */
.set noreorder


#include "mipsregs.h"

	.text

	.global	asm_fpmul64
	.global	asm_fpmul64ri
  .global asm_fp16mul64
  .global asm_dotProductVSXZ

/* long asm_fpmul64(long a, long b) */
.type asm_fpmul64, @function
.ent asm_fpmul64
asm_fpmul64:
  mult a0, a1
  mfhi t0
  sll t0, t0, 20
  mflo t1
  srl t1, t1, 12
  j ra
  or v0, t0, t1
.end asm_fpmul64
  
/* long asm_fpmul64ri(long a, long b) */
.type asm_fpmul64ri, @function
.ent asm_fpmul64ri
asm_fpmul64ri:
  mult a0, a1
  mfhi t0
  sll t0, t0, 8
  mflo t1
  srl t1, t1, 24
  j ra
  or v0, t0, t1
.end asm_fpmul64ri

/* long asm_fp16mul64(long a, long b) */
.type asm_fp16mul64, @function
.ent asm_fp16mul64
asm_fp16mul64:
  mult a0, a1
  mfhi t0
  sll t0, t0, 16
  mflo t1
  srl t1, t1, 16
  j ra
  or v0, t0, t1
.end asm_fp16mul64

/* long asm_dotProductVSXZ(VECTOR * a, SVECTOR * b) */
.type asm_dotProductVSXZ, @function
.ent asm_dotProductVSXZ
asm_dotProductVSXZ:
  lh t2, 0(a1) /* Load b->vx (short) */
  lw t0, 0(a0) /* Load a->vx (long) */
  lw t1, 8(a0) /* Load a->vz (long) */
  mult t0, t2
  lh t3, 4(a1) /* Load b->vz (short) */
  mfhi t0
  mflo t2
  nop
  nop
  mult t1, t3
  mfhi t1
  mflo t3
  addu t3, t1, t3
  sltu t1, t3, t1
  addu t0, t0, t2
  addu t1, t1, t0
  sll t1, t1, 20
  srl t3, t3, 12
  j ra
  or v0, t1, t3
.end asm_dotProductVSXZ

.type asm_dotProductVLXZ, @function
.ent asm_dotProductVLXZ
asm_dotProductVLXZ:
  lw t2, 0(a1) /* Load b->vx (long) */
  lw t0, 0(a0) /* Load a->vx (long) */
  lw t1, 8(a0) /* Load a->vz (long) */
  mult t0, t2
  lw t3, 4(a1) /* Load b->vz (long) */
  mfhi t0
  mflo t2
  nop
  nop
  mult t1, t3
  mfhi t1
  mflo t3
  addu t3, t1, t3
  sltu t1, t3, t1
  addu t0, t0, t2
  addu t1, t1, t0
  sll t1, t1, 20
  srl t3, t3, 12
  j ra
  or v0, t1, t3
.end asm_dotProductVLXZ
