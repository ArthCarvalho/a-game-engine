/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#include "actor/a_obj_grass_cut.h"
#include "scene/scene.h"

#include "spadstk.h"

#define GRASS_CUT_REGROW_SCALE 1024

struct SGM2 * obj_grass_cut_full_model;
struct SGM2 * obj_grass_cut_half_model;

void ObjGrassCutActorSetup() {

}

void ObjGrassCutActorInitialize(struct Actor * a, void * descriptor, void * scene) {
  ObjGrassCutActor * actor = (ObjGrassCutActor *)a;
  actor->base.Initialize = (PTRFUNC_3ARG) ObjGrassCutActorInitialize;
  actor->base.Destroy = (PTRFUNC_2ARG) ObjGrassCutActorDestroy;
  actor->base.Update = (PTRFUNC_2ARG) ObjGrassCutActorUpdate;
  actor->base.Draw = (PTRFUNC_4ARGCHAR) ObjGrassCutActorDraw;
  actor->model_full = actor->model = obj_grass_cut_full_model;
  actor->model_cut = obj_grass_cut_half_model;
  actor->base.visible = 1;
  actor->base.flags = ACTOR_INTERACTABLE | ACTOR_ACCURATEDIST;
  actor->cut_timer = 0;
  actor->matrix.t[0] = actor->base.pos.vx;
  actor->matrix.t[1] = actor->base.pos.vy;
  actor->matrix.t[2] = actor->base.pos.vz;
  actor->scale.vx = 4096;
  actor->scale.vy = 4096;
  actor->scale.vz = 4096;
  SVECTOR rot = actor->base.rot;
  RotMatrix_gte(&rot, &actor->matrix);
}

void ObjGrassCutActorDestroy(struct Actor * a, void * scene) {

}

void ObjGrassCutActorUpdate(struct Actor * a, void * scene) {
  ObjGrassCutActor * actor = (ObjGrassCutActor *)a;
  struct Scene_Ctx * scene_ctx = (struct Scene_Ctx*)scene;
  Actor * player = scene_ctx->player;



  if(actor->cut_timer > 3) {
    actor->cut_timer--;
  } else if(actor->cut_timer == 3) { // Setup regrow!
    actor->cut_timer--;
    actor->model = actor->model_full;
    actor->matrix.t[0] = actor->base.pos.vx;
    actor->matrix.t[1] = actor->base.pos.vy;
    actor->matrix.t[2] = actor->base.pos.vz;
    actor->scale.vx = GRASS_CUT_REGROW_SCALE;
    actor->scale.vy = GRASS_CUT_REGROW_SCALE;
    actor->scale.vz = GRASS_CUT_REGROW_SCALE;
    VECTOR scale = actor->scale;
    SVECTOR rot = actor->base.rot;
    RotMatrix_gte(&rot, &actor->matrix);
    ScaleMatrixL(&actor->matrix, &scale);
  } else if(actor->cut_timer == 2) { // Grow up!
    actor->scale.vy += 102;
    if(actor->scale.vy > 4096) actor->scale.vy = 4096;
    actor->scale.vx += 76;
    actor->scale.vz += 76;
    if(actor->scale.vx > 4096) actor->scale.vx = 4096;
    if(actor->scale.vz > 4096) actor->scale.vz = 4096;
    if(actor->scale.vx == 4096) actor->cut_timer--;
    VECTOR scale = actor->scale;
    SVECTOR rot = actor->base.rot;
    RotMatrix_gte(&rot, &actor->matrix);
    ScaleMatrixL(&actor->matrix, &scale);
  } else if(actor->cut_timer == 1) { // Finish growing up
    actor->cut_timer--;
    actor->scale.vx = 4096;
    actor->scale.vy = 4096;
    actor->scale.vz = 4096;
    VECTOR scale = actor->scale;
    SVECTOR rot = actor->base.rot;
    RotMatrix_gte(&rot, &actor->matrix);
    ScaleMatrixL(&actor->matrix, &scale);
  } else {
    if(actor->base.xzDistance < 288) {
      actor->cut_timer = 240;
      actor->model = actor->model_cut;

      // Generate particles?
      ParticleEmitter pemitter;

      pemitter.position = actor->base.pos;
      pemitter.particle_floor = pemitter.position.vy - 32;
      pemitter.position.vy += 32;

      pemitter.rotation.vx = 128;
      pemitter.rotation.vy = 64;
      pemitter.rotation.vz = 128;

      pemitter.gravity.vx = 0;
      pemitter.gravity.vy = -1;
      pemitter.gravity.vz = 0;

      pemitter.starting_lifetime = 30*2;
      pemitter.size = 102;
      pemitter.min_gravity_speed = -10;
      pemitter.max_particles = 10;
      pemitter.generator_radius = 32;
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
      pemitter.v0 = (416) & 0xFF;

      Scene_ParticleCreate(&pemitter,NULL);

    }
  }


}

u_char * ObjGrassCutActorDraw(struct Actor * a, MATRIX * view, u_char * packet_ptr, void * scene) {
  ObjGrassCutActor * actor = (ObjGrassCutActor *)a;
  MATRIX local_identity;
  CompMatrixLV(view, &actor->matrix, &local_identity);
  gte_SetRotMatrix(&local_identity);
  gte_SetTransMatrix(&local_identity);
  struct SGM2 * model = actor->model;
  u_int render_flags = SGM2_RENDER_ABE | SGM2_RENDER_NO_NCLIP;

  if(__DEBUG_TOGGLE) render_flags |= SGM2_RENDER_AMBIENT;

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
