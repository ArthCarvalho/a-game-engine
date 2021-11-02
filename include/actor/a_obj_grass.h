/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#ifndef A_OBJ_GRASS_H
#define A_OBJ_GRASS_H

#include "model/sgm2.h"
#include "global.h"
#include "actor/actors.h"

extern unsigned long obj_grass_tim[];
extern unsigned long obj_grass_sgm2[];
extern unsigned long obj_grass_low_sgm2[];
extern unsigned long obj_grass_low2_sgm2[];

extern struct SGM2 * obj_grass_model;
extern struct SGM2 * obj_grass_far_model;
extern struct SGM2 * obj_grass_far2_model;

typedef struct ObjGrassActor {
  struct Actor base;
  struct SGM2 * model; // tubo model
  struct SGM2 * model_far; // tubo far model
  struct SGM2 * model_far2; // tubo far model
  MATRIX matrix;
  u_int flags;
  u_int dist;
} ObjGrassActor;

void ObjGrassActorSetup(); // Loads and sets up data, called while loading new scenes, does not create instances

void ObjGrassActorInitialize(struct Actor * a, void * descriptor, void * scene);
void ObjGrassActorDestroy(struct Actor * a, void * scene);
void ObjGrassActorUpdate(struct Actor * a, void * scene);
u_char * ObjGrassActorDraw(struct Actor * a, MATRIX * view, u_char * pbuff, void * scene);

#endif // A_OBJ_GRASS_H