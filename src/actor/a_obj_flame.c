/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#include "actor/a_obj_flame.h"
#include "scene/scene.h"

#include "spadstk.h"

void ObjFlameActorSetup() {
  // Generate Screen Flare
  short gen_ang = 0;
  int step_tri = 0;
  int step_quad = 0;
  for(int i = 0; i < SCREENFLARE_SEGMENTS; i++) {
    gen_ang += SCREENFLARE_SEGMENT_ANG;
    short sin = isin(gen_ang);
    short cos = icos(gen_ang);
    flare_verts[i].vx = sin;
    flare_verts[i].vy = cos;
    flare_verts[i].vz = 0;
    flare_verts[i+SCREENFLARE_SEGMENTS].vx = (sin * SCREENFLARE_INNER_RING_SCALE) >> 12;
    flare_verts[i+SCREENFLARE_SEGMENTS].vy = (cos * SCREENFLARE_INNER_RING_SCALE) >> 12;
    flare_verts[i+SCREENFLARE_SEGMENTS].vz = 0;

    flare_index_outter_ring[step_quad] = i;
    flare_index_outter_ring[step_quad+1] = i + 1;
    flare_index_outter_ring[step_quad+2] = i + SCREENFLARE_SEGMENTS + 0;
    flare_index_outter_ring[step_quad+3] = i + SCREENFLARE_SEGMENTS + 1;
    if(i == SCREENFLARE_SEGMENTS-1) {
      flare_index_outter_ring[step_quad+1] -= SCREENFLARE_SEGMENTS;
      flare_index_outter_ring[step_quad+3] -= SCREENFLARE_SEGMENTS;
    }
    step_quad += 4;

    flare_index_inner[step_tri] = i + SCREENFLARE_SEGMENTS;
    flare_index_inner[step_tri+1] = i + SCREENFLARE_SEGMENTS + 1;
    flare_index_inner[step_tri+2] = SCREENFLARE_SEGMENTS2-1;
    if(i == SCREENFLARE_SEGMENTS-1) {
      flare_index_inner[step_tri+1] -= SCREENFLARE_SEGMENTS;
    }
    step_tri += 3;
  }
  SVECTOR zero = { 0, 0, 0, 0 };
  flare_verts[SCREENFLARE_SEGMENTS2-1] = zero;

  for(int i = 0; i < SCREENFLARE_FLICKER_FRAMES; i++) {
    flicker_frame[i] = rand() >> 11;
  }
}

void ObjFlameActorInitialize(struct Actor * a, void * descriptor, void * scene) {
  ObjFlameActor * actor = (ObjFlameActor *)a;
  Actor_Descriptor * actor_descriptor = (Actor_Descriptor *)descriptor;
  if(descriptor) {
    Actor_PopulateBase(&actor->base, descriptor);
  }
  actor->base.Initialize = (PTRFUNC_3ARG) ObjFlameActorInitialize;
  actor->base.Destroy = (PTRFUNC_2ARG) ObjFlameActorDestroy;
  actor->base.Update = (PTRFUNC_2ARG) ObjFlameActorUpdate;
  actor->base.Draw = (PTRFUNC_4ARGCHAR) ObjFlameActorDraw;
  actor->base.visible = 1;

  SVECTOR flame_pos = actor->base.pos;
  SVECTOR flame_scale = actor->base.scale;

  Draw_CreateFlame(&actor->flame, &flame_pos, &flame_scale, actor_descriptor->init_variables[0], actor_descriptor->init_variables[1]);
  actor->flame.flare_scale = actor_descriptor->init_variables[2];
  actor->flame.flare_dist = actor_descriptor->init_variables[3];

  CVECTOR lcolor = { 255, 255, 255 };
  actor->light = Lights_Create(&flame_pos, &lcolor, 2000, 0, scene);

}

void ObjFlameActorDestroy(struct Actor * a, void * scene) {
  ObjFlameActor * actor = (ObjFlameActor *)a;
  Lights_Destroy(actor->light);
  actor->light = NULL;

}

void ObjFlameActorUpdate(struct Actor * a, void * scene) {
  ObjFlameActor * actor = (ObjFlameActor *)a;
  Draw_CalcNearestLight(a, scene);
  if(actor->light) {
    actor->light->color.r = flame_colors[actor->flame.flame_color].r - (actor->flame.flicker_value*4); 
    actor->light->color.g = flame_colors[actor->flame.flame_color].g - (actor->flame.flicker_value*4);
    actor->light->color.b = flame_colors[actor->flame.flame_color].b - (actor->flame.flicker_value*4);
  }
}

u_char * ObjFlameActorDraw(struct Actor * a, MATRIX * view, u_char * packet_ptr, void * scene) {
  ObjFlameActor * actor = (ObjFlameActor *)a;

  SetSpadStack(SPAD_STACK_ADDR);

  packet_ptr = Draw_Flame(&actor->flame, view, packet_ptr, scene);
  
  ResetSpadStack();

  return packet_ptr;
}