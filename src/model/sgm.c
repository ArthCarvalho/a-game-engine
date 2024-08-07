/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#include "model/sgm.h"

#include <libgte.h>
#include <inline_n.h>
#include <gtemac.h>

u_char subdiv_enable = 0;
u_char subdiv_level0 = 1;
u_char subdiv_level1 = 1;
u_long subdiv_level0_d = 80;
u_long subdiv_level1_d = 180;

u_char subdiv_debug = 0;
u_char subdiv_dbg_r[2] = {64,64};
u_char subdiv_dbg_g[2] = {255,64};
u_char subdiv_dbg_b[2] = {64,255};

u_char ambient_enable = 0;
u_char ambient_r = 255;
u_char ambient_g = 255;
u_char ambient_b = 255;

u_long debug_subdiv_count;
u_long debug_face_count;

long fog_modifier1;
long fog_modifier2;
long fog_modifier3;

//#define FAR_COLOR_R 91
//#define FAR_COLOR_G 166
//#define FAR_COLOR_B 227

//#define FAR_COLOR_R 80
//#define FAR_COLOR_G 51
//#define FAR_COLOR_B 20

#define FAR_COLOR_R 81
#define FAR_COLOR_G 56
#define FAR_COLOR_B 9

//#define FAR_COLOR_R 29
//#define FAR_COLOR_G 29
//#define FAR_COLOR_B 10

//#define FAR_COLOR_R 128
//#define FAR_COLOR_G 128
//#define FAR_COLOR_B 128

//#define FAR_COLOR_R 134
//#define FAR_COLOR_G 165
//#define FAR_COLOR_B 176


//#define FAR_COLOR_R 0
//#define FAR_COLOR_G 0
//#define FAR_COLOR_B 0

#define TEST_CLX 512
#define TEST_CLY 496


void SGM_LoadFile(u_long * addr, SGM_FILESTR * dest) {
  int i;
  u_char * file_itr = (u_char*)addr;
  dest->file_addr = addr;
  dest->h = (SGM_HEADER*)addr;
  file_itr += sizeof(SGM_HEADER);
  dest->dh = (SGM_DATAHEADER*)file_itr;
  file_itr += sizeof(SGM_DATAHEADER);
  
    // TODO: add material section
  // Vertices
  file_itr = ((u_char*)addr)+dest->dh->vertex_offset;
  
  ////printf("file_itr = %p (from file offset)\n", file_itr);
  dest->pgt4_vtx = (POLY_GT4_VERTEX*)file_itr;
  file_itr += sizeof(POLY_GT4_VERTEX) * dest->dh->poly_gt4_count;
  
  dest->pgt3_vtx = (POLY_GT3_VERTEX*)file_itr;
  file_itr += sizeof(POLY_GT3_VERTEX) * dest->dh->poly_gt3_count;
  
  dest->pg4_vtx = (POLY_GT4_VERTEX*)file_itr;
  file_itr += sizeof(POLY_GT4_VERTEX) * dest->dh->poly_g4_count;
  
  dest->pg3_vtx = (POLY_GT3_VERTEX*)file_itr;
  file_itr += sizeof(POLY_GT3_VERTEX) * dest->dh->poly_g3_count;
  
  // Texture Coordinates
  dest->pgt4_uv = (POLY_GT4_TEX*)file_itr;
  file_itr += sizeof(POLY_GT4_TEX) * dest->dh->poly_gt4_count;
  
  dest->pgt3_uv = (POLY_GT3_TEX*)file_itr;
  file_itr += sizeof(POLY_GT3_TEX) * dest->dh->poly_gt3_count;
  
  // Colors
  dest->pgt4_col = (POLY_GT4_COLOR*)file_itr;
  file_itr += sizeof(POLY_GT4_COLOR) * dest->dh->poly_gt4_count;
  
  for(i=0; i < dest->dh->poly_gt4_count; i++){
    dest->pgt4_col[i].code = PolyGT4Code;
    dest->pgt4_col[i].pad1 = 0;
    dest->pgt4_col[i].pad2 = 0;
    dest->pgt4_col[i].pad3 = 0;
  }
  
  dest->pgt3_col = (POLY_GT3_COLOR*)file_itr;
  file_itr += sizeof(POLY_GT3_COLOR) * dest->dh->poly_gt3_count;
  
  for(i=0; i < dest->dh->poly_gt3_count; i++){
    dest->pgt3_col[i].code = PolyGT3Code;
    dest->pgt3_col[i].pad1 = 0;
    dest->pgt3_col[i].pad2 = 0;
  }
  
  dest->pg4_col = (POLY_GT4_COLOR*)file_itr;
  file_itr += sizeof(POLY_GT4_COLOR) * dest->dh->poly_g4_count;
  
  for(i=0; i < dest->dh->poly_g4_count; i++){
    dest->pg4_col[i].code = PolyG4Code;
    dest->pg4_col[i].pad1 = 0;
    dest->pg4_col[i].pad2 = 0;
    dest->pg4_col[i].pad3 = 0;
  }
  
  dest->pg3_col = (POLY_GT3_COLOR*)file_itr;
  file_itr += sizeof(POLY_GT3_COLOR) * dest->dh->poly_g3_count;
  
  for(i=0; i < dest->dh->poly_g3_count; i++){
    dest->pg3_col[i].code = PolyG3Code;
    dest->pg3_col[i].pad1 = 0;
    dest->pg3_col[i].pad2 = 0;
  }
  
  // Normals
  dest->pgt4_normal = 0;
  dest->pgt3_normal = 0;
  dest->pg4_normal = 0;
  dest->pg3_normal = 0;
}

