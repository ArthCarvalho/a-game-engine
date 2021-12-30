/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#ifndef A_OBJ_FLAME_H
#define A_OBJ_FLAME_H

#include "model/sgm2.h"
#include "global.h"
#include "actor/actors.h"
#include "actor/flame_common.h"

// 0 White
// 1 Orange
// 2 Yellow
// 3 Green
// 4 Blue
// 5 Purple

typedef struct ObjFlameActor {
  struct Actor base;
  FlameEffObj flame;
} ObjFlameActor;

void ObjFlameActorSetup(); // Loads and sets up data, called while loading new scenes, does not create instances

void ObjFlameActorInitialize(struct Actor * a, void * descriptor, void * scene);
void ObjFlameActorDestroy(struct Actor * a, void * scene);
void ObjFlameActorUpdate(struct Actor * a, void * scene);
u_char * ObjFlameActorDraw(struct Actor * a, MATRIX * view, u_char * pbuff, void * scene);

#endif // A_OBJ_FLAME_H