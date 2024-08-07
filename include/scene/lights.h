/*
 * A Game Engine
 *
 * (C) 2022 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#ifndef _LIGHTS_H_
#define _LIGHTS_H_

#include "global.h"

#define LIGHTS_MAX 16

#define LIGHT_ALLOCATED 0x8000
#define LIGHT_TYPE_DIRECTIONAL 0x4000

typedef struct Light {
  SVECTOR pos;
  CVECTOR color;
  long distanceSq;
  u_short distance;
  u_short flags;
} Light;


extern Light Lights_List[];
extern u_char Lights_Count;
extern Light Lights_Directional[];

Light * Lights_Create(SVECTOR * position, CVECTOR * color, u_short distance, u_short flags, void * scene);

void Lights_Destroy(Light * light);

void Lights_CalcNearest(Actor * a, void * scene);

void Lights_Initialize();

#endif // _LIGHTS_H_