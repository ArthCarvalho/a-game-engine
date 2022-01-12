/*
 * A Game Engine
 *
 * (C) 2022 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#ifndef A_OBJ_COLLECTABLE_H
#define A_OBJ_COLLECTABLE_H

#include "model/agm.h"
#include "model/sgm2.h"
#include "global.h"
#include "actor/actors.h"

typedef struct ObjCollectableActor {
  struct Actor base;
  u_short item_type;
  short roty;
  short y_offset;
  u_char state;
  u_char counter;
} ObjCollectableActor;

void ObjCollectableActorSetup(); // Loads and sets up data, called while loading new scenes, does not create instances

void ObjCollectableActorInitialize(struct Actor * a, void * descriptor, void * scene);
void ObjCollectableActorDestroy(struct Actor * a, void * scene);
void ObjCollectableActorUpdate(struct Actor * a, void * scene);
u_char * ObjCollectableActorDraw(struct Actor * a, MATRIX * view, u_char * pbuff, void * scene);



#endif // A_OBJ_COLLECTABLE_H