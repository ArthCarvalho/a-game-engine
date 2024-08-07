/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#include "model/sgm2.h"

#include <inline_n.h>
#include <gtemac.h>

#define SGM2_SUBDIV_LV1 180
#define SGM2_SUBDIV_LV2 60

#define SGM2_SUBDIV_HIGH_LV1 400
#define SGM2_SUBDIV_HIGH_LV2 100

#define SGM2_FOG_SHIFT 6
#define SGM2_FOG_MAX 16

SGM2_File * SGM2_LoadFile(u_long * addr) {
  // Parse the offsets inside the file and change them to memory addresses instead of offsets.
  SGM2_File * dest = (SGM2_File *)addr;

  ////printf("Loading SGM2\n");
  ////printf("vtx num: %d normal num: %d color num: %d mat num: %d\n",dest->vertex_count, dest->normal_count, dest->color_count, dest->mat_count);
  ////printf("g4 %d g3 %d gt4 %d gt3 %d\n",dest->poly_g4_count, dest->poly_g3_count, dest->poly_gt4_count, dest->poly_gt3_count);
  ////printf("g4 %x g3 %x gt4 %x gt3 %x\n",dest->poly_g4, dest->poly_g3, dest->poly_gt4, dest->poly_gt3);

  dest->vertex_data = (SVECTOR*)((char*)addr + ((long)dest->vertex_data));
  dest->normal_data = (SVECTOR*)((char*)addr + ((long)dest->normal_data));
  //dest->color_data = (SGM2_COLOR*)((char*)addr + ((long)dest->color_data));
  dest->material = (SGM2_MATERIAL*)((char*)addr + ((long)dest->material));
  dest->poly_g4 = (SGM2_POLYG4*)((char*)addr + ((long)dest->poly_g4));
  dest->poly_g3 = (SGM2_POLYG3*)((char*)addr + ((long)dest->poly_g3));
  dest->poly_gt4 = (SGM2_POLYGT4*)((char*)addr + ((long)dest->poly_gt4));
  dest->poly_gt3 = (SGM2_POLYGT3*)((char*)addr + ((long)dest->poly_gt3));

  SGM2_POLYGT4 * gt4itr = dest->poly_gt4;
  for(int i = 0; i < dest->poly_gt4_count;i++) {
    // Change this in exporter later
    gt4itr->material = gt4itr->code;
    gt4itr->code = 0x3c;
    gt4itr++;
  }

  SGM2_POLYGT3 * gt3itr = dest->poly_gt3;
  for(int i = 0; i < dest->poly_gt3_count;i++) {
    // Change this in exporter later
    gt3itr->material = gt3itr->code;
    gt3itr->code = 0x34;
    gt3itr++;
  }

  ////printf("g4 %x g3 %x gt4 %x gt3 %x\n",dest->poly_g4, dest->poly_g3, dest->poly_gt4, dest->poly_gt3);
  return dest;
}

