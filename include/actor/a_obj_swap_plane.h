/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#ifndef A_OBJ_SWAP_PLANE_H
#define A_OBJ_SWAP_PLANE_H

#include "global.h"
#include "actor/actors.h"

typedef struct ObjSwapPlaneActor{ 
  Actor base;
  unsigned char display_type;
  unsigned char front_room;
  unsigned char back_room;
  short swap_start;
  unsigned char swap_state;
  unsigned char swap_state_prev;
  unsigned char swap_active;
} ObjSwapPlaneActor;

void ObjSwapPlaneActorInitialize(struct Actor * address, Actor_Descriptor * descriptor, void * scene);
void ObjSwapPlaneActorDestroy(struct Actor * address, void * scene);
void ObjSwapPlaneActorUpdate(struct Actor * address, void * scene);
u_char * ObjSwapPlaneActorDraw(struct Actor * address, void * view, unsigned char * buffer, void * scene);

#endif // A_OBJ_SWAP_PLANE_H