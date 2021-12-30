/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#include "actor/a_obj_dekunuts.h"
#include "scene/scene.h"

#include "spadstk.h"

// Animation Data
#include "../../AGAME_PCDATA/objects/enemy/dekunuts/dekunuts.agm.anm.h"

struct AGM_Model obj_dekunuts_model;
struct ANM_Animation * obj_dekunuts_anim;
u_short obj_dekunuts_tpage;
u_short obj_dekunuts_clut[3];

struct SGM2 * obj_dekunuts_plant_model;
struct SGM2 * obj_dekunuts_body_model;

#define OBJ_DEKUNUTS_PLANT_TEXTURE_X 64
#define OBJ_DEKUNUTS_PLANT_TEXTURE_Y 256
#define OBJ_DEKUNUTS_BODY_TEXTURE_X 64
#define OBJ_DEKUNUTS_BODY_TEXTURE_Y 320

#define OBJ_DEKUNUTS_PLANT_CLUT_X 64
#define OBJ_DEKUNUTS_PLANT_CLUT_Y 512-2
#define OBJ_DEKUNUTS_BODY_CLUT_X 64
#define OBJ_DEKUNUTS_BODY_CLUT_Y 512-1

void ObjDekunutsActorSetup(struct Actor * a, void * scene) {
  load_texture_pos(
    (unsigned long)obj_dekunuts_plant_tim,
    OBJ_DEKUNUTS_PLANT_TEXTURE_X, OBJ_DEKUNUTS_PLANT_TEXTURE_Y,
    OBJ_DEKUNUTS_PLANT_CLUT_X, OBJ_DEKUNUTS_PLANT_CLUT_Y
  );
  load_texture_pos(
    (unsigned long)obj_dekunuts_body_tim,
    OBJ_DEKUNUTS_BODY_TEXTURE_X, OBJ_DEKUNUTS_BODY_TEXTURE_Y,
    OBJ_DEKUNUTS_BODY_CLUT_X, OBJ_DEKUNUTS_BODY_CLUT_Y
  );

  u_short white_clut[16] = {
    0x3DEF, 0x3DEF, 0x3DEF, 0x3DEF, 0x3DEF, 0x3DEF, 0x3DEF, 0x3DEF, 0x3DEF, 0x3DEF, 0x3DEF, 0x3DEF, 0x3DEF, 0x3DEF, 0x3DEF, 0x3DEF
  };

  load_clut_pos((unsigned long)white_clut, OBJ_DEKUNUTS_PLANT_CLUT_X, OBJ_DEKUNUTS_PLANT_CLUT_Y-1, 16, 1);

  obj_dekunuts_plant_model = (struct SGM2 *) SGM2_LoadFile((u_long*)obj_dekunuts_plant_sgm2);
  obj_dekunuts_plant_model->material[0].clut = GetClut(OBJ_DEKUNUTS_PLANT_CLUT_X, OBJ_DEKUNUTS_PLANT_CLUT_Y);
  obj_dekunuts_plant_model->material[0].tpage = getTPage(0, 1, OBJ_DEKUNUTS_PLANT_TEXTURE_X, OBJ_DEKUNUTS_PLANT_TEXTURE_Y);
  SGM2_OffsetTexCoords(obj_dekunuts_plant_model, 0, 0);

  obj_dekunuts_body_model = (struct SGM2 *) SGM2_LoadFile((u_long*)dekunuts_body_static_sgm2);
  obj_dekunuts_body_model->material[0].clut = GetClut(OBJ_DEKUNUTS_PLANT_CLUT_X, OBJ_DEKUNUTS_PLANT_CLUT_Y-1);
  obj_dekunuts_body_model->material[0].tpage = getTPage(0, 1, OBJ_DEKUNUTS_BODY_TEXTURE_X, OBJ_DEKUNUTS_BODY_TEXTURE_Y);
  obj_dekunuts_body_model->material[1].clut = GetClut(OBJ_DEKUNUTS_BODY_CLUT_X, OBJ_DEKUNUTS_BODY_CLUT_Y);
  obj_dekunuts_body_model->material[1].tpage = getTPage(0, 1, OBJ_DEKUNUTS_BODY_TEXTURE_X, OBJ_DEKUNUTS_BODY_TEXTURE_Y);
  obj_dekunuts_body_model->material[2].clut = GetClut(OBJ_DEKUNUTS_PLANT_CLUT_X, OBJ_DEKUNUTS_PLANT_CLUT_Y);
  obj_dekunuts_body_model->material[2].tpage = getTPage(0, 1, OBJ_DEKUNUTS_PLANT_TEXTURE_X, OBJ_DEKUNUTS_PLANT_TEXTURE_Y);
  SGM2_OffsetTexCoords(obj_dekunuts_body_model, 0, 0);
  SGM2_OffsetMatTexCoords(obj_dekunuts_body_model, 1, 0, 64);

  obj_dekunuts_tpage = getTPage(0, 1, OBJ_DEKUNUTS_BODY_TEXTURE_X, OBJ_DEKUNUTS_BODY_TEXTURE_Y);
  obj_dekunuts_clut[0] = obj_dekunuts_body_model->material[0].clut;
  obj_dekunuts_clut[1] = obj_dekunuts_body_model->material[1].clut;
  obj_dekunuts_clut[2] = obj_dekunuts_body_model->material[2].clut;

  AGM_LoadModel(&obj_dekunuts_model, dekunuts_agm);
  ANM_LoadAnimation(&obj_dekunuts_anim, (u_char*)dekunuts_anm);

  AGM_OffsetTexByMaterial(&obj_dekunuts_model, 1, 0, 64);
}

