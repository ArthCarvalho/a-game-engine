/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#include "actor/a_obj_swap_plane.h"
#include "scene/scene.h"

#include "spadstk.h"

/* Swap Plane Class */
void ObjSwapPlaneActorInitialize(struct Actor * address, Actor_Descriptor * descriptor, void * scene) {
  ObjSwapPlaneActor * myself = (ObjSwapPlaneActor*)address;
  myself->base.Initialize = (PTRFUNC_3ARG) ObjSwapPlaneActorInitialize;
  myself->base.Destroy = (PTRFUNC_2ARG) ObjSwapPlaneActorDestroy;
  myself->base.Update = (PTRFUNC_2ARG) ObjSwapPlaneActorUpdate;
  myself->base.Draw = (PTRFUNC_4ARGCHAR) ObjSwapPlaneActorDraw;

  Actor_PopulateBase(address, descriptor);
  myself->base.room = 0xFF; // Process independent of current room location
  myself->base.visible = 0;
  
  myself->front_room = descriptor->init_variables[0];
  myself->back_room = descriptor->init_variables[1];

  myself->base.bbox.max_x = descriptor->init_variables[2];
  myself->base.bbox.max_y = descriptor->init_variables[3];
  myself->base.bbox.max_z = descriptor->init_variables[4];

  myself->base.bbox.min_x = descriptor->init_variables[5];
  myself->base.bbox.min_y = descriptor->init_variables[6];
  myself->base.bbox.min_z = descriptor->init_variables[7];

  
  

  myself->display_type = 0; // No Display

  myself->swap_start = 0;
  myself->swap_state = 0;
  myself->swap_active = 0;
};

void ObjSwapPlaneActorDestroy(struct Actor * address, void * scene) {

};

void ObjSwapPlaneActorUpdate(struct Actor * actor, void * scene) {
  int d_x, d_y, d_z, dist;
  ObjSwapPlaneActor * myself = (ObjSwapPlaneActor *)actor;
  //PlayerActor * player = (PlayerActor *)scene->player;
  Scene_Ctx * scene_ctx = (Scene_Ctx *)scene;
  Actor * player = scene_ctx->player; 
  // Bounding check
  //FntPrint("S:X[%d][%d]Y[%d][%d]Z[%d][%d]\n",
  //myself->base.bbox.min_x, myself->base.bbox.max_x,
  //myself->base.bbox.min_y, myself->base.bbox.max_y,
  //myself->base.bbox.min_z, myself->base.bbox.max_z);
  myself->swap_state_prev = myself->swap_state;
  if(player->pos.vy > myself->base.bbox.min_y && player->pos.vy < myself->base.bbox.max_y){
    if(player->pos.vx > myself->base.bbox.min_x && player->pos.vx < myself->base.bbox.max_x){
      if(player->pos.vz > myself->base.bbox.min_z && player->pos.vz < myself->base.bbox.max_z){
          
          // Get Dist
          d_x = player->pos.vx - myself->base.pos.vx;
          d_y = player->pos.vy - myself->base.pos.vy;
          d_z = player->pos.vz - myself->base.pos.vz;
          dist = d_x * d_x + d_y * d_y + d_z * d_z;
          //FntPrint("Swap Dist: %d [0x%0x]\n", dist, dist);
          if(d_x < 180 && d_x > -180) {
            if(d_x > 0 && myself->swap_start < 0 || d_x < 0 && myself->swap_start > 0){
              if(d_x > 0) {
                scene_ctx->current_room_id = myself->front_room;
                scene_ctx->previous_room_id = myself->back_room;
              } else {
                scene_ctx->current_room_id = myself->back_room;
                scene_ctx->previous_room_id = myself->front_room;
              }
              scene_ctx->room_swap = 1;
              scene_ctx->actor_cleanup = 1;
              scene_ctx->previous_room_m = NULL;
            }
          }

          myself->swap_start = d_x;
      }
    }
  }

};

u_char * ObjSwapPlaneActorDraw(struct Actor * address, void * view, unsigned char * buffer, void * scene) {
  return buffer;
};