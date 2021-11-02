/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#include "actor/a_obj_grass.h"
#include "scene/scene.h"

#include "SPADSTK.h"

struct SGM2 * obj_grass_model;
struct SGM2 * obj_grass_far_model;
struct SGM2 * obj_grass_far2_model;

void ObjGrassActorSetup() {

}

void ObjGrassActorInitialize(struct Actor * a, void * descriptor, void * scene) {
  ObjGrassActor * actor = (ObjGrassActor *)a;
  actor->base.Initialize = (PTRFUNC_3ARG) ObjGrassActorInitialize;
  actor->base.Destroy = (PTRFUNC_2ARG) ObjGrassActorDestroy;
  actor->base.Update = (PTRFUNC_2ARG) ObjGrassActorUpdate;
  actor->base.Draw = (PTRFUNC_4ARGCHAR) ObjGrassActorDraw;
  actor->model = obj_grass_model;
  actor->model_far = obj_grass_far_model;
  actor->model_far2 = obj_grass_far2_model;
  actor->base.visible = 1;
  actor->base.flags = ACTOR_INTERACTABLE | ACTOR_ACCURATEDIST;

  actor->matrix.t[0] = actor->base.pos.vx;
  actor->matrix.t[1] = actor->base.pos.vy;
  actor->matrix.t[2] = actor->base.pos.vz;
  SVECTOR rot = actor->base.rot;
  RotMatrix_gte(&rot, &actor->matrix);
}

void ObjGrassActorDestroy(struct Actor * a, void * scene) {

}

void ObjGrassActorUpdate(struct Actor * a, void * scene) {
  ObjGrassActor * actor = (ObjGrassActor *)a;
  //struct Scene_Ctx * scene_ctx = (struct Scene_Ctx*)scene;
  //Actor * player = scene_ctx->player;

  if(actor->base.xzDistance < 288) {
    actor->base.Update = NULL;
    // Generate particles?
      ParticleEmitter pemitter;

      pemitter.position = actor->base.pos;
      pemitter.particle_floor = pemitter.position.vy - 32;
      pemitter.position.vy += 32;

      pemitter.rotation.vx = 8;
      pemitter.rotation.vy = 2;
      pemitter.rotation.vz = 6;

      pemitter.gravity.vx = 0;
      pemitter.gravity.vy = -1;
      pemitter.gravity.vz = 0;

      pemitter.starting_lifetime = 30*2;
      pemitter.size = 102;
      pemitter.min_gravity_speed = -10;
      pemitter.max_particles = 10;
      pemitter.generator_radius = 64;
      pemitter.center_offset = -32;

      //pemitter.r0 = scene_ctx->ambient.r;
      //pemitter.g0 = scene_ctx->ambient.g;
      //pemitter.b0 = scene_ctx->ambient.b;

      pemitter.r0 = 0x80;
      pemitter.g0 = 0x80;
      pemitter.b0 = 0x80;

      pemitter.tpage = actor->model->material[0].tpage;
      pemitter.clut = actor->model->material[0].clut;
      
      pemitter.u0 = (36*4) & 0xFF;
      pemitter.v0 = (384) & 0xFF;

      Scene_ParticleCreate(&pemitter,NULL);
  }
}

u_char * ObjGrassActorDraw(struct Actor * a, MATRIX * view, u_char * packet_ptr, void * scene) {
  ObjGrassActor * actor = (ObjGrassActor *)a;
  MATRIX local_identity;
  CompMatrixLV(view, &actor->matrix, &local_identity);
  gte_SetRotMatrix(&local_identity);
  gte_SetTransMatrix(&local_identity);
  struct SGM2 * model = actor->model;
  u_int render_flags = SGM2_RENDER_ABE | SGM2_RENDER_NO_NCLIP;

  SVECTOR vec0, vec1;
  long otzv;
  vec0.vx = 0;
  vec0.vy = 0;
  vec0.vz = 0;
  gte_ldv0(&vec0);
  gte_rtps();
  gte_stsxy((long*)&vec0.vx);
  gte_stsz(&otzv);
  if(otzv < 0) return packet_ptr;
  if(otzv > 2000) model = actor->model_far;
  if(otzv > 3000) model = actor->model_far2;
  if(otzv > 7000) render_flags |= SGM2_RENDER_BUMPCLUT;
  if(otzv > 8000) return packet_ptr;
  // Frustum culling
  if(vec0.vx < -64 || vec0.vx > SCREEN_W+64) return packet_ptr;
  if(vec0.vy < -64 || vec0.vy > SCREEN_H+128) return packet_ptr;


  SetSpadStack(SPAD_STACK_ADDR);
  packet_ptr = SGM2_UpdateModel(model, packet_ptr, (u_long*)G.pOt, 0, render_flags, scene);
  ResetSpadStack();

  return packet_ptr;
}
