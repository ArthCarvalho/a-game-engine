/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#ifndef A_OBJ_DEKUNUTS_H
#define A_OBJ_DEKUNUTS_H

#include "model/agm.h"
#include "model/sgm2.h"
#include "global.h"
#include "actor/actors.h"

extern unsigned long obj_dekunuts_plant_tim[];
extern unsigned long obj_dekunuts_body_tim[];
extern unsigned long obj_dekunuts_plant_sgm2[];
extern unsigned long dekunuts_body_static_sgm2[];
extern unsigned long dekunuts_agm[];
extern unsigned long dekunuts_anm[];

extern struct AGM_Model obj_dekunuts_model;
extern struct ANM_Animation * obj_dekunuts_anim;

extern struct SGM2 * obj_dekunuts_plant_model;
extern struct SGM2 * obj_dekunuts_body_model;

#define OBJ_DEKUNUTS_PLANT_TEXTURE_X 64
#define OBJ_DEKUNUTS_PLANT_TEXTURE_Y 256
#define OBJ_DEKUNUTS_BODY_TEXTURE_X 64
#define OBJ_DEKUNUTS_BODY_TEXTURE_Y 320

#define OBJ_DEKUNUTS_PLANT_CLUT_X 64
#define OBJ_DEKUNUTS_PLANT_CLUT_Y 512-2
#define OBJ_DEKUNUTS_BODY_CLUT_X 64
#define OBJ_DEKUNUTS_BODY_CLUT_Y 512-1

typedef struct ObjDekunutsActor {
  struct Actor base;
  struct AGM_Model * model;
  struct SGM2 * body_model;
  struct SGM2 * plant_model;
  MATRIX matrix;
  MATRIX plant_matrix;
  u_short anim;
  short burrow_offset;
  short burrow_offset_targ;
  u_short material_flags[4];
} ObjDekunutsActor;

void ObjDekunutsActorSetup(struct Actor * a, void * scene); // Loads and sets up data, called while loading new scenes, does not create instances

void ObjDekunutsActorInitialize(struct Actor * a, void * descriptor, void * scene);
void ObjDekunutsActorDestroy(struct Actor * a, void * scene);
void ObjDekunutsActorUpdate(struct Actor * a, void * scene);
u_char * ObjDekunutsActorDraw(struct Actor * a, MATRIX * view, u_char * pbuff, void * scene);

#endif // A_OBJ_DEKUNUTS_H