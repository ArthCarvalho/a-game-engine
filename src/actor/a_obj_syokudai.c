/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#include "actor/a_obj_syokudai.h"
#include "scene/scene.h"

#include "spadstk.h"

struct SGM2 * obj_syokudai_model;

void ObjSyokudaiActorSetup() {

}

void ObjSyokudaiActorInitialize(struct Actor * a, void * descriptor, void * scene) {
  ObjSyokudaiActor * actor = (ObjSyokudaiActor *)a;
  if(descriptor) {
    Actor_PopulateBase(&actor->base, descriptor);
  }
  actor->base.Initialize = (PTRFUNC_3ARG) ObjSyokudaiActorInitialize;
  actor->base.Destroy = (PTRFUNC_2ARG) ObjSyokudaiActorDestroy;
  actor->base.Update = (PTRFUNC_2ARG) ObjSyokudaiActorUpdate;
  actor->base.Draw = (PTRFUNC_4ARGCHAR) ObjSyokudaiActorDraw;
  actor->base.visible = 1;
  actor->base.flags = ACTOR_STATIC;
  actor->base.collisionData.radius = 77;
  actor->base.collisionData.height = 435;
  actor->body = obj_syokudai_model;
  actor->flame_timer = -1;
  // Actor Tranformation
  actor->body_matrix.t[0] = actor->base.pos.vx;
  actor->body_matrix.t[1] = actor->base.pos.vy;
  actor->body_matrix.t[2] = actor->base.pos.vz;
  actor->body_matrix.m[0][0] = 4096;
  actor->body_matrix.m[0][1] = 0;
  actor->body_matrix.m[0][2] = 0;
  actor->body_matrix.m[1][0] = 0;
  actor->body_matrix.m[1][1] = 4096;
  actor->body_matrix.m[1][2] = 0;
  actor->body_matrix.m[2][0] = 0;
  actor->body_matrix.m[2][1] = 0;
  actor->body_matrix.m[2][2] = 4096;

  SVECTOR flame_pos = actor->base.pos;
  SVECTOR flame_scale = {4096, 4096, 4096, 0};
  flame_pos.vy += SYOKUDAI_FLAME_HEIGHT;

  Draw_CreateFlame(&actor->flame, &flame_pos, &flame_scale, 1, 1);

  CVECTOR lcolor = { 255, 255, 255 };

  actor->light = Lights_Create(&flame_pos, &lcolor, 1500, 0, scene);

}

void ObjSyokudaiActorDestroy(struct Actor * a, void * scene) {
  ObjSyokudaiActor * actor = (ObjSyokudaiActor *)a;
  Lights_Destroy(actor->light);
  actor->light = NULL;
}

void ObjSyokudaiActorUpdate(struct Actor * a, void * scene) {
  //SVECTOR local_rotate, camera_inverse, camera_inverse_cross;
  ObjSyokudaiActor * actor = (ObjSyokudaiActor *)a;
  Actor * player;
  struct Scene_Ctx * scene_ctx = (struct Scene_Ctx*)scene;
  player = scene_ctx->player;
  //struct Camera * camera = scene_ctx->camera;
  //actor->flame_matrix.m[0][0] = camera->zaxis.vx;
  //actor->flame_matrix.m[0][1] = camera->zaxis.vy;
  //actor->flame_matrix.m[0][2] = camera->zaxis.vz;
  //actor->flame_matrix.m[2][0] = camera->zaxis.vz;
  //actor->flame_matrix.m[2][1] = camera->zaxis.vy;
  //actor->flame_matrix.m[2][2] = -camera->zaxis.vx;
  //actor->flicker_frame++;
  //actor->flicker_frame %= SCREENFLARE_FLICKER_FRAMES;

  CollisionCylinder col_player;
  CollisionCylinder col_obj;
  short dist, intersect, deltax, deltaz;

  col_player.origin = player->pos;
  col_player.radius = player->collisionData.radius;
  col_player.height = player->collisionData.height;
  col_obj.origin = actor->base.pos;
  col_obj.radius = actor->base.collisionData.radius;
  col_obj.height = actor->base.collisionData.height; // = 435;

  if(ActorCollision_CheckCylinders(&col_obj, &col_player, &dist, &intersect, &deltax, &deltaz) == 1) {
    ActorCollision_DisplaceActor(player, dist, intersect, deltax, deltaz);
  }

  Draw_CalcNearestLight((Actor *)actor, scene);

  /*if(actor->base.xzDistance < 1024) {
    Scene_Ctx * scene_ctx = (Scene_Ctx*)scene;
    PlayerActor * player = scene_ctx->player;
    if(player->nearest_light_dist > actor->base.xzDistance) {
      short dist_x = ((player->base.pos.vx - actor->base.pos.vx) << 12) / actor->base.xzDistance;
      short dist_z = ((player->base.pos.vz - actor->base.pos.vz) << 12) / actor->base.xzDistance;
      player->nearest_shadow_ang = fix12_atan2s(dist_x, dist_z);
      player->nearest_light_dist = actor->base.xzDistance;
    }
  }*/



  /*short distXDelta = (player->pos.vx - actor->base.pos.vx);
  short distZDelta = (player->pos.vz - actor->base.pos.vz);
  short cyl_sum_radius = player->collisionData.radius + actor->base.collisionData.radius;
  //u_int dist = get_distanceXZ({distXDelta, 0, distZDelta, 0});
  u_int dist =  SquareRoot0(distXDelta * distXDelta + distZDelta * distZDelta);
  if(dist < cyl_sum_radius) { // Test - Collision Code
    short cyl_overlap = cyl_sum_radius - dist;
    if(dist != 0) { // If both are at the exact same position, ignore.
      u_long dispfactor = ((u_long)cyl_overlap<<12) / dist;
      short disp_x = (distXDelta * dispfactor) >> 12;
      short disp_z = (distZDelta * dispfactor) >> 12;
      player->collisionData.displacement_x += disp_x;
      player->collisionData.displacement_z += disp_z;
    }
  }*/

  if(actor->light) {
    actor->light->color.r = flame_colors[actor->flame.flame_color].r - (actor->flame.flicker_value*4); 
    actor->light->color.g = flame_colors[actor->flame.flame_color].g - (actor->flame.flicker_value*4);
    actor->light->color.b = flame_colors[actor->flame.flame_color].b - (actor->flame.flicker_value*4);
  }


}

u_char * ObjSyokudaiActorDraw(struct Actor * a, MATRIX * view, u_char * packet_ptr, void * scene) {
  ObjSyokudaiActor * actor = (ObjSyokudaiActor *)a;
  MATRIX local_identity;
  CompMatrixLV(view, &actor->body_matrix, &local_identity);
  gte_SetRotMatrix(&local_identity);
  gte_SetTransMatrix(&local_identity);
  SetSpadStack(SPAD_STACK_ADDR);
  
  packet_ptr = SGM2_UpdateModel(actor->body, packet_ptr, (u_long*)G.pOt, 0, SGM2_RENDER_AMBIENT, scene);

  packet_ptr = Draw_Flame(&actor->flame, view, packet_ptr, scene);
    
  ResetSpadStack();

  return packet_ptr;
}