u_char * SGM2_UpdateModel(SGM2_File * model, u_char * packet_ptr, u_long * ot, short depth_offset, u_int flags, void * scene) {
  // Static, temporary tpage and clut
  POLY_GT4 * dest_pgt4_ptr;
  POLY_GT3 * dest_pgt3_ptr;
  SGM2_POLYGT4 * pgt4_ptr;
  SGM2_POLYGT3 * pgt3_ptr;
  u_int i;
  u_long tpageid;
  u_long clutid;
  u_long pg_count;
  u_short current_mat;
  u_short last_mat;
  DIVPOLYGON4 * divprop = (DIVPOLYGON4 *)(SCRATCH_PAD+0xC0);
  DIVPOLYGON3 * divprop3 = (DIVPOLYGON3 *)(SCRATCH_PAD+0xC0);
  Scene_Ctx * scenectx = (Scene_Ctx*)scene;
  u_char ambient_r = scenectx->ambient.r; // >> 7
  u_char ambient_g = scenectx->ambient.g; // >> 7
  u_char ambient_b = scenectx->ambient.b; // >> 7

  u_short subdiv_lvl1 = SGM2_SUBDIV_LV1;
  u_short subdiv_lvl2 = SGM2_SUBDIV_LV2;
  u_char subdiv_n1 = 1;
  u_char subdiv_n2 = 2;

  if(flags & SGM2_RENDER_SUBDIV_HIGH) {
    subdiv_lvl1 = SGM2_SUBDIV_HIGH_LV1;
    subdiv_lvl2 = SGM2_SUBDIV_HIGH_LV2;
    subdiv_n1 = 2;
    subdiv_n2 = 3;
  }
  

  divprop->pih = SCREEN_W;
  divprop->piv = SCREEN_H;
  divprop->ndiv = 1;
  //DIVPOLYGON4 divprop;
  //divprop.pih = SCREEN_W;
  //divprop.piv = SCREEN_H;
  //divprop.ndiv = 1;

  //tpageid = getTPage(1, 0, 0,0);
  //clutid = GetClut(TEST_CLX,TEST_CLY);
  /*
  clutid = (u_long)(model->material[0].clut + ((flags & SGM2_RENDER_BUMPCLUT) << 3)) << 16;
  tpageid = (u_long)model->material[0].tpage << 16;*/

  //clutid = model->material[0].clut;
  //tpageid = model->material[0].tpage;

  // Transform to buffer
  AGM_TransformModel(model->vertex_data, model->vertex_count);

  dest_pgt4_ptr = (POLY_GT4*)packet_ptr;
  pgt4_ptr = model->poly_gt4;
  pgt3_ptr = model->poly_gt3;
  pg_count = model->poly_gt4_count;

  for(i = 0; i < pg_count; i++, pgt4_ptr++) {
    long outer_product, otz;
    long tempz0,tempz1,tempz2,tempz3;
    SVECTOR * vec0 = &AGM_TransformBuffer[pgt4_ptr->idx0];
    SVECTOR * vec1 = &AGM_TransformBuffer[pgt4_ptr->idx1];
    SVECTOR * vec2 = &AGM_TransformBuffer[pgt4_ptr->idx2];
    SVECTOR * vec3 = &AGM_TransformBuffer[pgt4_ptr->idx3];

    clutid = (u_long)(model->material[pgt4_ptr->material].clut + ((flags & SGM2_RENDER_BUMPCLUT) << 3)) << 16;
    tpageid = (u_long)model->material[pgt4_ptr->material].tpage << 16;

    // Load screen XY coordinates to GTE Registers
    gte_ldsxy3(*(long*)&vec0->vx,*(long*)&vec1->vx,*(long*)&vec2->vx);

    // Perform clipping calculation
    if(!(flags & SGM2_RENDER_NO_NCLIP)){
      gte_nclip();
      // Store Outer Product
      gte_stopz(&outer_product);
      // Check side
      if(outer_product <= 0) continue; // Skip back facing polys    
    }

    // Load Z coordinates into GTE
    tempz0 = vec0->vz;
    tempz1 = vec1->vz;
    tempz2 = vec2->vz;
    tempz3 = vec3->vz;
    ////printf("Poly %d z: %d, %d, %d, %d\n",i,tempz0,tempz1,tempz2,tempz3);
    gte_ldsz4(tempz0,tempz1,tempz2,tempz3);
    // Get the average Z value
    gte_avsz4();
    // Store value to otz
    gte_stotz(&otz);
    // Reduce OTZ size
    otz = (otz >> OTSUBDIV);
    if(otz < 1) continue;
    
    otz += depth_offset;
    //otz = 50;
    //if(otz >= OTSIZE) otz = OTSIZE-1;

    if (otz < OTSIZE) {
      if(flags & SGM2_RENDER_SCREEN_CLIP) {
        if(quad_clip( (DVECTOR*)&vec0->vx,
                    (DVECTOR*)&vec1->vx,
                    (DVECTOR*)&vec2->vx,
                    (DVECTOR*)&vec3->vx)) continue;
      }
      if(otz < subdiv_lvl1 && flags & SGM2_RENDER_SUBDIV) {
        u_long uvcode0, uvcode1, uvcode2, uvcode3;
        CVECTOR col0, col1, col2, col3;
        //CVECTOR colcode;
        
        if(otz < subdiv_lvl2) {
          divprop->ndiv = subdiv_n2;
        } else {
          divprop->ndiv = subdiv_n1;
        }
        
        vec0 = &model->vertex_data[pgt4_ptr->idx0];
        vec1 = &model->vertex_data[pgt4_ptr->idx1];
        vec2 = &model->vertex_data[pgt4_ptr->idx2];
        vec3 = &model->vertex_data[pgt4_ptr->idx3];
        uvcode0 = *(u_short*)&pgt4_ptr->u0;
        uvcode1 = *(u_short*)&pgt4_ptr->u1;
        uvcode2 = *(u_short*)&pgt4_ptr->u2;
        uvcode3 = *(u_short*)&pgt4_ptr->u3;
        uvcode0 = (uvcode0 & 0xFFFF) | clutid;
        uvcode1 = (uvcode1 & 0xFFFF) | tpageid;

        if(flags & SGM2_RENDER_AMBIENT) { // !!! TODO - Change this to use GTE/BackLight !!!
          gte_ldrgb((CVECTOR*)&pgt4_ptr->r0);
          gte_cc();
          gte_strgb((CVECTOR*)&col0);
          gte_ldrgb((CVECTOR*)&pgt4_ptr->r1);
          gte_cc();
          gte_strgb((CVECTOR*)&col1);
          gte_ldrgb((CVECTOR*)&pgt4_ptr->r2);
          gte_cc();
          gte_strgb((CVECTOR*)&col2);
          gte_ldrgb((CVECTOR*)&pgt4_ptr->r3);
          gte_cc();
          gte_strgb((CVECTOR*)&col3);
        } else {
          col0 = *(CVECTOR*)&pgt4_ptr->r0;
          col1 = *(CVECTOR*)&pgt4_ptr->r1;
          col2 = *(CVECTOR*)&pgt4_ptr->r2;
          col3 = *(CVECTOR*)&pgt4_ptr->r3;
        }

        dest_pgt4_ptr = (POLY_GT4 *)DivideGT4(
        vec0, vec1, vec2, vec3,
        &uvcode0, &uvcode1, &uvcode2, &uvcode3, 
        (CVECTOR*)&col0, (CVECTOR*)&col1, (CVECTOR*)&col2, (CVECTOR*)&col3,
        dest_pgt4_ptr, (u_long *)(ot+otz), divprop);
        } else {
        u_long temp0, temp1, temp2, temp3;
        // If all tests have passed, now process the rest of the primitive.
        // Copy values already in the registers
        gte_stsxy3((long*)&dest_pgt4_ptr->x0,
                    (long*)&dest_pgt4_ptr->x1,
                    (long*)&dest_pgt4_ptr->x2);
        // Store 4th vertex from buffer directly
        temp0 = *(long*)(&vec3->vx);
        *(long*)(&dest_pgt4_ptr->x3) = temp0;
        // Vertex Colors
        /*temp1 = *(long*)(&pgt4_ptr->r0);
        temp2 = *(long*)(&pgt4_ptr->r1);
        temp3 = *(long*)(&pgt4_ptr->r2);
        temp0 = *(long*)(&pgt4_ptr->r3);
        *(long*)(&dest_pgt4_ptr->r0) = temp1;
        *(long*)(&dest_pgt4_ptr->r1) = temp2;
        *(long*)(&dest_pgt4_ptr->r2) = temp3;
        *(long*)(&dest_pgt4_ptr->r3) = temp0;*/
        if(flags & SGM2_RENDER_AMBIENT) { // !!! TODO - Change this to use GTE/BackLight !!!
          gte_ldrgb((CVECTOR*)&pgt4_ptr->r0);
          gte_cc();
          gte_strgb((CVECTOR*)&dest_pgt4_ptr->r0);
          gte_ldrgb((CVECTOR*)&pgt4_ptr->r1);
          gte_cc();
          gte_strgb((CVECTOR*)&dest_pgt4_ptr->r1);
          gte_ldrgb((CVECTOR*)&pgt4_ptr->r2);
          gte_cc();
          gte_strgb((CVECTOR*)&dest_pgt4_ptr->r2);
          gte_ldrgb((CVECTOR*)&pgt4_ptr->r3);
          gte_cc();
          gte_strgb((CVECTOR*)&dest_pgt4_ptr->r3);
          //gte_ldrgb3((CVECTOR*)&pgt4_ptr->r0, (CVECTOR*)&pgt4_ptr->r1, (CVECTOR*)&pgt4_ptr->r2);
          //gte_n
          //gte_strgb3_g4(dest_pgt4_ptr);
          /*dest_pgt4_ptr->r0 = (pgt4_ptr->r0 * ambient_r) >> 7;
          dest_pgt4_ptr->g0 = (pgt4_ptr->g0 * ambient_g) >> 7;
          dest_pgt4_ptr->b0 = (pgt4_ptr->b0 * ambient_b) >> 7;
          dest_pgt4_ptr->r1 = (pgt4_ptr->r1 * ambient_r) >> 7;
          dest_pgt4_ptr->g1 = (pgt4_ptr->g1 * ambient_g) >> 7;
          dest_pgt4_ptr->b1 = (pgt4_ptr->b1 * ambient_b) >> 7;
          dest_pgt4_ptr->r2 = (pgt4_ptr->r2 * ambient_r) >> 7;
          dest_pgt4_ptr->g2 = (pgt4_ptr->g2 * ambient_g) >> 7;
          dest_pgt4_ptr->b2 = (pgt4_ptr->b2 * ambient_b) >> 7;
          dest_pgt4_ptr->r3 = (pgt4_ptr->r3 * ambient_r) >> 7;
          dest_pgt4_ptr->g3 = (pgt4_ptr->g3 * ambient_g) >> 7;
          dest_pgt4_ptr->b3 = (pgt4_ptr->b3 * ambient_b) >> 7;*/
        } else {
          temp1 = *(long*)(&pgt4_ptr->r0);
          temp2 = *(long*)(&pgt4_ptr->r1);
          temp3 = *(long*)(&pgt4_ptr->r2);
          temp0 = *(long*)(&pgt4_ptr->r3);
          *(long*)(&dest_pgt4_ptr->r0) = temp1;
          *(long*)(&dest_pgt4_ptr->r1) = temp2;
          *(long*)(&dest_pgt4_ptr->r2) = temp3;
          *(long*)(&dest_pgt4_ptr->r3) = temp0;
        }

        if(flags & SGM2_RENDER_CLUTFOG){
          long fog = (otz-512) >> SGM2_FOG_SHIFT;
          if(fog < 0) fog = 0;
          if(fog > SGM2_FOG_MAX-1) fog = SGM2_FOG_MAX-1;
          clutid = clutid + (fog<<(6+16));
        }
        
        // Texture Coordinates
        // Set Texture Coordinates
        temp1 = *(short*)(&pgt4_ptr->u0);
        temp2 = *(short*)(&pgt4_ptr->u1);
        temp3 = *(short*)(&pgt4_ptr->u2);
        temp0 = *(short*)(&pgt4_ptr->u3);
        *(u_long*)(&dest_pgt4_ptr->u0) = (temp1 & 0xFFFF) | clutid;
        *(u_long*)(&dest_pgt4_ptr->u1) = (temp2 & 0xFFFF) | tpageid;
        *(short*)(&dest_pgt4_ptr->u2) = temp3;
        *(short*)(&dest_pgt4_ptr->u3) = temp0;

        // Set CLUT
        //dest_pgt4_ptr->clut = clutid;
        // Set Texture Page
        //dest_pgt4_ptr->tpage = tpageid;

        setPolyGT4(dest_pgt4_ptr);
        setSemiTrans(dest_pgt4_ptr, flags & SGM2_RENDER_ABE);
      
        // Add primitive to Ordering Table and advance pointer
        // to point to the next primitive to be processed.
        addPrim(ot+otz, dest_pgt4_ptr);
        dest_pgt4_ptr++;
      }
    }
  }

  dest_pgt3_ptr = (POLY_GT3*)dest_pgt4_ptr;
  pg_count = model->poly_gt3_count;

  for(i = 0; i < pg_count; i++, pgt3_ptr++) {
      long outer_product, otz;
      long tempz0,tempz1,tempz2;
      SVECTOR * vec0 = &AGM_TransformBuffer[pgt3_ptr->idx0];
      SVECTOR * vec1 = &AGM_TransformBuffer[pgt3_ptr->idx1];
      SVECTOR * vec2 = &AGM_TransformBuffer[pgt3_ptr->idx2];

      clutid = (u_long)(model->material[pgt3_ptr->material].clut + ((flags & SGM2_RENDER_BUMPCLUT) << 3)) << 16;
      tpageid = (u_long)model->material[pgt3_ptr->material].tpage << 16;
      
      // Load screen XY coordinates to GTE Registers
      gte_ldsxy3(*(long*)&vec0->vx,*(long*)&vec1->vx,*(long*)&vec2->vx);
      // Perform clipping calculation
      if(!(flags & SGM2_RENDER_NO_NCLIP)){
        gte_nclip();
        // Store Outer Product
        gte_stopz(&outer_product);
        // Check side
        if(outer_product <= 0) continue; // Skip back facing polys
      }
      ////printf("rendering face %d\n",i);
      // Load Z coordinates into GTE
      tempz0 = vec0->vz;
      tempz1 = vec1->vz;
      tempz2 = vec2->vz;
      ////printf("Poly %d z: %d, %d, %d, %d\n",i,tempz0,tempz1,tempz2,tempz3);
      gte_ldsz3(tempz0,tempz1,tempz2);
      // Get the average Z value
      gte_avsz3();
      // Store value to otz
      gte_stotz(&otz);
      // Reduce OTZ size
      otz = (otz >> OTSUBDIV)+depth_offset;
      //otz = 50;
      if(otz >= OTSIZE) otz = OTSIZE-1;
      if (otz > OTMIN) {
        // Clip quads that are offscreen before any other operation
        if(flags & SGM2_RENDER_SCREEN_CLIP) {
          if(tri_clip((DVECTOR*)vec0,
                    (DVECTOR*)vec1,
                    (DVECTOR*)vec2)) continue;
        }
        
        if(otz < subdiv_lvl1 && flags & SGM2_RENDER_SUBDIV) {
          u_long uvcode0, uvcode1, uvcode2;
          CVECTOR col0, col1, col2;
          if(otz < subdiv_lvl2) {
            divprop->ndiv = subdiv_n2;
          } else {
            divprop->ndiv = subdiv_n1;
          }

          vec0 = &model->vertex_data[pgt3_ptr->idx0];
          vec1 = &model->vertex_data[pgt3_ptr->idx1];
          vec2 = &model->vertex_data[pgt3_ptr->idx2];
          uvcode0 = *(u_short*)&pgt3_ptr->u0;
          uvcode1 = *(u_short*)&pgt3_ptr->u1;
          uvcode2 = *(u_short*)&pgt3_ptr->u2;
          uvcode0 = (uvcode0 & 0xFFFF) | clutid;
          uvcode1 = (uvcode1 & 0xFFFF) | tpageid;

          if(flags & SGM2_RENDER_AMBIENT) {
            gte_ldrgb((CVECTOR*)&pgt3_ptr->r0);
            gte_cc();
            gte_strgb((CVECTOR*)&col0);
            gte_ldrgb((CVECTOR*)&pgt3_ptr->r1);
            gte_cc();
            gte_strgb((CVECTOR*)&col1);
            gte_ldrgb((CVECTOR*)&pgt3_ptr->r2);
            gte_cc();
            gte_strgb((CVECTOR*)&col2);
          } else {
            col0 = *(CVECTOR*)&pgt3_ptr->r0;
            col1 = *(CVECTOR*)&pgt3_ptr->r1;
            col2 = *(CVECTOR*)&pgt3_ptr->r2;
          }

          dest_pgt3_ptr = (POLY_GT3 *)DivideGT3(
          vec0, vec1, vec2,
          &uvcode0, &uvcode1, &uvcode2, 
          (CVECTOR*)&col0, (CVECTOR*)&col1, (CVECTOR*)&col2,
          dest_pgt3_ptr, (u_long *)(ot+otz), divprop3);
        } else {
          u_long temp0, temp1, temp2;
          // If all tests have passed, now process the rest of the primitive.
          // Copy values already in the registers
          gte_stsxy3((long*)&dest_pgt3_ptr->x0,
                    (long*)&dest_pgt3_ptr->x1,
                    (long*)&dest_pgt3_ptr->x2);
          // Vertex Colors
          /*temp0 = *(u_long*)(&pgt3_ptr->r0);
          temp1 = *(u_long*)(&pgt3_ptr->r1);
          temp2 = *(u_long*)(&pgt3_ptr->r2);
          *(u_long*)(&dest_pgt3_ptr->r0) = temp0;
          *(u_long*)(&dest_pgt3_ptr->r1) = temp1;
          *(u_long*)(&dest_pgt3_ptr->r2) = temp2;*/

          if(flags & SGM2_RENDER_AMBIENT) { // !!! TODO - Change this to use GTE/BackLight !!!
            gte_ldrgb((CVECTOR*)&pgt3_ptr->r0);
            gte_cc();
            gte_strgb((CVECTOR*)&dest_pgt3_ptr->r0);
            gte_ldrgb((CVECTOR*)&pgt3_ptr->r1);
            gte_cc();
            gte_strgb((CVECTOR*)&dest_pgt3_ptr->r1);
            gte_ldrgb((CVECTOR*)&pgt3_ptr->r2);
            gte_cc();
            gte_strgb((CVECTOR*)&dest_pgt3_ptr->r2);
          } else {
            temp0 = *(u_long*)(&pgt3_ptr->r0);
            temp1 = *(u_long*)(&pgt3_ptr->r1);
            temp2 = *(u_long*)(&pgt3_ptr->r2);
            *(u_long*)(&dest_pgt3_ptr->r0) = temp0;
            *(u_long*)(&dest_pgt3_ptr->r1) = temp1;
            *(u_long*)(&dest_pgt3_ptr->r2) = temp2;
          }

          if(flags & SGM2_RENDER_CLUTFOG){
            long fog = (otz-512) >> SGM2_FOG_SHIFT;
            if(fog < 0) fog = 0;
            if(fog > SGM2_FOG_MAX-1) fog = SGM2_FOG_MAX-1;
            clutid = clutid + (fog<<(6+16));
          }

          // Set Texture Coordinates
          temp0 = *(u_short*)(&pgt3_ptr->u0);
          temp1 = *(u_short*)(&pgt3_ptr->u1);
          temp2 = *(u_short*)(&pgt3_ptr->u2);
          *(u_long*)(&dest_pgt3_ptr->u0) = (temp0 & 0xFFFF) | clutid;
          *(u_long*)(&dest_pgt3_ptr->u1) = (temp1 & 0xFFFF) | tpageid;
          *(u_short*)(&dest_pgt3_ptr->u2) = temp2;
          
          // Set CLUT
          //dest_pgt3_ptr->clut = clutid;

          // Set Texture Page
          //dest_pgt3_ptr->tpage = tpageid;

          setPolyGT3(dest_pgt3_ptr);

          setSemiTrans(dest_pgt3_ptr, flags & SGM2_RENDER_ABE);
          
          // Add primitive to Ordering Table and advance pointer
          // to point to the next primitive to be processed.
          addPrim(ot+otz, dest_pgt3_ptr);
          dest_pgt3_ptr++;
        }
      }
  }

  return (char*)dest_pgt3_ptr;
}


