/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#include "actor/a_obj_tsubo.h"
#include "scene/scene.h"

#include "spadstk.h"

struct SGM2 * obj_tsubo_model;
struct SGM2 * obj_tsubo_far_model;

void ObjTsuboActorSetup() {

}

void ObjTsuboActorInitialize(struct Actor * a, void * descriptor, void * scene) {
  ObjTsuboActor * actor = (ObjTsuboActor *)a;
  Actor_Descriptor * desc = (Actor_Descriptor *)descriptor;
  actor->base.Initialize = (PTRFUNC_3ARG) ObjTsuboActorInitialize;
  actor->base.Destroy = (PTRFUNC_2ARG) ObjTsuboActorDestroy;
  actor->base.Update = (PTRFUNC_2ARG) ObjTsuboActorUpdate;
  actor->base.Draw = (PTRFUNC_4ARGCHAR) ObjTsuboActorDraw;
  actor->model = obj_tsubo_model;
  actor->model_far = obj_tsubo_far_model;
  actor->base.visible = 1;
  actor->base.flags = ACTOR_INTERACTABLE | ACTOR_ACCURATEDIST;
  actor->base.collisionData.radius = 115;
  actor->base.collisionData.height = 230;

  actor->matrix.t[0] = actor->base.pos.vx = desc->x;
  actor->matrix.t[1] = actor->base.pos.vy = desc->y;
  actor->matrix.t[2] = actor->base.pos.vz = desc->z;
  actor->base.rot.vx = desc->rot_x;
  actor->base.rot.vy = desc->rot_y;
  actor->base.rot.vz = desc->rot_z;
  SVECTOR rot = actor->base.rot;
  RotMatrix_gte(&rot, &actor->matrix);
}

void ObjTsuboActorDestroy(struct Actor * a, void * scene) {

}

void ObjTsuboActorUpdate(struct Actor * a, void * scene) {
  ObjTsuboActor * actor = (ObjTsuboActor *)a;
  struct Scene_Ctx * scene_ctx = (struct Scene_Ctx*)scene;
  Actor * player = scene_ctx->player;
  short draw_dist = scene_ctx->draw_dist;

  SVECTOR tdist = player->pos;
  tdist.vx -= actor->base.pos.vx;
  tdist.vy -= actor->base.pos.vy;
  tdist.vz -= actor->base.pos.vz;
  {
    int mask = (tdist.vx >> 31);
	  tdist.vx = (tdist.vx + mask) ^ mask;
    //mask = (tdist.vy >> 31);
	  //tdist.vy = (tdist.vy + mask) ^ mask;
    mask = (tdist.vz >> 31);
	  tdist.vz = (tdist.vz + mask) ^ mask;
  }
  if(tdist.vx < draw_dist && tdist.vz < draw_dist) {
    /*u_int dist = get_distanceXZ(&tdist);
    //FntPrint("D:%d\n",dist);
    actor->dist = dist;
    //actor->base.visible = 1;
    // !!! TEST COLLISION CODE !!!
    // Player Cyl: 180, Obj Cyl: 220, Cyl half: 90/110
    short cyl_sum_radius = player->collisionData.radius + actor->base.collisionData.radius;
    short cyl_overlap = 0;
    if(dist < cyl_sum_radius) { // Test - Collision Code
      cyl_overlap = cyl_sum_radius - dist;
      if(dist != 0) { // If both are at the exact same position, ignore.
        u_long dispfactor = ((u_long)cyl_overlap<<12) / dist;
        short disp_x = ((player->pos.vx - actor->base.pos.vx) * dispfactor) >> 12;
        short disp_z = ((player->pos.vz - actor->base.pos.vz) * dispfactor) >> 12;
        player->collisionData.displacement_x += disp_x;
        player->collisionData.displacement_z += disp_z;

      }
    }*/
    CollisionCylinder col_player;
    CollisionCylinder col_obj;
    short dist, intersect, deltax, deltaz;

    col_player.origin = player->pos;
    col_player.radius = player->collisionData.radius;
    col_player.height = player->collisionData.height;
    col_obj.origin = actor->base.pos;
    col_obj.radius = actor->base.collisionData.radius;
    col_obj.height = actor->base.collisionData.height; // 230;
    

    if(ActorCollision_CheckCylinders(&col_obj, &col_player, &dist, &intersect, &deltax, &deltaz) == 1) {
      ActorCollision_DisplaceActor(player, dist, intersect, deltax, deltaz);
    }

    actor->matrix.t[0] = actor->base.pos.vx;
    actor->matrix.t[1] = actor->base.pos.vy;
    actor->matrix.t[2] = actor->base.pos.vz;

    //SVECTOR rot = {actor->base.rot.vx, actor->base.rot.vy, actor->base.rot.vz, 0};
    SVECTOR rot = actor->base.rot;
    RotMatrix_gte(&rot, &actor->matrix);
  } else {
    //actor->base.visible = 0;
  }
  //if(__DEBUG_TOGGLE) {
  //  actor->dist = get_distanceXZ(&tdist);
  //} else {
  //  actor->dist = get_distanceXYZ(&tdist);
  //}

  /*if(tdist.vx < 256 && tdist.vx > -256 && tdist.vz < 256 & tdist.vz > -256) {
    u_int dist = get_distanceXZ(&tdist);
    FntPrint("D:%d\n",dist);
    actor->dist = dist;
  }*/
  
  

  
}

u_char * ObjTsuboActorDraw(struct Actor * a, MATRIX * view, u_char * packet_ptr, void * scene) {
  ObjTsuboActor * actor = (ObjTsuboActor *)a;
  MATRIX local_identity;
  CompMatrixLV(view, &actor->matrix, &local_identity);
  gte_SetRotMatrix(&local_identity);
  gte_SetTransMatrix(&local_identity);
  struct SGM2 * model = actor->model;

  SVECTOR vec0, vec1;
  long otzv;
  vec0.vx = 0;
  vec0.vy = 0;
  vec0.vz = 0;
  gte_ldv0(&vec0);
  gte_rtps();
  gte_stsxy((long*)&vec0.vx);
  //gte_stotz(&otzv);
  gte_stsz(&otzv);
  if(otzv < 0) return packet_ptr;
  if(otzv > 3000) model = actor->model_far;
  if(otzv > 8000) return packet_ptr;
  // Frustum culling
  if(vec0.vx < -64 || vec0.vx > SCREEN_W+64) return packet_ptr;
  if(vec0.vy < -64 || vec0.vy > SCREEN_H+128) return packet_ptr;


  SetSpadStack(SPAD_STACK_ADDR);
  packet_ptr = SGM2_UpdateModel(model, packet_ptr, (u_long*)G.pOt, 0, SGM2_RENDER_AMBIENT, scene);
  ResetSpadStack();

  return packet_ptr;
}
