/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#ifndef INPUT_H
#define INPUT_H

#include "global.h"

#define PAD_L3      0x00000200
#define PAD_R3      0x00000400
#define PAD_L2			0x00000001
#define PAD_R2			0x00000002
#define PAD_L1			0x00000004
#define PAD_R1			0x00000008
#define PAD_UP			0x00001000
#define PAD_RIGHT		0x00002000
#define PAD_DOWN		0x00004000
#define PAD_LEFT		0x00008000
#define PAD_SEL			0x00000100
#define PAD_SELECT		PAD_SEL
#define PAD_START		  0x00000800
#define PAD_TRIANGLE	0x00000010
#define PAD_CIRCLE		0x00000020
#define PAD_CROSS		  0x00000040
#define PAD_SQUARE		0x00000080

#define NO_PADS			0
#define PAD_ONE			1
#define PAD_TWO			2
#define BOTH_PADS		3
#define IS_DUAL_SHOCK	0x80

#define PAD_A 0
#define PAD_B 1

#define PAD_STATE_CHECK		0
#define PAD_STATE_EQUAL		1
#define PAD_STATE_STABLE	2

#define ANALOG_L_X 0
#define ANALOG_L_Y 1
#define ANALOG_R_X 3
#define ANALOG_R_Y 2
 
void InputInit();
void InputUpdate(void);

#endif

