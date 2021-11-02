/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#ifndef A_OBJ_GRASS_CUT_H
#define A_OBJ_GRASS_CUT_H

#include "model/sgm2.h"
#include "global.h"
#include "actor/actors.h"

extern unsigned long obj_grass_cut_tim[];
extern unsigned long obj_grass_cut_full_sgm2[];
extern unsigned long obj_grass_cut_half_sgm2[];

extern struct SGM2 * obj_grass_cut_full_model;
extern struct SGM2 * obj_grass_cut_half_model;

typedef struct ObjGrassCutActor {
  struct Actor base;
  struct SGM2 * model; // tubo model
  struct SGM2 * model_full; // tubo far model
  struct SGM2 * model_cut; // tubo far model
  VECTOR scale;
  MATRIX matrix;
  u_int flags;
  u_short cut_timer;
} ObjGrassCutActor;

void ObjGrassCutActorSetup(); // Loads and sets up data, called while loading new scenes, does not create instances

void ObjGrassCutActorInitialize(struct Actor * a, void * descriptor, void * scene);
void ObjGrassCutActorDestroy(struct Actor * a, void * scene);
void ObjGrassCutActorUpdate(struct Actor * a, void * scene);
u_char * ObjGrassCutActorDraw(struct Actor * a, MATRIX * view, u_char * pbuff, void * scene);

#endif // A_OBJ_GRASS_CUT_H