void ObjDekunutsActorInitialize(struct Actor * a, void * descriptor, void * scene) {
  ObjDekunutsActor * actor = (ObjDekunutsActor *)a;
  Actor_Descriptor * desc = (Actor_Descriptor *)descriptor;
  actor->base.Initialize = (PTRFUNC_3ARG) ObjDekunutsActorInitialize;
  actor->base.Destroy = (PTRFUNC_2ARG) ObjDekunutsActorDestroy;
  actor->base.Update = (PTRFUNC_2ARG) ObjDekunutsActorUpdate;
  actor->base.Draw = (PTRFUNC_4ARGCHAR) ObjDekunutsActorDraw;
  actor->plant_model = obj_dekunuts_plant_model;
  actor->body_model = obj_dekunuts_body_model;
  actor->base.visible = 1;
  actor->base.flags = ACTOR_INTERACTABLE | ACTOR_ACCURATEDIST;
  actor->base.collisionData.radius = 115;
  actor->base.collisionData.height = 230;

  Actor_PopulateBase(a, descriptor);

  actor->matrix.t[0] = actor->base.pos.vx;// = desc->x;
  actor->matrix.t[1] = actor->base.pos.vy;// = desc->y;
  actor->matrix.t[2] = actor->base.pos.vz;// = desc->z;
  //actor->base.rot.vx = desc->rot_x;
  //actor->base.rot.vy = desc->rot_y;
  //actor->base.rot.vz = desc->rot_z;
  SVECTOR rot = actor->base.rot;
  RotMatrix_gte(&rot, &actor->matrix);
  actor->plant_matrix = actor->matrix;
}

void ObjDekunutsActorDestroy(struct Actor * a, void * scene) {
}

void ObjDekunutsActorUpdate(struct Actor * a, void * scene) {
  ObjDekunutsActor * actor = (ObjDekunutsActor *)a;
  struct Scene_Ctx * scene_ctx = (struct Scene_Ctx*)scene;
  Actor * player = scene_ctx->player;
  short draw_dist = scene_ctx->draw_dist;

  SVECTOR tdist = player->pos;
  tdist.vx -= actor->base.pos.vx;
  tdist.vy -= actor->base.pos.vy;
  tdist.vz -= actor->base.pos.vz;
  
  {
    // Look at player
    short look_angle = get_angleFromVectorXZ(&tdist);
    actor->base.rot.vy = look_angle;
    SVECTOR rot = actor->base.rot;
    RotMatrix_gte(&rot, &actor->matrix);
  }
  {
    int mask = (tdist.vx >> 31);
	  tdist.vx = (tdist.vx + mask) ^ mask;
    //mask = (tdist.vy >> 31);
	  //tdist.vy = (tdist.vy + mask) ^ mask;
    mask = (tdist.vz >> 31);
	  tdist.vz = (tdist.vz + mask) ^ mask;
  }
  if(tdist.vx < draw_dist && tdist.vz < draw_dist) {
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

  } else {

  }

  actor->anim++;
    
}

u_char * ObjDekunutsActorDraw(struct Actor * a, MATRIX * view, u_char * packet_ptr, void * scene) {
  ObjDekunutsActor * actor = (ObjDekunutsActor *)a;
  MATRIX local_identity;
  MATRIX temp;
  temp = actor->matrix;
  temp.t[1] += 49;
  CompMatrixLV(view, &temp, &local_identity);
  gte_SetRotMatrix(&local_identity);
  gte_SetTransMatrix(&local_identity);

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
  if(otzv > 8000) return packet_ptr;
  // Frustum culling
  if(vec0.vx < -128 || vec0.vx > SCREEN_W+128) return packet_ptr;
  if(vec0.vy < -128 || vec0.vy > SCREEN_H+128) return packet_ptr;

  

  //AGM_TransformBones(&obj_dekunuts_model, &local_identity, frameBuffer, player_bone_matrix);
  SVECTOR * anim = &obj_dekunuts_anim->animation_data[obj_dekunuts_anim->animation_list[1].start + (actor->anim % (obj_dekunuts_anim->animation_list[0].length-1)) * obj_dekunuts_anim->frame_size];
  AGM_TransformByBone(&obj_dekunuts_model, &temp, anim, view);
  //AGM_TransformModelOnly(&obj_dekunuts_model, player_bone_matrix, view);

  SetSpadStack(SPAD_STACK_ADDR);
  //packet_ptr = AGM_DrawModel(&obj_dekunuts_model, packet_ptr, (u_long*)G.pOt, 0);
  packet_ptr = AGM_DrawModelTPage(&obj_dekunuts_model, packet_ptr, (u_long*)G.pOt, 0, obj_dekunuts_tpage, obj_dekunuts_clut);
  ResetSpadStack();


  //SetSpadStack(SPAD_STACK_ADDR);
  //packet_ptr = SGM2_UpdateModel(actor->body_model, packet_ptr, (u_long*)G.pOt, 0, SGM2_RENDER_AMBIENT | SGM2_RENDER_NO_NCLIP, scene);
  //ResetSpadStack();

  CompMatrixLV(view, &actor->plant_matrix, &local_identity);
  gte_SetRotMatrix(&local_identity);
  gte_SetTransMatrix(&local_identity);

  SetSpadStack(SPAD_STACK_ADDR);
  packet_ptr = SGM2_UpdateModel(actor->plant_model, packet_ptr, (u_long*)G.pOt, 0, SGM2_RENDER_AMBIENT | SGM2_RENDER_NO_NCLIP, scene);
  ResetSpadStack();

  return packet_ptr;
}
