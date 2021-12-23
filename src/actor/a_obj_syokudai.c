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
struct SGM2 * obj_flame_model;
struct SGM2 * obj_flame_high_model;

// Screen Flare, 12 segments
#define SCREENFLARE_SEGMENTS 10
#define SCREENFLARE_SEGMENTS2 (SCREENFLARE_SEGMENTS * 2) + 1
#define SCREENFLARE_SEGMENTS_INNER SCREENFLARE_SEGMENTS * 3
#define SCREENFLARE_SEGMENTS_OUTTER SCREENFLARE_SEGMENTS * 4
#define SCREENFLARE_SEGMENT_ANG 4096 / SCREENFLARE_SEGMENTS
#define SCREENFLARE_INNER_RING_SCALE 2458
SVECTOR flare_verts[SCREENFLARE_SEGMENTS2];
u_char flare_index_outter_ring[SCREENFLARE_SEGMENTS_OUTTER];
u_char flare_index_inner[SCREENFLARE_SEGMENTS_INNER];

#define SCREENFLARE_FLICKER_FRAMES 90
u_char flicker_frame[SCREENFLARE_FLICKER_FRAMES];
#define SCREENFLARE_SIZE 19
#define SCREENFLARE_ENABLE_DITHERING 1

void ObjSyokudaiActorSetup() {
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

u_char * Draw_ScreenFlare(SVECTOR * pos, MATRIX * view, u_char * packet_ptr, void * scene, u_char flicker) {
  POLY_G4 * dest_pg4_ptr = (POLY_G4*) packet_ptr;
  POLY_G3 * dest_pg3_ptr;
  // Draw Outter Ring
  int step4 = 0;
  int step3 = 0;
  u_char flicker_level = flicker_frame[flicker];
  //CVECTOR flare_color = {60 - flicker_level, 44 - flicker_level, 30 - flicker_level};
  //CVECTOR flare_color = {60 - flicker_level, 47 - flicker_level, 30 - flicker_level};
  CVECTOR flare_color = {80 - flicker_level, 67 - flicker_level, 40 - flicker_level};


  SVECTOR dest = *pos;
  long zdepth;
  gte_SetRotMatrix(view);
  gte_SetTransMatrix(view);
  gte_ldv0(&dest);
  gte_rtps();
  gte_stsxy((long *)&dest);
  gte_stsz(&zdepth);
  if(zdepth <= 0) return packet_ptr;
  dest.vz = zdepth;
  if(dest.vx < 0 || dest.vx > SCREEN_W) return packet_ptr;
  if(dest.vy < 0 || dest.vy > SCREEN_H) return packet_ptr;
  dest.vx -= SCREEN_W/2;
  dest.vy -= SCREEN_H/2;
  
  
  struct Scene_Ctx * scene_ctx = (struct Scene_Ctx*)scene;
  struct Camera * camera = scene_ctx->camera;
  long temp0 = (((camera->aspect.vx * camera->fov_s) / dest.vz) * SCREENFLARE_SIZE) >> 12;
  long temp1 = (((camera->aspect.vy * camera->fov_s) / dest.vz) * SCREENFLARE_SIZE) >> 12;
  MATRIX temp = {
    {
      temp0, 0, 0,
      0, temp1, 0,
      0, 0, 4096
    },
    {dest.vx, dest.vy, 256}
  };

  gte_SetRotMatrix(&temp);
  gte_SetTransMatrix(&temp);

  for(int i = 0; i < SCREENFLARE_SEGMENTS; i++) {
    *(long*)&dest_pg4_ptr->r0 = 0;
    *(long*)&dest_pg4_ptr->r1 = 0;
    *(long*)&dest_pg4_ptr->r2 = *(long*)(&flare_color);
    *(long*)&dest_pg4_ptr->r3 = *(long*)(&flare_color);
    setPolyG4(dest_pg4_ptr);
    setSemiTrans(dest_pg4_ptr, 1);
    gte_ldv3(
        (SVECTOR*)&flare_verts[flare_index_outter_ring[step4]],
        (SVECTOR*)&flare_verts[flare_index_outter_ring[step4+1]],
        (SVECTOR*)&flare_verts[flare_index_outter_ring[step4+2]]
      );
    gte_rtpt();
    gte_stsxy3((long *)&dest_pg4_ptr->x0,(long *)&dest_pg4_ptr->x1,(long *)&dest_pg4_ptr->x2);
    gte_ldv0((SVECTOR*)&flare_verts[flare_index_outter_ring[step4+3]]);
    gte_rtps();
    gte_stsxy((long *)&dest_pg4_ptr->x3);

    addPrim(G.pOt+1, dest_pg4_ptr);
    dest_pg4_ptr++;
    step4+=4;

    dest_pg3_ptr = (POLY_G3*)dest_pg4_ptr;
    *(long*)&dest_pg3_ptr->r0 = *(long*)(&flare_color);
    *(long*)&dest_pg3_ptr->r1 = *(long*)(&flare_color);
    *(long*)&dest_pg3_ptr->r2 = *(long*)(&flare_color);
    setPolyG3(dest_pg3_ptr);
    setSemiTrans(dest_pg3_ptr, 1);
    gte_ldv3(
        (SVECTOR*)&flare_verts[flare_index_inner[step3]],
        (SVECTOR*)&flare_verts[flare_index_inner[step3+1]],
        (SVECTOR*)&flare_verts[flare_index_inner[step3+2]]
      );
    gte_rtpt();
    gte_stsxy3((long *)&dest_pg3_ptr->x0,(long *)&dest_pg3_ptr->x1,(long *)&dest_pg3_ptr->x2);
    addPrim(G.pOt+1, dest_pg3_ptr);
    dest_pg3_ptr++;
    dest_pg4_ptr = (POLY_G4*)dest_pg3_ptr;
    step3+=3;
  }

  packet_ptr = (u_char *)dest_pg3_ptr;

  setDrawTPage((DR_TPAGE*)packet_ptr, 1, SCREENFLARE_ENABLE_DITHERING, getTPage(0, 1, 0, 0));
  addPrim(G.pOt+1, packet_ptr);
  packet_ptr += sizeof(DR_TPAGE);

  return packet_ptr;
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
  actor->flame = obj_flame_model;
  actor->flame_high = obj_flame_high_model;
  actor->flame_color = 1;
  actor->flame_rand = rand() >> 9;
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
  
  actor->flame_matrix.t[0] = actor->base.pos.vx;
  actor->flame_matrix.t[1] = actor->base.pos.vy + 453;
  actor->flame_matrix.t[2] = actor->base.pos.vz;
  actor->flame_matrix.m[1][0] = 0;
  actor->flame_matrix.m[1][1] = 4096;
  actor->flame_matrix.m[1][2] = 0;

  actor->flicker_frame = rand() % SCREENFLARE_FLICKER_FRAMES;
}

void ObjSyokudaiActorDestroy(struct Actor * a, void * scene) {

}

void ObjSyokudaiActorUpdate(struct Actor * a, void * scene) {
  //SVECTOR local_rotate, camera_inverse, camera_inverse_cross;
  ObjSyokudaiActor * actor = (ObjSyokudaiActor *)a;
  Actor * player;
  struct Scene_Ctx * scene_ctx = (struct Scene_Ctx*)scene;
  player = scene_ctx->player;
  struct Camera * camera = scene_ctx->camera;
  actor->flame_matrix.m[0][0] = camera->zaxis.vx;
  actor->flame_matrix.m[0][1] = camera->zaxis.vy;
  actor->flame_matrix.m[0][2] = camera->zaxis.vz;
  actor->flame_matrix.m[2][0] = camera->zaxis.vz;
  actor->flame_matrix.m[2][1] = camera->zaxis.vy;
  actor->flame_matrix.m[2][2] = -camera->zaxis.vx;
  actor->flicker_frame++;
  actor->flicker_frame %= SCREENFLARE_FLICKER_FRAMES;

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


}

u_char * ObjSyokudaiActorDraw(struct Actor * a, MATRIX * view, u_char * packet_ptr, void * scene) {
  ObjSyokudaiActor * actor = (ObjSyokudaiActor *)a;
  MATRIX local_identity;
  CompMatrixLV(view, &actor->body_matrix, &local_identity);
  gte_SetRotMatrix(&local_identity);
  gte_SetTransMatrix(&local_identity);
  SetSpadStack(SPAD_STACK_ADDR);
  SVECTOR basepos = actor->base.pos;
  basepos.vy += 453 + 32;
  
  packet_ptr = SGM2_UpdateModel(actor->body, packet_ptr, (u_long*)G.pOt, 0, SGM2_RENDER_AMBIENT, scene);
  //ResetSpadStack();
  //SetSpadStack(SPAD_STACK_ADDR);
  packet_ptr = ObjSyokudaiActorDrawFlame(a, view, packet_ptr, scene);
  
  packet_ptr = Draw_ScreenFlare(&basepos, view, packet_ptr, scene, actor->flicker_frame);
  
  ResetSpadStack();

  return packet_ptr;
}


// Simplified SGM renderer for flames
u_char * ObjSyokudaiActorDrawFlame(struct Actor * a, MATRIX * view, u_char * packet_ptr, void * scene) {
  MATRIX local_identity;
  SVECTOR local_rotate; 
  ObjSyokudaiActor * actor = (ObjSyokudaiActor*)a;
  struct SGM2 * model = actor->flame;
  POLY_GT3 * dest_pgt3_ptr;
  SGM2_POLYGT3 * pgt3_ptr;
  u_int i;
  u_short tpageid, tpageido;
  u_short clutid, clutido;
  u_short pg_count;
  u_char v_offs = actor->flame_rand;

  CompMatrixLV(view, &actor->flame_matrix, &local_identity);

  /*{
  VECTOR scale = {4096*2, 4096*2, 4096*2, 0};
  ScaleMatrixL(&local_identity, &scale);
  }*/

  gte_SetRotMatrix(&local_identity);
  gte_SetTransMatrix(&local_identity);

  // Select LOD
  //if(actor->flame_high != NULL) {
  SVECTOR vec0, vec1;
  long otzv;
  vec0.vx = 0;
  vec0.vy = 0;
  vec0.vz = 0;
  gte_ldv0(&vec0);
  gte_rtps();
  gte_stsxy((long*)&vec1.vx);
  //gte_stotz(&otzv);
  gte_stsz(&otzv);
  // Z culling
  if(otzv < 0) return packet_ptr;
  // Frustum culling
  if(vec1.vx < -64 || vec1.vx > SCREEN_W+64) return packet_ptr;
  if(vec1.vy < -64 || vec1.vy > SCREEN_H+64) return packet_ptr;
  if(otzv < 1700) model = actor->flame_high;
  //}

  clutido = clutid = model->material[0].clut;
  tpageido = tpageid = model->material[0].tpage;

  clutid += (actor->flame_color << 6);
  tpageido = (tpageido & ~(1<<5)) | (2 << 5);
  

  // Transform to buffer
  AGM_TransformModel(model->vertex_data, model->vertex_count);

  pgt3_ptr = model->poly_gt3;

  dest_pgt3_ptr = (POLY_GT3*)packet_ptr;
  pg_count = model->poly_gt3_count;

  for(i = 0; i < pg_count; i++, pgt3_ptr++) {
      long outer_product, otz;
      long tempz0,tempz1,tempz2;
      SVECTOR * vec0 = &TransformBuffer[pgt3_ptr->idx0];
      SVECTOR * vec1 = &TransformBuffer[pgt3_ptr->idx1];
      SVECTOR * vec2 = &TransformBuffer[pgt3_ptr->idx2];
            
      // Load screen XY coordinates to GTE Registers
      gte_ldsxy3(*(long*)&vec0->vx,*(long*)&vec1->vx,*(long*)&vec2->vx);
      // Load Z coordinates into GTE
      tempz0 = vec0->vz;
      tempz1 = vec1->vz;
      tempz2 = vec2->vz;
      gte_ldsz3(tempz0,tempz1,tempz2);
      // Get the average Z value
      gte_avsz3();
      // Store value to otz
      gte_stotz(&otz);
      // Reduce OTZ size
      otz = (otz >> OTSUBDIV);
      //otz = 50;
      if (otz > OTMIN && otz < OTSIZE) {
        long temp0, temp1, temp2;
        // If all tests have passed, now process the rest of the primitive.
        // Copy values already in the registers
        gte_stsxy3((long*)&dest_pgt3_ptr->x0,
                   (long*)&dest_pgt3_ptr->x1,
                   (long*)&dest_pgt3_ptr->x2);
        // Vertex Colors
        temp0 = *(long*)(&pgt3_ptr->r0);
        temp1 = *(long*)(&pgt3_ptr->r1);
        temp2 = *(long*)(&pgt3_ptr->r2);
        *(long*)(&dest_pgt3_ptr->r0) = temp0;
        *(long*)(&dest_pgt3_ptr->r1) = temp1;
        *(long*)(&dest_pgt3_ptr->r2) = temp2;

        // Set Texture Coordinates
        dest_pgt3_ptr->u0 = pgt3_ptr->u0;
        dest_pgt3_ptr->v0 = pgt3_ptr->v0 + v_offs;
        dest_pgt3_ptr->u1 = pgt3_ptr->u1;
        dest_pgt3_ptr->v1 = pgt3_ptr->v1 + v_offs;
        dest_pgt3_ptr->u2 = pgt3_ptr->u2;
        dest_pgt3_ptr->v2 = pgt3_ptr->v2 + v_offs;
        
        // Set CLUT
        dest_pgt3_ptr->clut = clutid;
        // Set Texture Page
        dest_pgt3_ptr->tpage = tpageid;

        setPolyGT3(dest_pgt3_ptr);

        setSemiTrans(dest_pgt3_ptr, 1);
        
        addPrim(G.pOt+otz, dest_pgt3_ptr);
        /*POLY_GT3* dupli = dest_pgt3_ptr;
        dest_pgt3_ptr++;
        *dest_pgt3_ptr = *dupli;
        dest_pgt3_ptr->clut = clutido;
        dest_pgt3_ptr->tpage = tpageido;
        addPrim(G.pOt+otz, dest_pgt3_ptr);*/
        dest_pgt3_ptr++;

      }
  }

  return (char*)dest_pgt3_ptr;
}