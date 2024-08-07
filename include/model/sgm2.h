/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#ifndef SGM2_H
#define SGM2_H

#include "subdiv.h"
#include "global.h"
//#include "camera.h"
#include "model/clip.h"

#define TEST_CLX 512
#define TEST_CLY 496


#define gte_RotAverageNclip3Reduce(r1,r2,r3,r4,r5,r6,r8,r10)	\
				{	gte_ldv3(r1,r2,r3);	\
					gte_rtpt();		\
					gte_nclip();		\
					gte_stopz(r10);		\
					gte_stsxy3(r4,r5,r6);	\
					gte_avsz3();		\
					gte_stotz(r8);	}
          
#define gte_RotTransPersReduce(r1,r2)   			\
				{	gte_ldv0(r1);   	\
                                        gte_rtps();     	\
                                        gte_stsxy(r2);  }


typedef struct SGM2_PolyG4 {
  u_short idx0, idx1, idx2, idx3;
	u_short n0, n1, n2, n3;
	u_char r0, g0, b0, material;
  u_char r1, g1, b1, pad1;
  u_char r2, g2, b2, pad2;
  u_char r3, g3, b3, pad3;
} SGM2_POLYG4;

typedef struct SGM2_PolyG3 {
  u_short idx0, idx1, idx2;
	u_short n0, n1, n2;
	u_char r0, g0, b0, material;
  u_char r1, g1, b1, pad1;
  u_char r2, g2, b2, pad2;
} SGM2_POLYG3;

typedef struct SGM2_PolyGT4 {
	u_short idx0, idx1, idx2, idx3;
	u_short n0, n1, n2, n3;
	u_char r0, g0, b0, code;
  u_char r1, g1, b1, material;
  u_char r2, g2, b2, pad2;
  u_char r3, g3, b3, pad3;
	u_char u0, v0;
  u_char u1, v1;
  u_char u2, v2;
  u_char u3, v3;
} SGM2_POLYGT4;

typedef struct SGM2_PolyGT3 {
	u_short idx0, idx1, idx2;
	u_short n0, n1, n2;
	u_char r0, g0, b0, code;
  u_char r1, g1, b1, material;
  u_char r2, g2, b2, pad2;
	u_char u0, v0;
  u_char u1, v1;
  u_char u2, v2;
	u_short pad3;
} SGM2_POLYGT3;

typedef struct SGM2_Color {
	u_char r0, g0, b0, code;
} SGM2_COLOR;

typedef struct SGM2_Mat {
	u_short clut;
	u_short tpage;
} SGM2_MATERIAL;
                                        
typedef struct SGM2 {
  u_long fileid;
	u_short vertex_count;
	u_short normal_count;
	u_short color_count;
  u_short poly_g4_count;
  u_short poly_g3_count;
  u_short poly_gt4_count;
  u_short poly_gt3_count;
	u_char mat_count;
	SVECTOR * vertex_data;
  SVECTOR * normal_data;
	//SGM2_COLOR * color_data;
	SGM2_MATERIAL * material;
	SGM2_POLYG4 * poly_g4;
	SGM2_POLYG3 * poly_g3;
	SGM2_POLYGT4 * poly_gt4;
	SGM2_POLYGT3 * poly_gt3;
} SGM2_File;

#define SGM2_RENDER_ABE     		(1<<0)
#define SGM2_RENDER_SUBDIV  		(1<<1)
#define SGM2_RENDER_NO_NCLIP		(1<<2)
#define SGM2_RENDER_BUMPCLUT 		(1<<3)
#define SGM2_RENDER_AMBIENT  		(1<<4)
#define SGM2_RENDER_SUBDIV_HIGH	(1<<5)
#define SGM2_RENDER_CLUTFOG			(1<<6)
#define SGM2_RENDER_SCREEN_CLIP (1<<7)

#define SGM2_REFLECTION_SHIFT 7
#define SGM2_REFLECTION_SIZE 32

SGM2_File * SGM2_LoadFile(u_long * addr);

u_char * SGM2_UpdateModel(SGM2_File * model, u_char * packet_ptr, u_long * ot, short depth_offset, u_int flags, void * scene);

void SGM2_OffsetTexCoords(SGM2_File * model, short x, short y);

void SGM2_OffsetMatTexCoords(SGM2_File * model, u_short mat, short x, short y);

void SGM2_GenerateReflectionUV(SGM2_File * model, MATRIX * model_view, int x_offset, int y_offset);

u_char * SGM2_UpdateModelColor(SGM2_File * model, u_char * packet_ptr, u_long * ot, short depth_offset, u_int flags, CVECTOR color, void * scene);

#endif