void SGM2_OffsetTexCoords(SGM2_File * model, short x, short y) {
  u_int i;
  SGM2_POLYGT4 * pgt4_ptr;
  SGM2_POLYGT3 * pgt3_ptr;
  u_int pg_count;

  pgt4_ptr = model->poly_gt4;
  pgt3_ptr = model->poly_gt3;
  pg_count = model->poly_gt4_count;

  for(i = 0; i < pg_count; i++, pgt4_ptr++) {
    pgt4_ptr->u0 += x;
    pgt4_ptr->v0 += y;
    pgt4_ptr->u1 += x;
    pgt4_ptr->v1 += y;
    pgt4_ptr->u2 += x;
    pgt4_ptr->v2 += y;
    pgt4_ptr->u3 += x;
    pgt4_ptr->v3 += y;
  }
  pg_count = model->poly_gt3_count;
  for(i = 0; i < pg_count; i++, pgt3_ptr++) {
    pgt3_ptr->u0 += x;
    pgt3_ptr->v0 += y;
    pgt3_ptr->u1 += x;
    pgt3_ptr->v1 += y;
    pgt3_ptr->u2 += x;
    pgt3_ptr->v2 += y;
  }
}

void SGM2_OffsetMatTexCoords(SGM2_File * model, u_short mat, short x, short y) {
  u_int i;
  SGM2_POLYGT4 * pgt4_ptr;
  SGM2_POLYGT3 * pgt3_ptr;
  u_int pg_count;

  pgt4_ptr = model->poly_gt4;
  pgt3_ptr = model->poly_gt3;
  pg_count = model->poly_gt4_count;

  for(i = 0; i < pg_count; i++, pgt4_ptr++) {
    if(pgt4_ptr->material != mat) continue;
    pgt4_ptr->u0 += x;
    pgt4_ptr->v0 += y;
    pgt4_ptr->u1 += x;
    pgt4_ptr->v1 += y;
    pgt4_ptr->u2 += x;
    pgt4_ptr->v2 += y;
    pgt4_ptr->u3 += x;
    pgt4_ptr->v3 += y;
  }
  pg_count = model->poly_gt3_count;
  for(i = 0; i < pg_count; i++, pgt3_ptr++) {
    if(pgt3_ptr->material != mat) continue;
    pgt3_ptr->u0 += x;
    pgt3_ptr->v0 += y;
    pgt3_ptr->u1 += x;
    pgt3_ptr->v1 += y;
    pgt3_ptr->u2 += x;
    pgt3_ptr->v2 += y;
  }
}


