/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#ifndef SCREEN_H
#define SCREEN_H

#include "global.h"

#define LIFEMETER_TEX_X 512-4
#define LIFEMETER_TEX_Y 0
#define LIFEMETER_VRAM_X 240
#define LIFEMETER_VRAM_Y 0
#define LIFEMETER_VRAM_A_Y 32
#define LIFEMETER_CLUT_X 512
#define LIFEMETER_CLUT_Y 241

#define LIFEMETER_HEARTS_LINE 10

#define LIFEMETER_POS_X 24
#define LIFEMETER_POS_Y 16
// Life Meter Heart 1/4 piece tile
#define LIFEMETER_W 8
#define LIFEMETER_H 4

// Life Meter
void Screen_LifeMeterInit();

char * Screen_LifeMeterDraw(char * buff, char level);

char * Screen_RenderHeart(char * buff, short x, short y, char state, u_char pct, char alpha);

char * Screen_MagicBarDraw(char * buff, short x, short y, short w, short magic, char alpha);

#endif // SCREEN_H