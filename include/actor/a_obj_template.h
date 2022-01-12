/*
 * A Game Engine
 *
 * (C) 2022 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#ifndef A_OBJ_TEMPLATE_H
#define A_OBJ_TEMPLATE_H

#include "model/agm.h"
#include "model/sgm2.h"
#include "global.h"
#include "actor/actors.h"

typedef struct ObjTemplateActor {
  struct Actor base;
} ObjTemplateActor;

void ObjTemplateActorSetup(); // Loads and sets up data, called while loading new scenes, does not create instances

void ObjTemplateActorInitialize(struct Actor * a, void * descriptor, void * scene);
void ObjTemplateActorDestroy(struct Actor * a, void * scene);
void ObjTemplateActorUpdate(struct Actor * a, void * scene);
u_char * ObjTemplateActorDraw(struct Actor * a, MATRIX * view, u_char * pbuff, void * scene);

#endif // A_OBJ_TEMPLATE_H