void SGM2_GenerateReflectionUV(SGM2_File * model, MATRIX * model_view, int x_offset, int y_offset) {
  SVECTOR * transform_uv;
  SVECTOR * normal_ptr;
  SGM2_POLYGT4 * pgt4_ptr;
  SGM2_POLYGT3 * pgt3_ptr;

  normal_ptr = model->normal_data;
  pgt4_ptr = model->poly_gt4;
  pgt3_ptr = model->poly_gt3;
  transform_uv = AGM_TransformBuffer;

  gte_SetRotMatrix(model_view);
  // Batch transform normals
  for(int i = 0; i < model->normal_count; i++, normal_ptr++, transform_uv++) {
    VECTOR	result;
    SVECTOR final;

    //gte_ApplyMatrix(model_view, normal_ptr, &result);

    gte_ldv0(normal_ptr);
    gte_rtv0();
    gte_stlvnl(&result);

    //final.vx = (-result.vx >> SGM2_REFLECTION_SHIFT)+1;
    //final.vy = (-result.vy >> SGM2_REFLECTION_SHIFT)+1;
    final.vx = -result.vx;
    final.vy = -result.vy;
    // Copy over to temporary buffer
    //*transform_uv = final;
    AGM_TransformBuffer[i] = final;
  }

  // Apply normals to UV coordinates
  /*for(int i = 0; i < model->poly_gt4_count; i++, pgt4_ptr++) {
    pgt4_ptr->u0 = SGM2_REFLECTION_SIZE - (AGM_TransformBuffer[pgt4_ptr->idx0].vx) + x_offset;
    pgt4_ptr->v0 = SGM2_REFLECTION_SIZE - (AGM_TransformBuffer[pgt4_ptr->idx0].vy) + y_offset;

    pgt4_ptr->u1 = SGM2_REFLECTION_SIZE - (AGM_TransformBuffer[pgt4_ptr->idx1].vx) + x_offset;
    pgt4_ptr->v1 = SGM2_REFLECTION_SIZE - (AGM_TransformBuffer[pgt4_ptr->idx1].vy) + y_offset;

    pgt4_ptr->u2 = SGM2_REFLECTION_SIZE - (AGM_TransformBuffer[pgt4_ptr->idx2].vx) + x_offset;
    pgt4_ptr->v2 = SGM2_REFLECTION_SIZE - (AGM_TransformBuffer[pgt4_ptr->idx2].vy) + y_offset;

    pgt4_ptr->u3 = SGM2_REFLECTION_SIZE - (AGM_TransformBuffer[pgt4_ptr->idx3].vx) + x_offset;
    pgt4_ptr->v3 = SGM2_REFLECTION_SIZE - (AGM_TransformBuffer[pgt4_ptr->idx3].vy) + y_offset;
  }
  for(int i = 0; i < model->poly_gt3_count; i++, pgt3_ptr++) {
    pgt3_ptr->u0 = SGM2_REFLECTION_SIZE - (AGM_TransformBuffer[pgt3_ptr->idx0].vx) + x_offset;
    pgt3_ptr->v0 = SGM2_REFLECTION_SIZE - (AGM_TransformBuffer[pgt3_ptr->idx0].vy) + y_offset;

    pgt3_ptr->u1 = SGM2_REFLECTION_SIZE - (AGM_TransformBuffer[pgt3_ptr->idx1].vx) + x_offset;
    pgt3_ptr->v1 = SGM2_REFLECTION_SIZE - (AGM_TransformBuffer[pgt3_ptr->idx1].vy) + y_offset;

    pgt3_ptr->u2 = SGM2_REFLECTION_SIZE - (AGM_TransformBuffer[pgt3_ptr->idx2].vx) + x_offset;
    pgt3_ptr->v2 = SGM2_REFLECTION_SIZE - (AGM_TransformBuffer[pgt3_ptr->idx2].vy) + y_offset;
  }
}*/

for(int i = 0; i < model->poly_gt4_count; i++, pgt4_ptr++) {
    pgt4_ptr->u0 = SGM2_REFLECTION_SIZE - (AGM_TransformBuffer[pgt4_ptr->n0].vx) + x_offset;
    pgt4_ptr->v0 = SGM2_REFLECTION_SIZE - (AGM_TransformBuffer[pgt4_ptr->n0].vy) + y_offset;

    pgt4_ptr->u1 = SGM2_REFLECTION_SIZE - (AGM_TransformBuffer[pgt4_ptr->n1].vx) + x_offset;
    pgt4_ptr->v1 = SGM2_REFLECTION_SIZE - (AGM_TransformBuffer[pgt4_ptr->n1].vy) + y_offset;

    pgt4_ptr->u2 = SGM2_REFLECTION_SIZE - (AGM_TransformBuffer[pgt4_ptr->n2].vx) + x_offset;
    pgt4_ptr->v2 = SGM2_REFLECTION_SIZE - (AGM_TransformBuffer[pgt4_ptr->n2].vy) + y_offset;

    pgt4_ptr->u3 = SGM2_REFLECTION_SIZE - (AGM_TransformBuffer[pgt4_ptr->n3].vx) + x_offset;
    pgt4_ptr->v3 = SGM2_REFLECTION_SIZE - (AGM_TransformBuffer[pgt4_ptr->n3].vy) + y_offset;
  }
  for(int i = 0; i < model->poly_gt3_count; i++, pgt3_ptr++) {
    pgt3_ptr->u0 = SGM2_REFLECTION_SIZE - (AGM_TransformBuffer[pgt3_ptr->n0].vx) + x_offset;
    pgt3_ptr->v0 = SGM2_REFLECTION_SIZE - (AGM_TransformBuffer[pgt3_ptr->n0].vy) + y_offset;

    pgt3_ptr->u1 = SGM2_REFLECTION_SIZE - (AGM_TransformBuffer[pgt3_ptr->n1].vx) + x_offset;
    pgt3_ptr->v1 = SGM2_REFLECTION_SIZE - (AGM_TransformBuffer[pgt3_ptr->n1].vy) + y_offset;

    pgt3_ptr->u2 = SGM2_REFLECTION_SIZE - (AGM_TransformBuffer[pgt3_ptr->n2].vx) + x_offset;
    pgt3_ptr->v2 = SGM2_REFLECTION_SIZE - (AGM_TransformBuffer[pgt3_ptr->n2].vy) + y_offset;
  }
}


