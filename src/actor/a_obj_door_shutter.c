/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#include "actor/a_obj_door_shutter.h"
#include "scene/scene.h"

#include "spadstk.h"

// Door Height: 663

SVECTOR door_shutter_vectors[] = {
  {-192, 663, 30, 0},
  {192, 663, 30, 0},
  {-192,332, 30, 0},
  {192, 332, 30, 0},
  {-192, 0, 30, 0},
  {192, 0, 30, 0},
};

UVCOORD door_shutter_coords[] = {
  {0, 0},
  {64, 0},
  {0, 64},
  {64, 64},
  {0, 128},
  {64, 128},
};

u_short door_shutter_open_anim[16] = {
  0,
  0.03301*256,
  0.125899*256,
  0.269455*256,
  0.454466*256,
  0.671719*256,
  0.912003*256,
  1.1661*256,
  1.42481*256,
  1.67891*256,
  1.9192*256,
  2.13645*256,
  2.32146*256,
  2.46502*256,
  2.55791*256,
  2.59092*256
};

u_char door_shutter_coord_anim[16] = {
  0,
  (0.03301/2.59092)*128,
  (0.125899/2.59092)*128,
  (0.269455/2.59092)*128,
  (0.454466/2.59092)*128,
  (0.671719/2.59092)*128,
  (0.912003/2.59092)*128,
  (1.1661/2.59092)*128,
  (1.42481/2.59092)*128,
  (1.67891/2.59092)*128,
  (1.9192/2.59092)*128,
  (2.13645/2.59092)*128,
  (2.32146/2.59092)*128,
  (2.46502/2.59092)*128,
  (2.55791/2.59092)*128,
  (2.59092/2.59092)*128
};

void ObjDoorShutterActorSetup() {

}

void ObjDoorShutterActorInitialize(struct Actor * a, void * descriptor, void * scene) {
  ObjDoorShutterActor * actor = (ObjDoorShutterActor *)a;
  Actor_Descriptor * desc = (Actor_Descriptor *)descriptor;
  actor->base.Initialize = (PTRFUNC_3ARG) ObjDoorShutterActorInitialize;
  actor->base.Destroy = (PTRFUNC_2ARG) ObjDoorShutterActorDestroy;
  actor->base.Update = (PTRFUNC_2ARG) ObjDoorShutterActorUpdate;
  actor->base.Draw = (PTRFUNC_4ARGCHAR) ObjDoorShutterActorDraw;
  //actor->base.flags = ACTOR_INTERACTABLE | ACTOR_ACCURATEDIST;
  //actor->base.collisionData.radius = 115;
  //actor->base.collisionData.height = 230;
  actor->base.visible = 1;

  actor->anim_timer = 0;
  actor->sub_timer = 0;
  actor->flip_model = 0;

  if(descriptor) {
    Actor_PopulateBase(&actor->base, descriptor);
  }

  actor->matrix.t[0] = actor->base.pos.vx;
  actor->matrix.t[1] = actor->base.pos.vy;
  actor->matrix.t[2] = actor->base.pos.vz;

  actor->front_room = desc->init_variables[0];
  actor->back_room = desc->init_variables[1];

  SVECTOR rot = actor->base.rot;
  RotMatrix_gte(&rot, &actor->matrix);
}

void ObjDoorShutterActorDestroy(struct Actor * a, void * scene) {
}

