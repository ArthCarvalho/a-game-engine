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
#include "texture/texture.h"

#define REFLECTION_VRAM_X 448
#define REFLECTION_VRAM_Y 256
#define REFLECTION_CLUT_X 128
#define REFLECTION_CLUT_Y 478

extern unsigned long obj_item_sphere_sgm2[];
extern unsigned long obj_item_sphere_low_sgm2[];
extern unsigned long obj_item_sphere_tim[];
SGM2_File * item_model;
SGM2_File * item_model_low;

CVECTOR ruppy_color[] = {
  {145*0.75, 255*0.75, 34*0.75, 0},   // Green
  {17*0.75, 113*0.75, 255*0.75, 0},   // Blue
  {255*0.75, 17*0.75, 17*0.75, 0},    // Red
  {255*0.75, 195*0.75, 17*0.75, 0},   // Yellow
  {130*0.75, 17*0.75, 195*0.75, 0},   // Purple
  {128, 128, 128, 0}    // Silver
};

#define ITEM_YANIM_FRAMES 14

short y_offset_anim [] = {
  256*0.333489,
  256*0.175082,
  256*0.0,
  256*0.170914,
  256*0.283466,
  256*0.158408,
  256*0.0,
  256*0.110048,
  256*0.214263,
  256*0.101711,
  256*0.085036,
  256*0.0,
  256*0.085036,
  256*0.0,
};

void ObjCollectableActorSetup() {
  load_texture_pos((unsigned long)obj_item_sphere_tim, REFLECTION_VRAM_X, REFLECTION_VRAM_Y, REFLECTION_CLUT_X, REFLECTION_CLUT_Y);
  item_model = SGM2_LoadFile((u_long*)obj_item_sphere_sgm2);
  item_model_low = SGM2_LoadFile((u_long*)obj_item_sphere_low_sgm2);
  item_model_low->material[0].tpage = item_model->material[0].tpage = GetTPage(1, 0, REFLECTION_VRAM_X, REFLECTION_VRAM_Y);
  item_model_low->material[0].clut = item_model->material[0].clut = GetClut(REFLECTION_CLUT_X, REFLECTION_CLUT_Y);
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
  actor->roty = 0;
  actor->item_type = actor_descriptor->init_variables[0];
  actor->state = 0;
  actor->counter = 0;
}

void ObjCollectableActorDestroy(struct Actor * a, void * scene) {
  ObjCollectableActor * actor = (ObjCollectableActor *)a;
  

}

void ObjCollectableActorUpdate(struct Actor * a, void * scene) {
  ObjCollectableActor * actor = (ObjCollectableActor *)a;
  Scene_Ctx * scene_ctx = (Scene_Ctx *)scene;
  actor->roty = (actor->roty+64) & 0x0FFF;

  PlayerActor * player = (PlayerActor *)scene_ctx->player;

  switch(actor->state) {
    case 0:
      if(actor->base.xzDistanceSq < (200*200) && (player->base.pos.vy < (actor->base.pos.vy + 128)) && (player->base.pos.vy > (actor->base.pos.vy - 128))){
        actor->state = 1;
      }
      break;
    case 1:
      
      if(actor->counter > ITEM_YANIM_FRAMES-1) {
        actor->base.Update = NULL;
      }
      actor->base.pos = player->base.pos;
      actor->base.pos.vy += y_offset_anim[actor->counter] + (358);
      actor->counter++;
  }
  

}

u_char * ObjCollectableActorDraw(struct Actor * a, MATRIX * view, u_char * packet_ptr, void * scene) {
  ObjCollectableActor * actor = (ObjCollectableActor *)a;
  Scene_Ctx * scene_ctx = (Scene_Ctx *)scene;
  SGM2_File * model = item_model;
  MATRIX local_identity;
  MATRIX base_mat;

  SVECTOR rot = {0, actor->roty, 0, 0};
  RotMatrixZYX(&rot, &base_mat);
  base_mat.t[0] = actor->base.pos.vx;
  base_mat.t[1] = actor->base.pos.vy;
  base_mat.t[2] = actor->base.pos.vz;


  SetSpadStack(SPAD_STACK_ADDR);

  MATRIX view_scale = scene_ctx->camera->matrix_unscaled;
  VECTOR scalev = {30, 30, 30, 0};
  ScaleMatrix(&view_scale, &scalev);

  CompMatrixLV(&view_scale, &base_mat, &local_identity);

  SGM2_GenerateReflectionUV(model, &local_identity, 0, 0);

  CompMatrixLV(view, &base_mat, &local_identity);

  gte_SetRotMatrix(&local_identity);
  gte_SetTransMatrix(&local_identity);


  /*SVECTOR vec0, vec1;
  long otzv;
  vec0.vx = 0;
  vec0.vy = 0;
  vec0.vz = 0;
  gte_ldv0(&vec0);
  gte_rtps();
  gte_stsxy((long*)&vec1.vx);
  gte_stsz(&otzv);
  // Z culling
  if(otzv < 0) return packet_ptr;
  // Frustum culling
  if(vec1.vx < -64 || vec1.vx > SCREEN_W+64) return packet_ptr;
  if(vec1.vy < -64 || vec1.vy > SCREEN_H+64) return packet_ptr;
  if(otzv < 4000) model = item_model;*/

  packet_ptr = SGM2_UpdateModelColor(model, packet_ptr, (u_long*)G.pOt, 0, 0, ruppy_color[actor->item_type], scene);
  
  ResetSpadStack();

  return packet_ptr;
}