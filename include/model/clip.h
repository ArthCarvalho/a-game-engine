/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

// From PSn00bSDK
#ifndef _CLIP_H
#define _CLIP_H

#include "global.h"

extern RECT local_clip;

/* tri_clip
 *
 * Returns non-zero if a triangle (v0, v1, v2) is outside 'clip'.
 *
 * clip			- Clipping area
 * v0,v1,v2		- Triangle coordinates
 *
 */
unsigned int tri_clip(DVECTOR *v0, DVECTOR *v1, DVECTOR *v2);

/* quad_clip
 *
 * Returns non-zero if a quad (v0, v1, v2, v3) is outside 'clip'.
 *
 * clip			- Clipping area
 * v0,v1,v2,v3	- Quad coordinates
 *
 */
unsigned int quad_clip(DVECTOR *v0, DVECTOR *v1, DVECTOR *v2, DVECTOR *v3);

typedef struct VEC2D {
  long vx, vy;
} VECTOR2D;

int tri_clip_s(RECT32 * rect, VECTOR2D *v0, VECTOR2D *v1, VECTOR2D *v2);

#endif // _CLIP_H