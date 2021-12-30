/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#ifndef A_OBJ_SYOKUDAI_H
#define A_OBJ_SYOKUDAI_H

#include "model/sgm2.h"
#include "global.h"
#include "actor/actors.h"
#include "actor/flame_common.h"

extern unsigned long obj_syokudai_tim[];
extern unsigned long obj_syokudai_sgm2[];

extern struct SGM2 * obj_syokudai_model;

#define SYOKUDAI_FLAME_HEIGHT 453

typedef struct ObjSyokudaiActor {
  struct Actor base;
  short flame_timer; // -1 <= Always on, 0 = off, 1 >= countdown
  struct SGM2 * body; // syokudai model
  MATRIX body_matrix; // Position matrix, calculated at init.
  FlameEffObj flame; // Flame Data
} ObjSyokudaiActor;

void ObjSyokudaiActorSetup(); // Loads and sets up data, called while loading new scenes, does not create instances

void ObjSyokudaiActorInitialize(struct Actor * a, void * descriptor, void * scene);
void ObjSyokudaiActorDestroy(struct Actor * a, void * scene);
void ObjSyokudaiActorUpdate(struct Actor * a, void * scene);
u_char * ObjSyokudaiActorDraw(struct Actor * a, MATRIX * view, u_char * pbuff, void * scene);

#endif // A_OBJ_SYOKUDAI_H