void ObjDoorShutterActorUpdate(struct Actor * a, void * scene) {
  ObjDoorShutterActor * actor = (ObjDoorShutterActor *)a;
  struct Scene_Ctx * scene_ctx = (struct Scene_Ctx*)scene;
  struct Camera * cam = (struct Camera *)((Scene_Ctx*)scene)->camera;
  Actor * player = scene_ctx->player;
  PlayerActor * pl = (PlayerActor *)player;

  // Set model facing directon
  VECTOR ppos = {player->pos.vx - actor->matrix.t[0], 0, player->pos.vz - actor->matrix.t[2]};
  short plen = SquareRoot0(ppos.vx * ppos.vx + ppos.vz * ppos.vz);
  ppos.vx = (ppos.vx<<12)/plen;
  ppos.vz = (ppos.vz<<12)/plen;
  SVECTOR z_dir = {actor->matrix.m[2][0], 0, -actor->matrix.m[2][2], 0};
  long dot = dotProductXZ(&ppos, &z_dir);
  
  if(dot >= 0) {
    actor->flip_model = 2048;
  } else {
    actor->flip_model = 0;
  }

  SVECTOR tdist = player->pos;
  tdist.vx -= actor->base.pos.vx;
  tdist.vy -= actor->base.pos.vy;
  tdist.vz -= actor->base.pos.vz;
  {
    int mask = (tdist.vx >> 31);
	  tdist.vx = (tdist.vx + mask) ^ mask;
    mask = (tdist.vz >> 31);
	  tdist.vz = (tdist.vz + mask) ^ mask;
  }
  if(tdist.vx < 400 && tdist.vz < 400) {
    pl->interact = a;
    //pl->action = ACTION_OPEN;
    if(actor->sub_timer == 0 && (g_pad_press & PAD_CIRCLE) && !(pl->state & PLAYER_CUTSCENE_MODE)) {
      actor->sub_timer = 1;
      scene_ctx->cinema_mode = 1;
      scene_ctx->interface_fade = 1;
      Player_ForceIdle((Actor *)pl);
      pl->state |= PLAYER_CUTSCENE_MODE;
      if(actor->back_room != actor->front_room) {
        scene_ctx->room_swap = 1;
      }
      if(!actor->flip_model) {
        pl->y_move_dir = (actor->base.rot.vy + 2048) % 4096;
        if(actor->back_room != actor->front_room) {
          scene_ctx->current_room_id = actor->back_room;
          scene_ctx->previous_room_id = actor->front_room;
        }
        actor->camera_side = 0;
      } else {
        pl->y_move_dir = actor->base.rot.vy;
        if(actor->back_room != actor->front_room) {
          scene_ctx->current_room_id = actor->front_room;
          scene_ctx->previous_room_id = actor->back_room;
        }
        actor->camera_side = 1;
      }
    }

  } else {
    if(actor->sub_timer == 2) {
      actor->sub_timer = 3;
    }
    //actor->base.visible = 0;
  }

  if(actor->sub_timer != 0) {
    cam->state = 2;
    cam->state_fix = 1;
  }


  if(actor->sub_timer == 1) {
    actor->anim_timer++;
    if(actor->anim_timer == 3) demo_counter = 31;
  }
  if(actor->anim_timer > 16) {
    actor->anim_timer = 16;
    actor->sub_timer = 2;
    // Swap Camera Sides
    SVECTOR cam_pos = {608, 945, -1348, 0};
    if(actor->camera_side) cam_pos.vz = -cam_pos.vz;
    VECTOR f_cam_pos;
    gte_SetRotMatrix(&actor->matrix);
    ApplyRotMatrix(&cam_pos, &f_cam_pos);
    cam->position.vx = f_cam_pos.vx + actor->matrix.t[0];
    cam->position.vy = f_cam_pos.vy + actor->matrix.t[1];
    cam->position.vz = f_cam_pos.vz + actor->matrix.t[2];
  }
  if(actor->sub_timer == 3) {
    actor->anim_timer-=2;
  }
  if(actor->anim_timer > 128) {
    actor->anim_timer = 0;
    actor->sub_timer = 0;
    Camera_AddQuake(cam, 8);
    cam->state = 0;
    cam->state_fix = 0;
    scene_ctx->cinema_mode = 0;
    scene_ctx->interface_fade = 0;
    if(actor->back_room != actor->front_room) {
      scene_ctx->previous_room_m = NULL;
      scene_ctx->actor_cleanup = 1;
    }
    pl->state &= ~PLAYER_CUTSCENE_MODE;
  }
  //FntPrint("anim_timer %d\n", actor->anim_timer);
}

#define DOOR_SHUTTER_DEPTH_MOD 10