/*
void SGM2_SubdivideGT3_Sub1() {
  // Calculate vertex subdivision
  // A -> B * 0.5
  short ab_half_x = (pgt3_ptr->x1 - pgt3_ptr->x0) >> 1;
  short ab_half_y = (pgt3_ptr->y1 - pgt3_ptr->y0) >> 1;
  short ab_half_z = (pgt3_ptr->z1 - pgt3_ptr->z0) >> 1;
  // B -> C * 0.5
  short bc_half_x = (pgt3_ptr->x2 - pgt3_ptr->x1) >> 1;
  short bc_half_y = (pgt3_ptr->y2 - pgt3_ptr->y1) >> 1;
  short bc_half_z = (pgt3_ptr->z2 - pgt3_ptr->z1) >> 1;
  // C -> A * 0.5
  short ca_half_x = (pgt3_ptr->x0 - pgt3_ptr->x2) >> 1;
  short ca_half_y = (pgt3_ptr->y0 - pgt3_ptr->y2) >> 1;
  short ca_half_z = (pgt3_ptr->z0 - pgt3_ptr->z2) >> 1;
  // Add offset
  ab_half_x += pgt3_ptr->x0;
  ab_half_y += pgt3_ptr->y0;
  ab_half_z += pgt3_ptr->z0;
  bc_half_x += pgt3_ptr->x1;
  bc_half_y += pgt3_ptr->y1;
  bc_half_z += pgt3_ptr->z1;
  ca_half_x += pgt3_ptr->x2;
  ca_half_y += pgt3_ptr->y2;
  ca_half_z += pgt3_ptr->z2;
  // Calculate UV subdivision
  // A -> B * 0.5
  short ab_half_u = (pgt3_ptr->u1 - pgt3_ptr->u0) >> 1;
  short ab_half_v = (pgt3_ptr->v1 - pgt3_ptr->v0) >> 1;
  // B -> C * 0.5
  short bc_half_u = (pgt3_ptr->u2 - pgt3_ptr->u1) >> 1;
  short bc_half_v = (pgt3_ptr->v2 - pgt3_ptr->v1) >> 1;
  // C -> A * 0.5
  short ca_half_u = (pgt3_ptr->u0 - pgt3_ptr->u2) >> 1;
  short ca_half_v = (pgt3_ptr->v0 - pgt3_ptr->v2) >> 1;
  // Add offset
  ab_half_u += pgt3_ptr->u0;
  ab_half_v += pgt3_ptr->v0;
  bc_half_u += pgt3_ptr->u1;
  bc_half_v += pgt3_ptr->v1;
  ca_half_u += pgt3_ptr->u2;
  ca_half_v += pgt3_ptr->v2;
  // Calculate color subdivision
  // A -> B
  u_char ab_r = (pgt3_ptr->r0 + pgt3_ptr->r1) >> 1;
  u_char ab_g = (pgt3_ptr->g0 + pgt3_ptr->g1) >> 1;
  u_char ab_b = (pgt3_ptr->b0 + pgt3_ptr->b1) >> 1;
  // B -> C
  u_char bc_r = (pgt3_ptr->r1 + pgt3_ptr->r2) >> 1;
  u_char bc_g = (pgt3_ptr->g1 + pgt3_ptr->g2) >> 1;
  u_char bc_b = (pgt3_ptr->b1 + pgt3_ptr->b2) >> 1;
  // C -> A
  u_char ca_r = (pgt3_ptr->r2 + pgt3_ptr->r0) >> 1;
  u_char ca_g = (pgt3_ptr->g2 + pgt3_ptr->g0) >> 1;
  u_char ca_b = (pgt3_ptr->b2 + pgt3_ptr->b0) >> 1;
}*/

