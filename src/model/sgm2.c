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
#define SGM2_SUBDIV_LV2 80

SGM2_File * SGM2_LoadFile(u_long * addr) {
  // Parse the offsets inside the file and change them to memory addresses instead of offsets.
  SGM2_File * dest = (SGM2_File *)addr;

  printf("Loading SGM2\n");
  //printf("vtx num: %d normal num: %d color num: %d mat num: %d\n",dest->vertex_count, dest->normal_count, dest->color_count, dest->mat_count);
  printf("g4 %d g3 %d gt4 %d gt3 %d\n",dest->poly_g4_count, dest->poly_g3_count, dest->poly_gt4_count, dest->poly_gt3_count);
  printf("g4 %x g3 %x gt4 %x gt3 %x\n",dest->poly_g4, dest->poly_g3, dest->poly_gt4, dest->poly_gt3);

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

  printf("g4 %x g3 %x gt4 %x gt3 %x\n",dest->poly_g4, dest->poly_g3, dest->poly_gt4, dest->poly_gt3);
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
  DIVPOLYGON4 * divprop = (DIVPOLYGON4 *)(SCRATCH_PAD+0xC0);
  DIVPOLYGON3 * divprop3 = (DIVPOLYGON3 *)(SCRATCH_PAD+0xC0);
  Scene_Ctx * scenectx = (Scene_Ctx*)scene;
  u_char ambient_r = scenectx->ambient.r; // >> 7
  u_char ambient_g = scenectx->ambient.g; // >> 7
  u_char ambient_b = scenectx->ambient.b; // >> 7
  

  divprop->pih = SCREEN_W;
  divprop->piv = SCREEN_H;
  divprop->ndiv = 1;
  //DIVPOLYGON4 divprop;
  //divprop.pih = SCREEN_W;
  //divprop.piv = SCREEN_H;
  //divprop.ndiv = 1;

  //tpageid = getTPage(1, 0, 0,0);
  //clutid = GetClut(TEST_CLX,TEST_CLY);
  
  clutid = (u_long)(model->material[0].clut + ((flags & SGM2_RENDER_BUMPCLUT) << 3)) << 16;
  tpageid = (u_long)model->material[0].tpage << 16;

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
    SVECTOR * vec0 = &TransformBuffer[pgt4_ptr->idx0];
    SVECTOR * vec1 = &TransformBuffer[pgt4_ptr->idx1];
    SVECTOR * vec2 = &TransformBuffer[pgt4_ptr->idx2];
    SVECTOR * vec3 = &TransformBuffer[pgt4_ptr->idx3];

    //if(quad_clip( (DVECTOR*)&vec0->vx,
    //              (DVECTOR*)&vec1->vx,
    //              (DVECTOR*)&vec2->vx,
    //              (DVECTOR*)&vec3->vx)) continue;

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
    //printf("Poly %d z: %d, %d, %d, %d\n",i,tempz0,tempz1,tempz2,tempz3);
    gte_ldsz4(tempz0,tempz1,tempz2,tempz3);
    // Get the average Z value
    gte_avsz4();
    // Store value to otz
    gte_stotz(&otz);
    // Reduce OTZ size
    otz = (otz >> OTSUBDIV)+depth_offset;
    //otz = 50;
    if (otz > OTMIN && otz < OTSIZE) {
      if(otz < SGM2_SUBDIV_LV1 && flags & SGM2_RENDER_SUBDIV) {
        u_long uvcode0, uvcode1, uvcode2, uvcode3;
        //CVECTOR colcode;
        divprop->ndiv = 1;
        if(otz < SGM2_SUBDIV_LV2) divprop->ndiv = 2;
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
        dest_pgt4_ptr = (POLY_GT4 *)DivideGT4(
        vec0, vec1, vec2, vec3,
        &uvcode0, &uvcode1, &uvcode2, &uvcode3, 
        (CVECTOR*)&pgt4_ptr->r0, (CVECTOR*)&pgt4_ptr->r1, (CVECTOR*)&pgt4_ptr->r2, (CVECTOR*)&pgt4_ptr->r3,
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
          dest_pgt4_ptr->r0 = (pgt4_ptr->r0 * ambient_r) >> 7;
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
          dest_pgt4_ptr->b3 = (pgt4_ptr->b3 * ambient_b) >> 7;
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
      SVECTOR * vec0 = &TransformBuffer[pgt3_ptr->idx0];
      SVECTOR * vec1 = &TransformBuffer[pgt3_ptr->idx1];
      SVECTOR * vec2 = &TransformBuffer[pgt3_ptr->idx2];
      
      // Clip quads that are offscreen before any other operation
      //if(tri_clip((DVECTOR*)vec0,
      //            (DVECTOR*)vec1,
      //            (DVECTOR*)vec2)) continue;
      
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
      //printf("rendering face %d\n",i);
      // Load Z coordinates into GTE
      tempz0 = vec0->vz;
      tempz1 = vec1->vz;
      tempz2 = vec2->vz;
      //printf("Poly %d z: %d, %d, %d, %d\n",i,tempz0,tempz1,tempz2,tempz3);
      gte_ldsz3(tempz0,tempz1,tempz2);
      // Get the average Z value
      gte_avsz3();
      // Store value to otz
      gte_stotz(&otz);
      // Reduce OTZ size
      otz = (otz >> OTSUBDIV)+depth_offset;
      //otz = 50;
      if (otz > OTMIN && otz < OTSIZE) {
        if(otz < SGM2_SUBDIV_LV1 && flags & SGM2_RENDER_SUBDIV) {
          u_long uvcode0, uvcode1, uvcode2;
          divprop->ndiv = 1;
          if(otz < SGM2_SUBDIV_LV2) divprop->ndiv = 2;
          vec0 = &model->vertex_data[pgt3_ptr->idx0];
          vec1 = &model->vertex_data[pgt3_ptr->idx1];
          vec2 = &model->vertex_data[pgt3_ptr->idx2];
          uvcode0 = *(u_short*)&pgt3_ptr->u0;
          uvcode1 = *(u_short*)&pgt3_ptr->u1;
          uvcode2 = *(u_short*)&pgt3_ptr->u2;
          uvcode0 = (uvcode0 & 0xFFFF) | clutid;
          uvcode1 = (uvcode1 & 0xFFFF) | tpageid;
          dest_pgt3_ptr = (POLY_GT3 *)DivideGT3(
          vec0, vec1, vec2,
          &uvcode0, &uvcode1, &uvcode2, 
          (CVECTOR*)&pgt3_ptr->r0, (CVECTOR*)&pgt3_ptr->r1, (CVECTOR*)&pgt3_ptr->r2,
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
            dest_pgt3_ptr->r0 = (pgt3_ptr->r0 * ambient_r) >> 7;
            dest_pgt3_ptr->g0 = (pgt3_ptr->g0 * ambient_g) >> 7;
            dest_pgt3_ptr->b0 = (pgt3_ptr->b0 * ambient_b) >> 7;
            dest_pgt3_ptr->r1 = (pgt3_ptr->r1 * ambient_r) >> 7;
            dest_pgt3_ptr->g1 = (pgt3_ptr->g1 * ambient_g) >> 7;
            dest_pgt3_ptr->b1 = (pgt3_ptr->b1 * ambient_b) >> 7;
            dest_pgt3_ptr->r2 = (pgt3_ptr->r2 * ambient_r) >> 7;
            dest_pgt3_ptr->g2 = (pgt3_ptr->g2 * ambient_g) >> 7;
            dest_pgt3_ptr->b2 = (pgt3_ptr->b2 * ambient_b) >> 7;
          } else {
            temp0 = *(u_long*)(&pgt3_ptr->r0);
            temp1 = *(u_long*)(&pgt3_ptr->r1);
            temp2 = *(u_long*)(&pgt3_ptr->r2);
            *(u_long*)(&dest_pgt3_ptr->r0) = temp0;
            *(u_long*)(&dest_pgt3_ptr->r1) = temp1;
            *(u_long*)(&dest_pgt3_ptr->r2) = temp2;
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