u_char * ObjDoorShutterActorDraw(struct Actor * a, MATRIX * view, u_char * packet_ptr, void * scene) {
  ObjDoorShutterActor * actor = (ObjDoorShutterActor *)a;
  MATRIX local_identity;
  MATRIX anim_identity = actor->matrix;
  if(actor->flip_model) {
    anim_identity.m[0][0] = -actor->matrix.m[0][0];
    anim_identity.m[0][2] = -actor->matrix.m[0][2];
    anim_identity.m[2][0] = -actor->matrix.m[2][0];
    anim_identity.m[2][2] = -actor->matrix.m[2][2];
  }
  CompMatrixLV(view, &anim_identity, &local_identity);
  gte_SetRotMatrix(&local_identity);
  gte_SetTransMatrix(&local_identity);

  SVECTOR door_verts_tmp[6];
  UVCOORD door_coord_tmp[6];
  short anim_offs = door_shutter_open_anim[actor->anim_timer];
  short uv_offs = door_shutter_coord_anim[actor->anim_timer];


  door_verts_tmp[0] = door_shutter_vectors[0];
  door_verts_tmp[1] = door_shutter_vectors[1];
  door_verts_tmp[2] = door_shutter_vectors[2];
  door_verts_tmp[3] = door_shutter_vectors[3];
  door_verts_tmp[4] = door_shutter_vectors[4];
  door_verts_tmp[5] = door_shutter_vectors[5];

  door_coord_tmp[0] = door_shutter_coords[0];
  door_coord_tmp[1] = door_shutter_coords[1];
  door_coord_tmp[2] = door_shutter_coords[2];
  door_coord_tmp[3] = door_shutter_coords[3];
  door_coord_tmp[4] = door_shutter_coords[4];
  door_coord_tmp[5] = door_shutter_coords[5];

  if(actor->anim_timer) {
    door_verts_tmp[2].vy = door_verts_tmp[2].vy + anim_offs;
    door_verts_tmp[3].vy = door_verts_tmp[3].vy + anim_offs;
    door_verts_tmp[4].vy = door_verts_tmp[4].vy + anim_offs;
    door_verts_tmp[5].vy = door_verts_tmp[5].vy + anim_offs;

    door_coord_tmp[0].v = door_coord_tmp[0].v + door_shutter_coord_anim[actor->anim_timer];
    door_coord_tmp[1].v = door_coord_tmp[1].v + door_shutter_coord_anim[actor->anim_timer];

    if(door_verts_tmp[2].vy > 663) {
      door_verts_tmp[2].vy = 663;
      door_verts_tmp[3].vy = 663;
      door_coord_tmp[2].v = door_coord_tmp[2].v + (uv_offs-64);
      door_coord_tmp[3].v = door_coord_tmp[3].v + (uv_offs-64);
    }
  }

  POLY_FT4 * poly = (POLY_FT4*)packet_ptr;

  long outer_product, otz;

  gte_ldv3(&door_verts_tmp[0], &door_verts_tmp[1], &door_verts_tmp[2]);
  gte_rtpt();
  gte_avsz3();
  gte_stotz(&otz);
  otz = (otz >> OTSUBDIV) + DOOR_SHUTTER_DEPTH_MOD;
  if (otz > OTMIN && otz < OTSIZE) {
    long otz2;
    CVECTOR color = { 0x80, 0x80, 0x80, 0x00 };
    gte_stsxy3((long *)&poly->x0,(long *)&poly->x1,(long *)&poly->x2);

    gte_ldv3(&door_verts_tmp[3], &door_verts_tmp[4], &door_verts_tmp[5]);
    gte_rtpt();
    gte_avsz3();
    gte_stotz(&otz2);
    otz2 = (otz2 >> OTSUBDIV) + DOOR_SHUTTER_DEPTH_MOD;
    long temp0, temp1;
    gte_stsxy3((long *)&poly->x3,&temp0, &temp1);

    *(u_short*)&poly->u0 = *(u_short*)&door_coord_tmp[0].u;
    *(u_short*)&poly->u1 = *(u_short*)&door_coord_tmp[1].u;
    *(u_short*)&poly->u2 = *(u_short*)&door_coord_tmp[2].u;
    *(u_short*)&poly->u3 = *(u_short*)&door_coord_tmp[3].u;

    gte_ldrgb(&color);
    gte_cc();
    gte_strgb(&color);

    *(u_long*)&poly->r0 = *(u_long*)&color.r;

    poly->tpage = getTPage(0, 0, 384, 0);
    poly->clut = getClut(784,500);

    setPolyFT4(poly);
    addPrim(G.pOt+otz, poly);
    POLY_FT4 * prevpoly = poly;
    poly++;

    *(long *)&poly->x0 = *(long *)&prevpoly->x2;
    *(long *)&poly->x1 = *(long *)&prevpoly->x3;
    *(long *)&poly->x2 = temp0;
    *(long *)&poly->x3 = temp1;

    *(u_short*)&poly->u0 = *(u_short*)&door_coord_tmp[2].u;
    *(u_short*)&poly->u1 = *(u_short*)&door_coord_tmp[3].u;
    *(u_short*)&poly->u2 = *(u_short*)&door_coord_tmp[4].u;
    *(u_short*)&poly->u3 = *(u_short*)&door_coord_tmp[5].u;

    *(u_long*)&poly->r0 = *(u_long*)&color.r;

    poly->tpage = getTPage(0, 0, 384, 0);
    poly->clut = getClut(784,500);

    setPolyFT4(poly);
    addPrim(G.pOt+otz2, poly);
    poly++;
  }

  


  

  return (u_char*)poly;
}
