/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#ifndef FLAME_COMMON_H
#define FLAME_COMMON_H

#include "model/sgm2.h"
#include "global.h"
#include "actor/actors.h"

extern unsigned long obj_flame_tim[];
extern unsigned long obj_flame_pal_tim[];
extern unsigned long obj_flame_sgm2[];
extern unsigned long obj_flame_high_sgm2[];

extern struct SGM2 * obj_flame_model;
extern struct SGM2 * obj_flame_high_model;

// Flame Color IDs
// 0 White
// 1 Orange
// 2 Yellow
// 3 Green
// 4 Blue
// 5 Purple

extern SVECTOR flare_verts[];
extern u_char flare_index_outter_ring[];
extern u_char flare_index_inner[];
extern u_char flicker_frame[];

extern CVECTOR flame_colors[];

// Screen Flare, 8 segments
#define SCREENFLARE_SEGMENTS 8
#define SCREENFLARE_SEGMENTS2 (SCREENFLARE_SEGMENTS * 2) + 1
#define SCREENFLARE_SEGMENTS_INNER SCREENFLARE_SEGMENTS * 3
#define SCREENFLARE_SEGMENTS_OUTTER SCREENFLARE_SEGMENTS * 4
#define SCREENFLARE_SEGMENT_ANG 4096 / SCREENFLARE_SEGMENTS
#define SCREENFLARE_INNER_RING_SCALE 2458
#define SCREENFLARE_FLICKER_FRAMES 90
#define SCREENFLARE_SIZE 19
#define SCREENFLARE_ENABLE_DITHERING 1

u_char * Draw_ScreenFlare(SVECTOR * pos, MATRIX * view, u_char * packet_ptr, void * scene, u_char flicker, u_short scale, u_char color);

void Draw_SetupFlame();

void Draw_CreateFlame(FlameEffObj * flame, SVECTOR * pos, SVECTOR * scale, u_char color, u_char show_flare);

u_char * Draw_Flame(FlameEffObj * flame, MATRIX * view, u_char * packet_ptr, void * scene);

#endif // FLAME_COMMON_H