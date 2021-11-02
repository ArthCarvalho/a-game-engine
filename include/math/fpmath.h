/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#ifndef FPMATH_H
#define FPMATH_H

#include <libgte.h>

#define TO_FIXED(x) (((int)x) << 12)
#define FROM_FIXED(x) (((int)x) >> 12)

// Reduce precision after multiply operations
// (Using a different name from TO_FIXED to avoid confusion)
#define FIXED_REDUCE(x) (((int)x) >> 12);

#define FIXED(x) ((int)(x * 4096.0f)) // WARNING: This is only for immediate values, do not use actual float values here.

extern long asm_fpmul64(long a, long b);
extern long asm_fpmul64ri(long a, long b);


extern long asm_fp16mul64(long a, long b); // 16.16 version of asm_fpmul64

extern long asm_dotProductVSXZ(VECTOR * a, SVECTOR * b);
extern long asm_dotProductVLXZ(VECTOR * a, VECTOR * b);



#endif
