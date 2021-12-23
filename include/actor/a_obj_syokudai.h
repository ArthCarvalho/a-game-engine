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

extern unsigned long obj_syokudai_tim[];
extern unsigned long obj_syokudai_sgm2[];

extern unsigned long obj_flame_tim[];
extern unsigned long obj_flame_pal_tim[];
extern unsigned long obj_flame_sgm2[];
extern unsigned long obj_flame_high_sgm2[];

extern struct SGM2 * obj_syokudai_model;
extern struct SGM2 * obj_flame_model;
extern struct SGM2 * obj_flame_high_model;

// 0 White
// 1 Orange
// 2 Yellow
// 3 Green
// 4 Blue
// 5 Purple

#define SYOKUDAI_FLAME_HEIGHT 496

typedef struct ObjSyokudaiActor {
  struct Actor base;
  u_char flame_color; // select flame color
  u_char flame_rand; // Randomize flame animation per object
  u_char flicker_frame;
  short flame_timer; // -1 <= Always on, 0 = off, 1 >= countdown
  struct SGM2 * body; // syokudai model
  struct SGM2 * flame; // flame model
  struct SGM2 * flame_high; // High Poly LOD
  MATRIX body_matrix; // Position matrix, calculated at init.
  MATRIX flame_matrix; // Updated to always face the camera
} ObjSyokudaiActor;

void ObjSyokudaiActorSetup(); // Loads and sets up data, called while loading new scenes, does not create instances

void ObjSyokudaiActorInitialize(struct Actor * a, void * descriptor, void * scene);
void ObjSyokudaiActorDestroy(struct Actor * a, void * scene);
void ObjSyokudaiActorUpdate(struct Actor * a, void * scene);
u_char * ObjSyokudaiActorDraw(struct Actor * a, MATRIX * view, u_char * pbuff, void * scene);
u_char * ObjSyokudaiActorDrawFlame(struct Actor * a, MATRIX * view, u_char * packet_ptr, void * scene);

#endif // A_OBJ_SYOKUDAI_H