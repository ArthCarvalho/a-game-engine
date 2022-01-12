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

#include "actor/a_obj_collectable.h"
#include "scene/scene.h"


void ObjCollectableActorSetup() {
}

void ObjCollectableActorInitialize(struct Actor * a, void * descriptor, void * scene) {
  ObjCollectableActor * actor = (ObjCollectableActor *)a;
  Actor_Descriptor * actor_descriptor = (Actor_Descriptor *)descriptor;
  if(descriptor) {
    Actor_PopulateBase(&actor->base, descriptor);
  }
  actor->base.Initialize = (PTRFUNC_3ARG) ObjCollectableActorInitialize;
  actor->base.Destroy = (PTRFUNC_2ARG) ObjCollectableActorDestroy;
  actor->base.Update = (PTRFUNC_2ARG) ObjCollectableActorUpdate;
  actor->base.Draw = (PTRFUNC_4ARGCHAR) ObjCollectableActorDraw;
  actor->base.visible = 1;
}

void ObjCollectableActorDestroy(struct Actor * a, void * scene) {
  ObjCollectableActor * actor = (ObjCollectableActor *)a;
}

void ObjCollectableActorUpdate(struct Actor * a, void * scene) {
  ObjCollectableActor * actor = (ObjCollectableActor *)a;
}

u_char * ObjCollectableActorDraw(struct Actor * a, MATRIX * view, u_char * packet_ptr, void * scene) {
  ObjCollectableActor * actor = (ObjCollectableActor *)a;

  SetSpadStack(SPAD_STACK_ADDR);
  
  ResetSpadStack();

  return packet_ptr;
}