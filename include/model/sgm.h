/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#ifndef SGM_H
#define SGM_H

#include "SUBDIV.h"
#include "global.h"
//#include "camera.h"
#include "model/clip.h"

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
                                        
#define PolyG3Code 0x30
#define PolyGT3Code 0x34
#define PolyG4Code 0x38
#define PolyGT4Code 0x3c

// HEADER: SGM0
// File Version: v0
// FLAGS
// 0x1 Has Materials
// 0x2 Has Vertex Colors
// 0x4 Has Normals
typedef struct _SGM_HEADER{
  u_long FOURCC;
  u_short fileVersion;
  u_short reserved0;
  u_short reserved1;
  u_short flags;
  u_long fileSize;
} SGM_HEADER;

// Getting the data for each type of face:
// poly_gt4_offset = sizeof(SGM_HEADER) + sizeof(SGM_DATAHEADER);
// poly_gt3_offset = poly_gt4_offset + sizeof(poly_gt4)*poly_gt4_count;
// poly_g4_offset = poly_gt3_offset + sizeof(poly_gt4)*poly_gt4_count;
// poly_g3_offset = poly_g4_offset + sizeof(poly_gt3)*poly_gt3_count;
//
typedef struct _SGM_DATAHEADER {
  u_short material_count;
  u_short poly_gt4_count;
  u_short poly_gt3_count;
  u_short poly_g4_count;
  u_short poly_g3_count;
  u_short pad;
  u_long vertex_offset;
  u_long texcoord_offset;
  u_long color_offset;
  u_long normal_offset;
} SGM_DATAHEADER;

typedef struct _SGM_MATERIAL {
  u_long texture_id;
  u_long clut_id;
  u_short properties;
  // properties:
  // blending mode
  // semi-transparency
  // no-lit (skip light processing for faces with this material)
} SGM_MATERIAL;

typedef struct _SVECSGM {
  short vx, vy, vz;
} SVECSGM;

typedef struct _POLY_GT4_VERTEX {
  SVECTOR v0;
  SVECTOR v1;
  SVECTOR v2;
  SVECTOR v3;
} POLY_GT4_VERTEX;

typedef struct _POLY_GT4_TEX {
  u_char u0, v0;
  u_char u1, v1;
  u_char u2, v2;
  u_char u3, v3;
} POLY_GT4_TEX;

typedef struct _POLY_GT4_COLOR {
  u_char r0,g0,b0,code;
  u_char r1,g1,b1,pad1;
  u_char r2,g2,b2,pad2;
  u_char r3,g3,b3,pad3;
} POLY_GT4_COLOR;

typedef struct _POLY_GT4_NORM {
  short nx0, ny0, nz0, pad0;
  short nx1, ny1, nz1, pad1;
  short nx2, ny2, nz2, pad2;
  short nx3, ny3, nz3, pad3;
} POLY_GT4_NORM;

typedef struct _POLY_GT3_VERTEX {
  SVECTOR v0;
  SVECTOR v1;
  SVECTOR v2;
} POLY_GT3_VERTEX;

typedef struct _POLY_GT3_TEX {
  u_char u0, v0;
  u_char u1, v1;
  u_char u2, v2;
  u_short pad;
} POLY_GT3_TEX;

typedef struct _POLY_GT3_COLOR {
  u_char r0,g0,b0, code;
  u_char r1,g1,b1, pad1;
  u_char r2,g2,b2, pad2;
} POLY_GT3_COLOR;

typedef struct _POLY_GT3_NORM {
  short nx0, ny0, nz0, pad0;
  short nx1, ny1, nz1, pad1;
  short nx2, ny2, nz2, pad2;
} POLY_GT3_NORM;

typedef struct _SGM_FILESTR {
  u_long * file_addr;
  SGM_HEADER * h;
  SGM_DATAHEADER * dh;
  // Vertex
  POLY_GT4_VERTEX * pgt4_vtx;
  POLY_GT3_VERTEX * pgt3_vtx;
  POLY_GT4_VERTEX * pg4_vtx;
  POLY_GT3_VERTEX * pg3_vtx;
  // UV
  POLY_GT4_TEX * pgt4_uv;
  POLY_GT3_TEX * pgt3_uv;
  // Color
  POLY_GT4_COLOR * pgt4_col;
  POLY_GT3_COLOR * pgt3_col;
  POLY_GT4_COLOR * pg4_col;
  POLY_GT3_COLOR * pg3_col;
  // Normals
  POLY_GT4_NORM * pgt4_normal;
  POLY_GT3_NORM * pgt3_normal;
  POLY_GT4_NORM * pg4_normal;
  POLY_GT3_NORM * pg3_normal;
} SGM_FILESTR;

typedef struct _SGM_MDL {
  SGM_FILESTR * file;  
} SGM_MDL;

typedef struct _SGM_OBJ {
  SGM_MDL * model;  // Model Pointer
	MATRIX    matrix; // Rotation & Position Matrix
} SGM_OBJ;

extern u_char subdiv_enable;
extern u_char subdiv_level0;
extern u_char subdiv_level1;
extern u_long subdiv_level0_d;
extern u_long subdiv_level1_d;
extern u_char subdiv_debug;

extern u_char ambient_enable;
extern u_char ambient_r;
extern u_char ambient_g;
extern u_char ambient_b;

extern u_long debug_subdiv_count;
extern u_long debug_face_count;

// SGM_LoadFile: Loads and processes file from addres (addr) to destination (dest)
void SGM_LoadFile(u_long * addr, SGM_FILESTR * dest);

void SGM_SetupModel(SGM_FILESTR * file, SGM_MDL * model);

u_char * SGM_UpdateModel(SGM_MDL * model, u_char * packet_ptr, u_long * ot, short depth_offset);

void SGM_DrawObject(u_long *ot, SGM_OBJ * obj, MATRIX * camera_view, u_short otag);

#endif