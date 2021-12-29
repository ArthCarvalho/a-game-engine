/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#ifndef TEXTURE_H
#define TEXTURE_H

#include "global.h"

typedef struct {
	unsigned long pmode;
	short   px, py;
	unsigned short pw, ph;
	unsigned long *pixel;
	short   cx, cy;
	unsigned short cw, ch;
	unsigned long *clut;
} TIMIMAGE;

void GetTimInfo(unsigned long *im, TIMIMAGE * tim);

void load_texture(u_long addr);
void load_texture_pos(u_long addr, u_long x, u_long y, u_long c_x, u_long c_y);
void load_texture_pos_fog(u_long addr, u_long x, u_long y, u_long c_x, u_long c_y, u_short fog);
void load_tex_noclut_pos(u_long addr, u_long x, u_long y, u_long c_x, u_long c_y);
void load_clut_pos(u_long addr, u_long c_x, u_long c_y, u_char c_w, u_char c_h);

#endif