void SGM_SetupModel(SGM_FILESTR * file, SGM_MDL * model) {
  model->file = file;
}

u_char * SGM_UpdateModel(SGM_MDL * model, u_char * packet_ptr, u_long * ot, short depth_offset) {
  int i;
  
  SGM_FILESTR * data = model->file;
  SGM_DATAHEADER * dh = data->dh;
  
  u_short tpageid;
  u_short clutid;

  u_char fog_color[] = {FAR_COLOR_R, FAR_COLOR_G, FAR_COLOR_B, 0};
  
  // Transformation Data
  long	p, otz, opz, flg, fogz;
  //long clutnew;
	int temp, temp2, temp3;
	int	isfront;

  RECT screen_rect;
  
  DIVPOLYGON4 * divprop = (DIVPOLYGON4 *)SCRATCH_PAD;
  
  POLY_GT4_VERTEX * vtx_src_gt4;
  POLY_GT4_TEX * tex_src_gt4;
  POLY_GT4_COLOR * col_src_gt4;
  
  POLY_GT3_VERTEX * vtx_src_gt3;
  POLY_GT3_TEX * tex_src_gt3;
  POLY_GT3_COLOR * col_src_gt3;
  
  POLY_GT4 * vtx_dest_gt4;
  POLY_GT3 * vtx_dest_gt3;
  POLY_G4 * vtx_dest_g4;
  POLY_G3 * vtx_dest_g3;
  
  int subdiv_level = 0;
  
  u_char * packet = packet_ptr;
  
  divprop->pih = SCREEN_W;
  divprop->piv = SCREEN_H;
  divprop->ndiv = 1;
  
  vtx_dest_gt4 = (POLY_GT4*)packet;
  
  tpageid = getTPage(1, 0, 0,0);
  clutid = GetClut(TEST_CLX,TEST_CLY);
  
  vtx_src_gt4 = data->pgt4_vtx;
  
  //DEBUG - Avoid Crash
  if(subdiv_level0 < 1) subdiv_level0 = 1;
  if(subdiv_level1 < 1) subdiv_level1 = 1;
  if(subdiv_level0 > 2) subdiv_level0 = 2;
  if(subdiv_level1 > 2) subdiv_level1 = 2;

  screen_rect = G.screen_rect;

  gte_SetFarColor(FAR_COLOR_R,FAR_COLOR_G,FAR_COLOR_B);

  //FntPrint("Total Faces SGM: %d\n",model->file->dh->poly_gt4_count+model->file->dh->poly_gt3_count);
  
  for(i = 0; i < model->file->dh->poly_gt4_count; i++, vtx_src_gt4++) {
    long fogp;
    long fog_z0, fog_z1, fog_z2, fog_z3;
    // Transform destination poly
    //gte_RotAverageNclip3Reduce(
		//	(SVECTOR*)&vtx_src_gt4->v0,
		//	(SVECTOR*)&vtx_src_gt4->v1,
		//	(SVECTOR*)&vtx_src_gt4->v2,
		//	(long *)&vtx_dest_gt4->x0, (long *)&vtx_dest_gt4->x1,
		//	(long *)&vtx_dest_gt4->x2, &otz, &isfront);


    gte_ldv3((SVECTOR*)&vtx_src_gt4->v0,(SVECTOR*)&vtx_src_gt4->v1,(SVECTOR*)&vtx_src_gt4->v2);
    gte_rtpt();

    gte_nclip();
    gte_stopz(&isfront);

    // Check side
    if(isfront <= 0) continue;
    
    //gte_stsz3(&fog_z0, &fog_z1, &fog_z2);
    gte_avsz3();
    gte_stsz(&fogz);
    gte_stotz(&otz);
    otz = (otz >> OTSUBDIV)+depth_offset;

    //fog_z0 >>= 2;
    //fog_z1 >>= 2;
    //fog_z2 >>= 2;

    

    if (otz > OTMIN && otz < OTSIZE)
		{
      gte_stsxy3((long *)&vtx_dest_gt4->x0,(long *)&vtx_dest_gt4->x1,(long *)&vtx_dest_gt4->x2);

      // Only transform 4th vector if facing towards the view
      //gte_RotTransPersReduce((SVECTOR*)&vtx_src_gt4->v3,(long *)&vtx_dest_gt4->x3);
      gte_ldv0((SVECTOR*)&vtx_src_gt4->v3);
      gte_rtps();

      tex_src_gt4 = &data->pgt4_uv[i];
      col_src_gt4 = &data->pgt4_col[i];

      vtx_dest_gt4->tpage = tpageid;

      gte_stsxy((long *)&vtx_dest_gt4->x3); 
      //gte_stsz(&fog_z3);
      //fog_z3 >>= 2;
      
      
      // Clip quads that are offscreen
      //if(quad_clip( (DVECTOR*)&vtx_dest_gt4->x0,
      //              (DVECTOR*)&vtx_dest_gt4->x1,
      //              (DVECTOR*)&vtx_dest_gt4->x2,
      //              (DVECTOR*)&vtx_dest_gt4->x3)) continue;

      fogp = (fogz>> fog_modifier2) - fog_modifier1;

      // If on screen, now process the rest of the primitive.
      
      // Set Vertex Colors
      
      //fog_z0 = (fog_z0>> fog_modifier2) - fog_modifier1;
      //fog_z1 = (fog_z1>> fog_modifier2) - fog_modifier1;
      //fog_z2 = (fog_z2>> fog_modifier2) - fog_modifier1;
      //fog_z3 = (fog_z3>> fog_modifier2) - fog_modifier1;
      
      if(fogp <= 0){
        long temp0, temp1, temp2, temp3;
        temp0 = *(long*)(&col_src_gt4->r0);
        temp1 = *(long*)(&col_src_gt4->r1);
        temp2 = *(long*)(&col_src_gt4->r2);
        temp3 = *(long*)(&col_src_gt4->r3);
        *(long*)(&vtx_dest_gt4->r0) = temp0;
        *(long*)(&vtx_dest_gt4->r1) = temp1;
        *(long*)(&vtx_dest_gt4->r2) = temp2;
        *(long*)(&vtx_dest_gt4->r3) = temp3;

        temp0 = *(short*)(&tex_src_gt4->u0);
        temp1 = *(short*)(&tex_src_gt4->u1);
        temp2 = *(short*)(&tex_src_gt4->u2);
        temp3 = *(short*)(&tex_src_gt4->u3);
        *(short*)(&vtx_dest_gt4->u0) = temp0;
        *(short*)(&vtx_dest_gt4->u1) = temp1;
        *(short*)(&vtx_dest_gt4->u2) = temp2;
        *(short*)(&vtx_dest_gt4->u3) = temp3;
        /*
        long temp0, temp1;
        temp0 = *(long*)(&col_src_gt4->r0);
        temp1 = *(long*)(&col_src_gt4->r1);
        *(long*)(&vtx_dest_gt4->r0) = temp0;
        *(long*)(&vtx_dest_gt4->r1) = temp1;
        temp0 = *(long*)(&col_src_gt4->r2);
        temp1 = *(long*)(&col_src_gt4->r3);
        *(long*)(&vtx_dest_gt4->r2) = temp0;
        *(long*)(&vtx_dest_gt4->r3) = temp1;

        temp0 = *(short*)(&tex_src_gt4->u0);
        temp1 = *(short*)(&tex_src_gt4->u1);
        *(short*)(&vtx_dest_gt4->u0) = temp0;
        *(short*)(&vtx_dest_gt4->u1) = temp1;
        temp0 = *(short*)(&tex_src_gt4->u2);
        temp1 = *(short*)(&tex_src_gt4->u3);
        *(short*)(&vtx_dest_gt4->u2) = temp0;
        *(short*)(&vtx_dest_gt4->u3) = temp1;
        */
        /*
        *(long*)(&vtx_dest_gt4->r0) = *(long*)(&col_src_gt4->r0);
        *(long*)(&vtx_dest_gt4->r1) = *(long*)(&col_src_gt4->r1);
        *(long*)(&vtx_dest_gt4->r2) = *(long*)(&col_src_gt4->r2);
        *(long*)(&vtx_dest_gt4->r3) = *(long*)(&col_src_gt4->r3);

        *(short*)(&vtx_dest_gt4->u0) = *(short*)(&tex_src_gt4->u0);
        *(short*)(&vtx_dest_gt4->u1) = *(short*)(&tex_src_gt4->u1);
        *(short*)(&vtx_dest_gt4->u2) = *(short*)(&tex_src_gt4->u2);
        *(short*)(&vtx_dest_gt4->u3) = *(short*)(&tex_src_gt4->u3);
        */
        vtx_dest_gt4->clut = getClut(TEST_CLX,TEST_CLY);
      } else if(fogp > 4096){
        long temp = *(long*)fog_color;
        *(long*)(&vtx_dest_gt4->r0) = temp;
        *(long*)(&vtx_dest_gt4->r1) = temp;
        *(long*)(&vtx_dest_gt4->r2) = temp;
        *(long*)(&vtx_dest_gt4->r3) = temp;
        *(short*)(&vtx_dest_gt4->u0) = 0;
        *(short*)(&vtx_dest_gt4->u1) = 0;
        *(short*)(&vtx_dest_gt4->u2) = 0;
        *(short*)(&vtx_dest_gt4->u3) = 0;
        vtx_dest_gt4->clut = getClut(TEST_CLX,TEST_CLY+15);
      } else {
        long fogarg = fogp + fog_modifier3;
        long fogcl;
        short temp0, temp1;

        gte_ldrgb3(&col_src_gt4->r0, &col_src_gt4->r1, &col_src_gt4->r2);
        gte_lddp(fogarg);

        fogcl = TEST_CLY + ((fogp>>8)-1);

        gte_dpct_b();

        if(fogcl < TEST_CLY) fogcl = TEST_CLY;
        vtx_dest_gt4->clut = getClut(TEST_CLX,fogcl);
        
        gte_strgb3(&vtx_dest_gt4->r0, &vtx_dest_gt4->r1, &vtx_dest_gt4->r2);

        gte_ldrgb(&col_src_gt4->r3);
        //gte_lddp(fogarg);
        gte_dpcs();		
        // Use idle time until dpcs finishes?
        /*
        *(short*)(&vtx_dest_gt4->u0) = *(short*)(&tex_src_gt4->u0);
        *(short*)(&vtx_dest_gt4->u1) = *(short*)(&tex_src_gt4->u1);
        *(short*)(&vtx_dest_gt4->u2) = *(short*)(&tex_src_gt4->u2);
        *(short*)(&vtx_dest_gt4->u3) = *(short*)(&tex_src_gt4->u3);
        */
        temp0 = *(short*)(&tex_src_gt4->u0);
        temp1 = *(short*)(&tex_src_gt4->u1);
        *(short*)(&vtx_dest_gt4->u0) = temp0;
        *(short*)(&vtx_dest_gt4->u1) = temp1;
        temp0 = *(short*)(&tex_src_gt4->u2);
        temp1 = *(short*)(&tex_src_gt4->u3);
        *(short*)(&vtx_dest_gt4->u2) = temp0;
        *(short*)(&vtx_dest_gt4->u3) = temp1;

        gte_strgb(&vtx_dest_gt4->r3);

        //gte_DpqColor3(&col_src_gt4->r0, &col_src_gt4->r1, &col_src_gt4->r2, fogarg,
        //            &vtx_dest_gt4->r0, &vtx_dest_gt4->r1, &vtx_dest_gt4->r2);
        //gte_DpqColor(&col_src_gt4->r3, fogarg, &vtx_dest_gt4->r3);

        //*(long*)(&vtx_dest_gt4->r0) = *(long*)(&col_src_gt4->r0);
        //*(long*)(&vtx_dest_gt4->r1) = *(long*)(&col_src_gt4->r1);
        //*(long*)(&vtx_dest_gt4->r2) = *(long*)(&col_src_gt4->r2);
        //*(long*)(&vtx_dest_gt4->r3) = *(long*)(&col_src_gt4->r3);

        //fog_z0 = (fog_z0 >> fog_modifier2) - fog_modifier1;
        //fog_z1 = (fog_z1 >> fog_modifier2) - fog_modifier1;
        //fog_z2 = (fog_z2 >> fog_modifier2) - fog_modifier1;
        //fog_z3 = (fog_z3 >> fog_modifier2) - fog_modifier1;

        //gte_DpqColor(&col_src_gt4->r0, fog_z0, &vtx_dest_gt4->r0);
        //gte_DpqColor(&col_src_gt4->r1, fog_z1, &vtx_dest_gt4->r1);
        //gte_DpqColor(&col_src_gt4->r2, fog_z2, &vtx_dest_gt4->r2);
        //gte_DpqColor(&col_src_gt4->r3, fog_z3, &vtx_dest_gt4->r3);
        //gte_ldlvl(&fogn);		// load source
        //gte_ldfc(&fogf);		// load dest
        //gte_lddp(fogp);
        //gte_intpl();			  // interpolate (8 cycles)
        //gte_stlvnl(&fogcl);  // put interpolated value into dest
        //fogcl = 256 + ((fogp>>8)-2);

        //vtx_dest_gt4->clut = getClut(TEST_CLX,TEST_CLY);

        //vtx_dest_gt4->clut = getClut(0,256);

        //gte_DpqColor3(&col_src_gt4->r0, &col_src_gt4->r1, &col_src_gt4->r2, fogp,
        //            &vtx_dest_gt4->r0, &vtx_dest_gt4->r1, &vtx_dest_gt4->r2);
        //gte_DpqColor(&col_src_gt4->r3, fogp, &vtx_dest_gt4->r3);
        
      }

      
      
        //gte_DpqColor(&col_src_gt4->r0, fog_z0, &vtx_dest_gt4->r0);
        //gte_DpqColor(&col_src_gt4->r1, fog_z1, &vtx_dest_gt4->r1);
        //gte_DpqColor(&col_src_gt4->r2, fog_z2, &vtx_dest_gt4->r2);
        //gte_DpqColor(&col_src_gt4->r3, fog_z3, &vtx_dest_gt4->r3);
        

        //*(long*)(&vtx_dest_gt4->r0) = *(long*)(&col_src_gt4->r0);
        //*(long*)(&vtx_dest_gt4->r1) = *(long*)(&col_src_gt4->r1);
        //*(long*)(&vtx_dest_gt4->r2) = *(long*)(&col_src_gt4->r2);
        //*(long*)(&vtx_dest_gt4->r3) = *(long*)(&col_src_gt4->r3);

      // Set Texture Coordinates
      
      // Set CLUT
        //clutnew = (clutnew<<6) + clutid;
      
      
      
      // Set Texture Page

      // Initialize destination poly
      setPolyGT4(vtx_dest_gt4);

      // Check for subdivision
      if( otz < subdiv_level1_d && subdiv_enable ) {
        if(otz > subdiv_level0_d) {
          divprop->ndiv = subdiv_level1;
          subdiv_level = subdiv_level1;
          if(subdiv_debug) {
            vtx_dest_gt4->r0 = vtx_dest_gt4->r1 = vtx_dest_gt4->r2 = vtx_dest_gt4->r3 = subdiv_dbg_r[0];
            vtx_dest_gt4->g0 = vtx_dest_gt4->g1 = vtx_dest_gt4->g2 = vtx_dest_gt4->g3 = subdiv_dbg_g[0];
            vtx_dest_gt4->b0 = vtx_dest_gt4->b1 = vtx_dest_gt4->b2 = vtx_dest_gt4->b3 = subdiv_dbg_b[0];
          }
        } else {
          divprop->ndiv = subdiv_level0;
          subdiv_level = subdiv_level0;
          if(subdiv_debug) {
            vtx_dest_gt4->r0 = vtx_dest_gt4->r1 = vtx_dest_gt4->r2 = vtx_dest_gt4->r3 = subdiv_dbg_r[1];
            vtx_dest_gt4->g0 = vtx_dest_gt4->g1 = vtx_dest_gt4->g2 = vtx_dest_gt4->g3 = subdiv_dbg_g[1];
            vtx_dest_gt4->b0 = vtx_dest_gt4->b1 = vtx_dest_gt4->b2 = vtx_dest_gt4->b3 = subdiv_dbg_b[1];
          }
        }
        debug_subdiv_count += 1;
        
        //memset(0x1f800000,0xFF,0x400);
        // Subdivide poly
        //vtx_dest_gt4 += DEL_subdivide_gt4_asm(vtx_dest_gt4, ot+otz, subdiv_level) >> 2;
        vtx_dest_gt4 = (POLY_GT4 *)DivideGT4(
          (SVECTOR*)&vtx_src_gt4->v0, (SVECTOR*)&vtx_src_gt4->v1, (SVECTOR*)&vtx_src_gt4->v2, (SVECTOR*)&vtx_src_gt4->v3,
          (u_long*)&vtx_dest_gt4->u0, (u_long*)&vtx_dest_gt4->u1, (u_long*)&vtx_dest_gt4->u2, (u_long*)&vtx_dest_gt4->u3,
          (CVECTOR*)&vtx_dest_gt4->r0, (CVECTOR*)&vtx_dest_gt4->r1, (CVECTOR*)&vtx_dest_gt4->r2, (CVECTOR*)&vtx_dest_gt4->r3,
          vtx_dest_gt4, (u_long *)(ot+otz), divprop);
      } else {
        // Add primitive to Ordering Table and advance pointer
        // to point to the next primitive to be processed.
        addPrim(ot+otz, vtx_dest_gt4);
        vtx_dest_gt4++;
      }
    }
  }
  
  vtx_dest_gt3 = (POLY_GT3*)vtx_dest_gt4;
  vtx_src_gt3 = data->pgt3_vtx;

  for(i = 0; i < model->file->dh->poly_gt3_count; i++, vtx_src_gt3++) {
    long fogp;
    // Transform destination poly
    //gte_RotAverageNclip3Reduce(
		//	(SVECTOR*)&vtx_src_gt3->v0,
		//	(SVECTOR*)&vtx_src_gt3->v1,
		//	(SVECTOR*)&vtx_src_gt3->v2,
		//	(long *)&vtx_dest_gt3->x0, (long *)&vtx_dest_gt3->x1,
		//	(long *)&vtx_dest_gt3->x2, &otz, &isfront);

    gte_ldv3((SVECTOR*)&vtx_src_gt3->v0,(SVECTOR*)&vtx_src_gt3->v1,(SVECTOR*)&vtx_src_gt3->v2);
    gte_rtpt();

    gte_nclip();
    gte_stopz(&isfront);
    
    //gte_stsz3(&fog_z0, &fog_z1, &fog_z2);
    gte_avsz3();
    gte_stsz(&fogz);
    gte_stotz(&otz);
    otz = (otz >> OTSUBDIV)+depth_offset;

    
    //fogz = otz << 1;
    //fogz = otz;
    //otz = (otz >> OTSUBDIV)+depth_offset;
    // Check side
    if(isfront <= 0) continue;

    if (otz > OTMIN && otz < OTSIZE)
		{
      fogp = (fogz>> fog_modifier2) - fog_modifier1;
      gte_stsxy3((long *)&vtx_dest_gt3->x0,(long *)&vtx_dest_gt3->x1,(long *)&vtx_dest_gt3->x2);
      // Clip quads that are offscreen
      //if(tri_clip((DVECTOR*)&vtx_dest_gt3->x0,
      //            (DVECTOR*)&vtx_dest_gt3->x1,
      //            (DVECTOR*)&vtx_dest_gt3->x2)) continue;

      // If on screen, now process the rest of the primitive.
      tex_src_gt3 = &data->pgt3_uv[i];
      col_src_gt3 = &data->pgt3_col[i];
      // Set Vertex Colors
      //fogp = (fogz>> fog_modifier2) - fog_modifier1;
      
      if(fogp < 0){
        long temp0, temp1, temp2;
        fogp = 0;
        temp0 = *(long*)(&col_src_gt3->r0);
        temp1 = *(long*)(&col_src_gt3->r1);
        temp2 = *(long*)(&col_src_gt3->r2);
        *(long*)(&vtx_dest_gt3->r0) = temp0;
        *(long*)(&vtx_dest_gt3->r1) = temp1;
        *(long*)(&vtx_dest_gt3->r2) = temp2;
        temp0 = *(short*)(&tex_src_gt3->u0);
        temp1 = *(short*)(&tex_src_gt3->u1);
        temp2 = *(short*)(&tex_src_gt3->u2); 
        *(short*)(&vtx_dest_gt3->u0) = temp0;
        *(short*)(&vtx_dest_gt3->u1) = temp1;
        *(short*)(&vtx_dest_gt3->u2) = temp2; 
        vtx_dest_gt3->clut = getClut(512,TEST_CLY);
        /*
        *(long*)(&vtx_dest_gt3->r0) = *(long*)(&col_src_gt3->r0);
        *(long*)(&vtx_dest_gt3->r1) = *(long*)(&col_src_gt3->r1);
        *(long*)(&vtx_dest_gt3->r2) = *(long*)(&col_src_gt3->r2);
        vtx_dest_gt3->clut = getClut(512,TEST_CLY);
        *(short*)(&vtx_dest_gt3->u0) = *(short*)(&tex_src_gt3->u0);
        *(short*)(&vtx_dest_gt3->u1) = *(short*)(&tex_src_gt3->u1);
        *(short*)(&vtx_dest_gt3->u2) = *(short*)(&tex_src_gt3->u2); 
        */
      } else if(fogp > 4096){
        long temp0 = *(long*)fog_color;
        fogp = 4096;
        *(long*)(&vtx_dest_gt3->r0) = temp0;
        *(long*)(&vtx_dest_gt3->r1) = temp0;
        *(long*)(&vtx_dest_gt3->r2) = temp0;
        *(short*)(&vtx_dest_gt3->u0) = 0;
        *(short*)(&vtx_dest_gt3->u1) = 0;
        *(short*)(&vtx_dest_gt3->u2) = 0;
        vtx_dest_gt3->clut = getClut(512,TEST_CLY+15);

        /*
        *(long*)(&vtx_dest_gt3->r0) = *(long*)fog_color;
        *(long*)(&vtx_dest_gt3->r1) = *(long*)fog_color;
        *(long*)(&vtx_dest_gt3->r2) = *(long*)fog_color;
        vtx_dest_gt3->clut = getClut(512,TEST_CLY+15);
        *(short*)(&vtx_dest_gt3->u0) = 0;
        *(short*)(&vtx_dest_gt3->u1) = 0;
        *(short*)(&vtx_dest_gt3->u2) = 0;
        */ 
      } else {
        long fogarg = fogp + fog_modifier3;
        long fogcl;
        short temp0, temp1;
        gte_ldrgb3(&col_src_gt3->r0, &col_src_gt3->r1, &col_src_gt3->r2);
        gte_lddp(fogarg);
        // Using what would be two nops before dpct? I hope so!
        fogcl = TEST_CLY + ((fogp>>8)-1);
        gte_dpct_b();

        if(fogcl < TEST_CLY) fogcl = TEST_CLY;
        vtx_dest_gt3->clut = getClut(TEST_CLX,fogcl);

        temp0 = *(short*)(&tex_src_gt3->u0);
        temp1 = *(short*)(&tex_src_gt3->u1);
        *(short*)(&vtx_dest_gt3->u0) = temp0;
        temp0 = *(short*)(&tex_src_gt3->u2); 
        *(short*)(&vtx_dest_gt3->u1) = temp1;
        *(short*)(&vtx_dest_gt3->u2) = temp0; 

        /*
        *(short*)(&vtx_dest_gt3->u0) = *(short*)(&tex_src_gt3->u0);
        *(short*)(&vtx_dest_gt3->u1) = *(short*)(&tex_src_gt3->u1);
        *(short*)(&vtx_dest_gt3->u2) = *(short*)(&tex_src_gt3->u2); 
        */

        gte_strgb3(&vtx_dest_gt3->r0, &vtx_dest_gt3->r1, &vtx_dest_gt3->r2);
        //gte_DpqColor3(&col_src_gt3->r0, &col_src_gt3->r1, &col_src_gt3->r2, fogarg,
        //              &vtx_dest_gt3->r0, &vtx_dest_gt3->r1, &vtx_dest_gt3->r2);

        
        
      }
      
      // Set Texture Coordinates
      
      
      // Set CLUT
      //vtx_dest_gt3->clut = clutid;

      
      // Set Texture Page
      vtx_dest_gt3->tpage = tpageid;
      
      // Initialize destination poly
      setPolyGT3(vtx_dest_gt3);
      
      // Add primitive to Ordering Table and advance pointer
      // to point to the next primitive to be processed.
      addPrim(ot+otz, vtx_dest_gt3);
      vtx_dest_gt3++;
      //debug_face_count += 1;
    }
  }

  vtx_dest_g4 = (POLY_G4 *) vtx_dest_gt3;
  vtx_src_gt4 = data->pg4_vtx;
  
  for(i = 0; i < model->file->dh->poly_g4_count; i++, vtx_src_gt4++) {
    // Transform destination poly
    gte_RotAverageNclip3Reduce(
			(SVECTOR*)&vtx_src_gt4->v0,
			(SVECTOR*)&vtx_src_gt4->v1,
			(SVECTOR*)&vtx_src_gt4->v2,
			(long *)&vtx_dest_g4->x0, (long *)&vtx_dest_g4->x1,
			(long *)&vtx_dest_g4->x2, &otz, &isfront);
    otz = (otz >> OTSUBDIV)+depth_offset;
    // Check side
    if(isfront <= 0) continue;
    
    // Only transform 4th vector if facing towards the view
    gte_RotTransPersReduce((SVECTOR*)&vtx_src_gt4->v3,(long *)&vtx_dest_g4->x3);
    
    // Clip quads that are offscreen
    //if(quad_clip( (DVECTOR*)&vtx_dest_g4->x0,
    //              (DVECTOR*)&vtx_dest_g4->x1,
    //              (DVECTOR*)&vtx_dest_g4->x2,
    //              (DVECTOR*)&vtx_dest_g4->x3)) continue;

    if (otz > OTMIN && otz < OTSIZE)
		{
      long temp0, temp1, temp2, temp3;
      // If on screen, now process the rest of the primitive.
      col_src_gt4 = &data->pg4_col[i];
      // Set Vertex Colors
      temp0 = *(long*)(&col_src_gt4->r0);
      temp1 = *(long*)(&col_src_gt4->r1);
      temp2 = *(long*)(&col_src_gt4->r2);
      temp3 = *(long*)(&col_src_gt4->r3);
      *(long*)(&vtx_dest_g4->r0) = temp0;
      *(long*)(&vtx_dest_g4->r1) = temp1;
      *(long*)(&vtx_dest_g4->r2) = temp2;
      *(long*)(&vtx_dest_g4->r3) = temp3;
      /*
      *(long*)(&vtx_dest_g4->r0) = *(long*)(&col_src_gt4->r0);
      *(long*)(&vtx_dest_g4->r1) = *(long*)(&col_src_gt4->r1);
      *(long*)(&vtx_dest_g4->r2) = *(long*)(&col_src_gt4->r2);
      *(long*)(&vtx_dest_g4->r3) = *(long*)(&col_src_gt4->r3);
      */
      
      // Initialize destination poly
      setPolyG4(vtx_dest_g4);
      
      // Add primitive to Ordering Table and advance pointer
      // to point to the next primitive to be processed.
      addPrim(ot+otz, vtx_dest_g4);
      vtx_dest_g4++;
    }
  }
  
  vtx_dest_g3 = (POLY_G3*)vtx_dest_g4;
  vtx_src_gt3 = data->pg3_vtx;

  for(i = 0; i < model->file->dh->poly_g3_count; i++, vtx_src_gt3++) {    
    // Transform destination poly
    gte_RotAverageNclip3Reduce(
			(SVECTOR*)&vtx_src_gt3->v0,
			(SVECTOR*)&vtx_src_gt3->v1,
			(SVECTOR*)&vtx_src_gt3->v2,
			(long *)&vtx_dest_g3->x0, (long *)&vtx_dest_g3->x1,
			(long *)&vtx_dest_g3->x2, &otz, &isfront);
    // Check side
    if(isfront <= 0) continue;
    
    // Clip quads that are offscreen
    //if(tri_clip((DVECTOR*)&vtx_dest_g3->x0,
    //            (DVECTOR*)&vtx_dest_g3->x1,
    //            (DVECTOR*)&vtx_dest_g3->x2)) continue;

    otz = (otz >> OTSUBDIV)+depth_offset;
              
    if (otz > OTMIN && otz < OTSIZE)
		{
      long temp0, temp1;
      // If on screen, now process the rest of the primitive.
      col_src_gt3 = &data->pg3_col[i];
      // Set Vertex Colors
      temp0 = *(long*)(&col_src_gt3->r0);
      temp1 = *(long*)(&col_src_gt3->r1);
      *(long*)(&vtx_dest_g3->r0) = temp0;
      temp0 = *(long*)(&col_src_gt3->r2);
      *(long*)(&vtx_dest_g3->r1) = temp1;
      *(long*)(&vtx_dest_g3->r2) = temp0;
      /*
      *(long*)(&vtx_dest_g3->r0) = *(long*)(&col_src_gt3->r0);
      *(long*)(&vtx_dest_g3->r1) = *(long*)(&col_src_gt3->r1);
      *(long*)(&vtx_dest_g3->r2) = *(long*)(&col_src_gt3->r2);
      */
      
      // Initialize destination poly
      setPolyG3(vtx_dest_g3);
      
      // Add primitive to Ordering Table and advance pointer
      // to point to the next primitive to be processed.
      addPrim(ot+otz, vtx_dest_g3);
      vtx_dest_g3++;
    }
  }

  packet = (u_char *) vtx_dest_g3;
  // return the updated packet pointer
  return packet;
}

void SGM_DrawObject(u_long *ot, SGM_OBJ * obj, MATRIX * camera_view, u_short otag) {
  MATRIX tmatrix_final;
	CompMatrixLV(camera_view, &obj->matrix, &tmatrix_final);

	SetRotMatrix(&tmatrix_final);
	SetTransMatrix(&tmatrix_final);

  //SGM_DrawModel(ot, obj->model, otag);
}