u_char * SGM2_UpdateModelColor(SGM2_File * model, u_char * packet_ptr, u_long * ot, short depth_offset, u_int flags, CVECTOR color, void * scene) {
  // Static, temporary tpage and clut
  POLY_GT4 * dest_pgt4_ptr;
  POLY_GT3 * dest_pgt3_ptr;
  SGM2_POLYGT4 * pgt4_ptr;
  SGM2_POLYGT3 * pgt3_ptr;
  u_int i;
  u_long tpageid;
  u_long clutid;
  u_long pg_count;
  u_short current_mat;
  u_short last_mat;
  DIVPOLYGON4 * divprop = (DIVPOLYGON4 *)(SCRATCH_PAD+0xC0);
  DIVPOLYGON3 * divprop3 = (DIVPOLYGON3 *)(SCRATCH_PAD+0xC0);
  Scene_Ctx * scenectx = (Scene_Ctx*)scene;
  u_char ambient_r = scenectx->ambient.r; // >> 7
  u_char ambient_g = scenectx->ambient.g; // >> 7
  u_char ambient_b = scenectx->ambient.b; // >> 7

  u_short subdiv_lvl1 = SGM2_SUBDIV_LV1;
  u_short subdiv_lvl2 = SGM2_SUBDIV_LV2;
  u_char subdiv_n1 = 1;
  u_char subdiv_n2 = 2;

  if(flags & SGM2_RENDER_SUBDIV_HIGH) {
    subdiv_lvl1 = SGM2_SUBDIV_HIGH_LV1;
    subdiv_lvl2 = SGM2_SUBDIV_HIGH_LV2;
    subdiv_n1 = 2;
    subdiv_n2 = 3;
  }
  

  divprop->pih = SCREEN_W;
  divprop->piv = SCREEN_H;
  divprop->ndiv = 1;
  //DIVPOLYGON4 divprop;
  //divprop.pih = SCREEN_W;
  //divprop.piv = SCREEN_H;
  //divprop.ndiv = 1;

  //tpageid = getTPage(1, 0, 0,0);
  //clutid = GetClut(TEST_CLX,TEST_CLY);
  /*
  clutid = (u_long)(model->material[0].clut + ((flags & SGM2_RENDER_BUMPCLUT) << 3)) << 16;
  tpageid = (u_long)model->material[0].tpage << 16;*/

  //clutid = model->material[0].clut;
  //tpageid = model->material[0].tpage;

  // Transform to buffer
  AGM_TransformModel(model->vertex_data, model->vertex_count);

  dest_pgt4_ptr = (POLY_GT4*)packet_ptr;
  pgt4_ptr = model->poly_gt4;
  pgt3_ptr = model->poly_gt3;
  pg_count = model->poly_gt4_count;

  for(i = 0; i < pg_count; i++, pgt4_ptr++) {
    long outer_product, otz;
    long tempz0,tempz1,tempz2,tempz3;
    SVECTOR * vec0 = &AGM_TransformBuffer[pgt4_ptr->idx0];
    SVECTOR * vec1 = &AGM_TransformBuffer[pgt4_ptr->idx1];
    SVECTOR * vec2 = &AGM_TransformBuffer[pgt4_ptr->idx2];
    SVECTOR * vec3 = &AGM_TransformBuffer[pgt4_ptr->idx3];

    clutid = (u_long)(model->material[pgt4_ptr->material].clut + ((flags & SGM2_RENDER_BUMPCLUT) << 3)) << 16;
    tpageid = (u_long)model->material[pgt4_ptr->material].tpage << 16;

    // Load screen XY coordinates to GTE Registers
    gte_ldsxy3(*(long*)&vec0->vx,*(long*)&vec1->vx,*(long*)&vec2->vx);

    // Perform clipping calculation
    if(!(flags & SGM2_RENDER_NO_NCLIP)){
      gte_nclip();
      // Store Outer Product
      gte_stopz(&outer_product);
      // Check side
      if(outer_product <= 0) continue; // Skip back facing polys    
    }

    // Load Z coordinates into GTE
    tempz0 = vec0->vz;
    tempz1 = vec1->vz;
    tempz2 = vec2->vz;
    tempz3 = vec3->vz;
    ////printf("Poly %d z: %d, %d, %d, %d\n",i,tempz0,tempz1,tempz2,tempz3);
    gte_ldsz4(tempz0,tempz1,tempz2,tempz3);
    // Get the average Z value
    gte_avsz4();
    // Store value to otz
    gte_stotz(&otz);
    // Reduce OTZ size
    otz = (otz >> OTSUBDIV);
    if(otz < 1) continue;
    
    otz += depth_offset;
    //otz = 50;
    //if(otz >= OTSIZE) otz = OTSIZE-1;

    if (otz < OTSIZE) {
      if(flags & SGM2_RENDER_SCREEN_CLIP) {
        if(quad_clip( (DVECTOR*)&vec0->vx,
                    (DVECTOR*)&vec1->vx,
                    (DVECTOR*)&vec2->vx,
                    (DVECTOR*)&vec3->vx)) continue;
      }
      if(otz < subdiv_lvl1 && flags & SGM2_RENDER_SUBDIV) {
        u_long uvcode0, uvcode1, uvcode2, uvcode3;
        CVECTOR col0, col1, col2, col3;
        //CVECTOR colcode;
        
        if(otz < subdiv_lvl2) {
          divprop->ndiv = subdiv_n2;
        } else {
          divprop->ndiv = subdiv_n1;
        }
        
        vec0 = &model->vertex_data[pgt4_ptr->idx0];
        vec1 = &model->vertex_data[pgt4_ptr->idx1];
        vec2 = &model->vertex_data[pgt4_ptr->idx2];
        vec3 = &model->vertex_data[pgt4_ptr->idx3];
        uvcode0 = *(u_short*)&pgt4_ptr->u0;
        uvcode1 = *(u_short*)&pgt4_ptr->u1;
        uvcode2 = *(u_short*)&pgt4_ptr->u2;
        uvcode3 = *(u_short*)&pgt4_ptr->u3;
        uvcode0 = (uvcode0 & 0xFFFF) | clutid;
        uvcode1 = (uvcode1 & 0xFFFF) | tpageid;

        if(flags & SGM2_RENDER_AMBIENT) { // !!! TODO - Change this to use GTE/BackLight !!!
          gte_ldrgb((CVECTOR*)&pgt4_ptr->r0);
          gte_cc();
          gte_strgb((CVECTOR*)&col0);
          gte_ldrgb((CVECTOR*)&pgt4_ptr->r1);
          gte_cc();
          gte_strgb((CVECTOR*)&col1);
          gte_ldrgb((CVECTOR*)&pgt4_ptr->r2);
          gte_cc();
          gte_strgb((CVECTOR*)&col2);
          gte_ldrgb((CVECTOR*)&pgt4_ptr->r3);
          gte_cc();
          gte_strgb((CVECTOR*)&col3);
        } else {
          col0 = color;
          col1 = color;
          col2 = color;
          col3 = color;
        }

        dest_pgt4_ptr = (POLY_GT4 *)DivideGT4(
        vec0, vec1, vec2, vec3,
        &uvcode0, &uvcode1, &uvcode2, &uvcode3, 
        (CVECTOR*)&col0, (CVECTOR*)&col1, (CVECTOR*)&col2, (CVECTOR*)&col3,
        dest_pgt4_ptr, (u_long *)(ot+otz), divprop);
        } else {
        u_long temp0, temp1, temp2, temp3;
        // If all tests have passed, now process the rest of the primitive.
        // Copy values already in the registers
        gte_stsxy3((long*)&dest_pgt4_ptr->x0,
                    (long*)&dest_pgt4_ptr->x1,
                    (long*)&dest_pgt4_ptr->x2);
        // Store 4th vertex from buffer directly
        temp0 = *(long*)(&vec3->vx);
        *(long*)(&dest_pgt4_ptr->x3) = temp0;
        // Vertex Colors
        /*temp1 = *(long*)(&pgt4_ptr->r0);
        temp2 = *(long*)(&pgt4_ptr->r1);
        temp3 = *(long*)(&pgt4_ptr->r2);
        temp0 = *(long*)(&pgt4_ptr->r3);
        *(long*)(&dest_pgt4_ptr->r0) = temp1;
        *(long*)(&dest_pgt4_ptr->r1) = temp2;
        *(long*)(&dest_pgt4_ptr->r2) = temp3;
        *(long*)(&dest_pgt4_ptr->r3) = temp0;*/
        if(flags & SGM2_RENDER_AMBIENT) { // !!! TODO - Change this to use GTE/BackLight !!!
          gte_ldrgb((CVECTOR*)&pgt4_ptr->r0);
          gte_cc();
          gte_strgb((CVECTOR*)&dest_pgt4_ptr->r0);
          gte_ldrgb((CVECTOR*)&pgt4_ptr->r1);
          gte_cc();
          gte_strgb((CVECTOR*)&dest_pgt4_ptr->r1);
          gte_ldrgb((CVECTOR*)&pgt4_ptr->r2);
          gte_cc();
          gte_strgb((CVECTOR*)&dest_pgt4_ptr->r2);
          gte_ldrgb((CVECTOR*)&pgt4_ptr->r3);
          gte_cc();
          gte_strgb((CVECTOR*)&dest_pgt4_ptr->r3);
          //gte_ldrgb3((CVECTOR*)&pgt4_ptr->r0, (CVECTOR*)&pgt4_ptr->r1, (CVECTOR*)&pgt4_ptr->r2);
          //gte_n
          //gte_strgb3_g4(dest_pgt4_ptr);
          /*dest_pgt4_ptr->r0 = (pgt4_ptr->r0 * ambient_r) >> 7;
          dest_pgt4_ptr->g0 = (pgt4_ptr->g0 * ambient_g) >> 7;
          dest_pgt4_ptr->b0 = (pgt4_ptr->b0 * ambient_b) >> 7;
          dest_pgt4_ptr->r1 = (pgt4_ptr->r1 * ambient_r) >> 7;
          dest_pgt4_ptr->g1 = (pgt4_ptr->g1 * ambient_g) >> 7;
          dest_pgt4_ptr->b1 = (pgt4_ptr->b1 * ambient_b) >> 7;
          dest_pgt4_ptr->r2 = (pgt4_ptr->r2 * ambient_r) >> 7;
          dest_pgt4_ptr->g2 = (pgt4_ptr->g2 * ambient_g) >> 7;
          dest_pgt4_ptr->b2 = (pgt4_ptr->b2 * ambient_b) >> 7;
          dest_pgt4_ptr->r3 = (pgt4_ptr->r3 * ambient_r) >> 7;
          dest_pgt4_ptr->g3 = (pgt4_ptr->g3 * ambient_g) >> 7;
          dest_pgt4_ptr->b3 = (pgt4_ptr->b3 * ambient_b) >> 7;*/
        } else {
          *(long*)(&dest_pgt4_ptr->r0) = *(long*)&color;
          *(long*)(&dest_pgt4_ptr->r1) = *(long*)&color;
          *(long*)(&dest_pgt4_ptr->r2) = *(long*)&color;
          *(long*)(&dest_pgt4_ptr->r3) = *(long*)&color;
        }

        if(flags & SGM2_RENDER_CLUTFOG){
          long fog = (otz-512) >> SGM2_FOG_SHIFT;
          if(fog < 0) fog = 0;
          if(fog > SGM2_FOG_MAX-1) fog = SGM2_FOG_MAX-1;
          clutid = clutid + (fog<<(6+16));
        }
        
        // Texture Coordinates
        // Set Texture Coordinates
        temp1 = *(short*)(&pgt4_ptr->u0);
        temp2 = *(short*)(&pgt4_ptr->u1);
        temp3 = *(short*)(&pgt4_ptr->u2);
        temp0 = *(short*)(&pgt4_ptr->u3);
        *(u_long*)(&dest_pgt4_ptr->u0) = (temp1 & 0xFFFF) | clutid;
        *(u_long*)(&dest_pgt4_ptr->u1) = (temp2 & 0xFFFF) | tpageid;
        *(short*)(&dest_pgt4_ptr->u2) = temp3;
        *(short*)(&dest_pgt4_ptr->u3) = temp0;

        // Set CLUT
        //dest_pgt4_ptr->clut = clutid;
        // Set Texture Page
        //dest_pgt4_ptr->tpage = tpageid;

        setPolyGT4(dest_pgt4_ptr);
        setSemiTrans(dest_pgt4_ptr, flags & SGM2_RENDER_ABE);
      
        // Add primitive to Ordering Table and advance pointer
        // to point to the next primitive to be processed.
        addPrim(ot+otz, dest_pgt4_ptr);
        dest_pgt4_ptr++;
      }
    }
  }

  dest_pgt3_ptr = (POLY_GT3*)dest_pgt4_ptr;
  pg_count = model->poly_gt3_count;

  for(i = 0; i < pg_count; i++, pgt3_ptr++) {
      long outer_product, otz;
      long tempz0,tempz1,tempz2;
      SVECTOR * vec0 = &AGM_TransformBuffer[pgt3_ptr->idx0];
      SVECTOR * vec1 = &AGM_TransformBuffer[pgt3_ptr->idx1];
      SVECTOR * vec2 = &AGM_TransformBuffer[pgt3_ptr->idx2];

      clutid = (u_long)(model->material[pgt3_ptr->material].clut + ((flags & SGM2_RENDER_BUMPCLUT) << 3)) << 16;
      tpageid = (u_long)model->material[pgt3_ptr->material].tpage << 16;
      
      // Load screen XY coordinates to GTE Registers
      gte_ldsxy3(*(long*)&vec0->vx,*(long*)&vec1->vx,*(long*)&vec2->vx);
      // Perform clipping calculation
      if(!(flags & SGM2_RENDER_NO_NCLIP)){
        gte_nclip();
        // Store Outer Product
        gte_stopz(&outer_product);
        // Check side
        if(outer_product <= 0) continue; // Skip back facing polys
      }
      ////printf("rendering face %d\n",i);
      // Load Z coordinates into GTE
      tempz0 = vec0->vz;
      tempz1 = vec1->vz;
      tempz2 = vec2->vz;
      ////printf("Poly %d z: %d, %d, %d, %d\n",i,tempz0,tempz1,tempz2,tempz3);
      gte_ldsz3(tempz0,tempz1,tempz2);
      // Get the average Z value
      gte_avsz3();
      // Store value to otz
      gte_stotz(&otz);
      // Reduce OTZ size
      otz = (otz >> OTSUBDIV)+depth_offset;
      //otz = 50;
      if(otz >= OTSIZE) otz = OTSIZE-1;
      if (otz > OTMIN) {
        // Clip quads that are offscreen before any other operation
        if(flags & SGM2_RENDER_SCREEN_CLIP) {
          if(tri_clip((DVECTOR*)vec0,
                      (DVECTOR*)vec1,
                      (DVECTOR*)vec2)) continue;
        }
        if(otz < subdiv_lvl1 && flags & SGM2_RENDER_SUBDIV) {
          u_long uvcode0, uvcode1, uvcode2;
          CVECTOR col0, col1, col2;
          if(otz < subdiv_lvl2) {
            divprop->ndiv = subdiv_n2;
          } else {
            divprop->ndiv = subdiv_n1;
          }

          vec0 = &model->vertex_data[pgt3_ptr->idx0];
          vec1 = &model->vertex_data[pgt3_ptr->idx1];
          vec2 = &model->vertex_data[pgt3_ptr->idx2];
          uvcode0 = *(u_short*)&pgt3_ptr->u0;
          uvcode1 = *(u_short*)&pgt3_ptr->u1;
          uvcode2 = *(u_short*)&pgt3_ptr->u2;
          uvcode0 = (uvcode0 & 0xFFFF) | clutid;
          uvcode1 = (uvcode1 & 0xFFFF) | tpageid;

          if(flags & SGM2_RENDER_AMBIENT) {
            gte_ldrgb((CVECTOR*)&pgt3_ptr->r0);
            gte_cc();
            gte_strgb((CVECTOR*)&col0);
            gte_ldrgb((CVECTOR*)&pgt3_ptr->r1);
            gte_cc();
            gte_strgb((CVECTOR*)&col1);
            gte_ldrgb((CVECTOR*)&pgt3_ptr->r2);
            gte_cc();
            gte_strgb((CVECTOR*)&col2);
          } else {
            col0 = color;
            col1 = color;
            col2 = color;
          }

          dest_pgt3_ptr = (POLY_GT3 *)DivideGT3(
          vec0, vec1, vec2,
          &uvcode0, &uvcode1, &uvcode2, 
          (CVECTOR*)&col0, (CVECTOR*)&col1, (CVECTOR*)&col2,
          dest_pgt3_ptr, (u_long *)(ot+otz), divprop3);
        } else {
          u_long temp0, temp1, temp2;
          // If all tests have passed, now process the rest of the primitive.
          // Copy values already in the registers
          gte_stsxy3((long*)&dest_pgt3_ptr->x0,
                    (long*)&dest_pgt3_ptr->x1,
                    (long*)&dest_pgt3_ptr->x2);
          // Vertex Colors
          /*temp0 = *(u_long*)(&pgt3_ptr->r0);
          temp1 = *(u_long*)(&pgt3_ptr->r1);
          temp2 = *(u_long*)(&pgt3_ptr->r2);
          *(u_long*)(&dest_pgt3_ptr->r0) = temp0;
          *(u_long*)(&dest_pgt3_ptr->r1) = temp1;
          *(u_long*)(&dest_pgt3_ptr->r2) = temp2;*/

          if(flags & SGM2_RENDER_AMBIENT) { // !!! TODO - Change this to use GTE/BackLight !!!
            gte_ldrgb((CVECTOR*)&pgt3_ptr->r0);
            gte_cc();
            gte_strgb((CVECTOR*)&dest_pgt3_ptr->r0);
            gte_ldrgb((CVECTOR*)&pgt3_ptr->r1);
            gte_cc();
            gte_strgb((CVECTOR*)&dest_pgt3_ptr->r1);
            gte_ldrgb((CVECTOR*)&pgt3_ptr->r2);
            gte_cc();
            gte_strgb((CVECTOR*)&dest_pgt3_ptr->r2);
          } else {
            *(u_long*)(&dest_pgt3_ptr->r0) = *(long*)&color;
            *(u_long*)(&dest_pgt3_ptr->r1) = *(long*)&color;
            *(u_long*)(&dest_pgt3_ptr->r2) = *(long*)&color;
          }

          if(flags & SGM2_RENDER_CLUTFOG){
            long fog = (otz-512) >> SGM2_FOG_SHIFT;
            if(fog < 0) fog = 0;
            if(fog > SGM2_FOG_MAX-1) fog = SGM2_FOG_MAX-1;
            clutid = clutid + (fog<<(6+16));
          }

          // Set Texture Coordinates
          temp0 = *(u_short*)(&pgt3_ptr->u0);
          temp1 = *(u_short*)(&pgt3_ptr->u1);
          temp2 = *(u_short*)(&pgt3_ptr->u2);
          *(u_long*)(&dest_pgt3_ptr->u0) = (temp0 & 0xFFFF) | clutid;
          *(u_long*)(&dest_pgt3_ptr->u1) = (temp1 & 0xFFFF) | tpageid;
          *(u_short*)(&dest_pgt3_ptr->u2) = temp2;
          
          // Set CLUT
          //dest_pgt3_ptr->clut = clutid;

          // Set Texture Page
          //dest_pgt3_ptr->tpage = tpageid;

          setPolyGT3(dest_pgt3_ptr);

          setSemiTrans(dest_pgt3_ptr, flags & SGM2_RENDER_ABE);
          
          // Add primitive to Ordering Table and advance pointer
          // to point to the next primitive to be processed.
          addPrim(ot+otz, dest_pgt3_ptr);
          dest_pgt3_ptr++;
        }
      }
  }

  return (char*)dest_pgt3_ptr;
}