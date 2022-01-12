/*
 * A Game Engine
 *
 * (C) 2022 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#include "spadstk.h"

#include "actor/a_obj_template.h"
#include "scene/scene.h"


void ObjTemplateActorSetup() {
}

void ObjTemplateActorInitialize(struct Actor * a, void * descriptor, void * scene) {
  ObjTemplateActor * actor = (ObjTemplateActor *)a;
  Actor_Descriptor * actor_descriptor = (Actor_Descriptor *)descriptor;
  if(descriptor) {
    Actor_PopulateBase(&actor->base, descriptor);
  }
  actor->base.Initialize = (PTRFUNC_3ARG) ObjTemplateActorInitialize;
  actor->base.Destroy = (PTRFUNC_2ARG) ObjTemplateActorDestroy;
  actor->base.Update = (PTRFUNC_2ARG) ObjTemplateActorUpdate;
  actor->base.Draw = (PTRFUNC_4ARGCHAR) ObjTemplateActorDraw;
  actor->base.visible = 1;
}

void ObjTemplateActorDestroy(struct Actor * a, void * scene) {
  ObjTemplateActor * actor = (ObjSyokudaiActor *)a;
}

void ObjTemplateActorUpdate(struct Actor * a, void * scene) {
  ObjTemplateActor * actor = (ObjSyokudaiActor *)a;
}

u_char * ObjTemplateActorDraw(struct Actor * a, MATRIX * view, u_char * packet_ptr, void * scene) {
  ObjTemplateActor * actor = (ObjTemplateActor *)a;

  SetSpadStack(SPAD_STACK_ADDR);
  
  ResetSpadStack();

  return packet_ptr;
}