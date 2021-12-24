/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#ifndef A_OBJ_DOOR_SHUTTER_H
#define A_OBJ_DOOR_SHUTTER_H

#include "model/sgm2.h"
#include "global.h"
#include "actor/actors.h"

extern unsigned long obj_dangeon_door_tim[];

// Door Height: 663

extern SVECTOR door_shutter_vectors[];

typedef struct ObjDoorShutterActor {
  struct Actor base;
  MATRIX matrix;
  u_int flags;
  u_int dist;
  u_char anim_timer;
  u_char sub_timer;
  u_short flip_model;
  u_char front_room;
  u_char back_room;
} ObjDoorShutterActor;

void ObjDoorShutterActorSetup(); // Loads and sets up data, called while loading new scenes, does not create instances

void ObjDoorShutterActorInitialize(struct Actor * a, void * descriptor, void * scene);
void ObjDoorShutterActorDestroy(struct Actor * a, void * scene);
void ObjDoorShutterActorUpdate(struct Actor * a, void * scene);
u_char * ObjDoorShutterActorDraw(struct Actor * a, MATRIX * view, u_char * pbuff, void * scene);

#endif // A_OBJ_DOOR_SHUTTER_H