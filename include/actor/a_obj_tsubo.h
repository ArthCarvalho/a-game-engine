/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#ifndef A_OBJ_TSUBO_H
#define A_OBJ_TSUBO_H

#include "model/sgm2.h"
#include "global.h"
#include "actor/actors.h"

extern unsigned long obj_tsubo_tim[];
extern unsigned long obj_tsubo_sgm2[];
extern unsigned long obj_tsubo_low_sgm2[];

extern struct SGM2 * obj_tsubo_model;
extern struct SGM2 * obj_tsubo_far_model;

typedef struct ObjTsuboActor {
  struct Actor base;
  struct SGM2 * model; // tubo model
  struct SGM2 * model_far; // tubo far model
  MATRIX matrix;
  u_int flags;
  u_int dist;
} ObjTsuboActor;

void ObjTsuboActorSetup(); // Loads and sets up data, called while loading new scenes, does not create instances

void ObjTsuboActorInitialize(struct Actor * a, void * descriptor, void * scene);
void ObjTsuboActorDestroy(struct Actor * a, void * scene);
void ObjTsuboActorUpdate(struct Actor * a, void * scene);
u_char * ObjTsuboActorDraw(struct Actor * a, MATRIX * view, u_char * pbuff, void * scene);

#endif // A_OBJ_TSUBO_H