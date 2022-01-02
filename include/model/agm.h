/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#ifndef AGM_H
#define AGM_H

#include "subdiv.h"
#include "global.h"
#include "camera/camera.h"
#include "model/clip.h"

// AGM material properties
#define AGM_MATERIAL_NOCULLING  0x0001
#define AGM_MATERIAL_NOLIGHTING 0x0002
#define AGM_MATERIAL_NORENDER   0x8000

typedef struct AGM_PolyG4 {
  u_short idx0, idx1, idx2, idx3;
  u_char r0, g0, b0, clutid;
  u_char r1, g1, b1, pad1;
  u_char r2, g2, b2, pad2;
  u_char r3, g3, b3, pad3;
} AGM_POLYG4;

typedef struct AGM_PolyG3 {
  u_short idx0, idx1, idx2, pad0;
  u_char r0, g0, b0, clutid;
  u_char r1, g1, b1, pad2;
  u_char r2, g2, b2, pad3;
} AGM_POLYG3;

typedef struct AGM_PolyGT4 {
  u_short idx0, idx1, idx2, idx3;
  u_char r0, g0, b0, clutid;
  u_char r1, g1, b1, pad1;
  u_char r2, g2, b2, pad2;
  u_char r3, g3, b3, pad3;
  u_char u0, v0;
  u_char u1, v1;
  u_char u2, v2;
  u_char u3, v3;
} AGM_POLYGT4;

typedef struct AGM_PolyGT3 {
  u_short idx0, idx1, idx2, pad0;
  u_char r0, g0, b0, clutid;
  u_char r1, g1, b1, pad2;
  u_char r2, g2, b2, pad3;
  u_char u0, v0;
  u_char u1, v1;
  u_char u2, v2;
  u_short pad4;
} AGM_POLYGT3;

// Matrix Format, aligned:
// struct {
//   short m[3][3];
//   short pad; // Alignment padding
//   long t[3];
// }

typedef struct AGM_Bone {
  u_short matrix_idx;
  u_short first_child;  // If no child   = 0xFFFF
  u_short next_sibling; // If no sibling = 0xFFFF
  u_short vertex_start; // Vertex Indices start offset
  u_short vertex_count; // Vertex Indices count
} AGM_BONE;

typedef struct AGM_SkelHeader {
  u_short     bone_count;
  u_short     vertex_count;
  AGM_BONE  * bone_list;
  MATRIX    * matrix_list;
  u_short   * vertex_indices;
} AGM_SKELHEADER;

typedef struct AGM_File {
  u_long fileid;
  u_short version;
  u_short vertex_count;
  u_short poly_g4_count;
  u_short poly_g3_count;
  u_short poly_gt4_count;
  u_short poly_gt3_count;
  SVECTOR * vertex_data;
  SVECTOR * normal_data;
  AGM_POLYG4 * poly_g4;
  AGM_POLYG3 * poly_g3;
  AGM_POLYGT4 * poly_gt4;
  AGM_POLYGT3 * poly_gt3;
  AGM_SKELHEADER * skeleton;
  u_long total_size;
} AGM_FILE;

typedef struct AGM_Model {
  AGM_FILE * file;
  u_short * affector_list;
} AGM_model;


typedef struct {
  u_short start;
  u_short length;
} ANM_ENTRY;

typedef struct ANM_Animation {
  u_char pad[3];
  u_char frame_size; // Size of each frame: (bones + 1) * sizeof(SVECTOR)
  u_short animation_count; // Total Animations in this file
  u_short frame_count; // Total Frames in this file
  ANM_ENTRY * animation_list;
  SVECTOR * animation_data;
} ANM_ANIMATION;

// Temporary buffer to store transformed vertices for animated models
extern long AGM_TransformBufferSize;
extern SVECTOR * AGM_TransfomBuffer;
extern SVECTOR * AGM_NormalTransfomBuffer;

// Transform Matrix Stack, max depth = 6
extern MATRIX AGM_MatrixStack[6];
extern MATRIX * AGM_MatrixStackPointer;

void AGM_Init(unsigned long buffer_size);

// Parse loaded file in memory and return a model pointer
void AGM_LoadModel(AGM_model * model, u_long * addr);

void AGM_UnloadModel(AGM_model * model);

// Transform Model Into Screen Space/Bones Transformation
// Store data to temporary scratch
//void AGM_TransformModel(AGM_model * model);
void AGM_TransformModel(SVECTOR * vertex_source, unsigned short vtx_count);

void AGM_TransformByBone(AGM_model * model, MATRIX * mtx, SVECTOR * anim, MATRIX * view);

void AGM_TransformByBoneCopy(AGM_model * model, MATRIX * mtx, SVECTOR * anim, MATRIX * view, MATRIX * out);

void AGM_ProcessBone(AGM_BONE * bones, u_short current_bone, SVECTOR * vtxbuff, SVECTOR * normalbuff, u_short * idxbuff, MATRIX * matrixbuff, SVECTOR * anim, MATRIX * view);

void AGM_ProcessBoneCopy(AGM_BONE * bones, u_short current_bone, SVECTOR * vtxbuff, u_short * idxbuff, MATRIX * matrixbuff, SVECTOR * anim, MATRIX * view, MATRIX * out);

void AGM_TransformBlock(AGM_BONE * bone, SVECTOR * vtxbuff, u_short * idxbuff, SVECTOR * dest);

void AGM_LightBlock(AGM_BONE * bone, SVECTOR * normalbuff, u_short * idxbuff, CVECTOR * dest);

void AGM_TransformBones(AGM_model * model, MATRIX * mtx, SVECTOR * anim, MATRIX * out);

void AGM_ProcessBoneToWorld(AGM_BONE * bones, u_short current_bone, MATRIX * matrixbuff, SVECTOR * anim, MATRIX * out);

void AGM_TransformModelOnly(AGM_model * model, MATRIX * mtx, MATRIX * view, MATRIX * light);

void AGM_ProcessTransformModel(AGM_BONE * bones, u_short current_bone, SVECTOR * vtxbuff, SVECTOR * normalbuff, u_short * idxbuff, MATRIX * matrixbuff, MATRIX * view, MATRIX * light);

// Generate Ordering Table entries
// Must be called after AGM_TransformModel() for the same model
u_char * AGM_DrawModel(AGM_model * model, u_char * packet_ptr, u_long * ot, short zoffset);

void ANM_LoadAnimation(ANM_ANIMATION ** anim, u_char * addr);

// Generates a new frame (frameF) with the resulting interpolation between frameA and frameB
void ANM_InterpolateFrames(SVECTOR * frameF, SVECTOR * frameA, SVECTOR * frameB, short factor, int size);

// TEMPORARY: REMOVE LATER
void PrintMatrix(MATRIX * mat);

u_char * AGM_DrawModelTPage(AGM_model * model, u_char * packet_ptr, u_long * ot, short zoffset, u_short tpageid, u_short * clutid, u_short * mat);

void AGM_OffsetTexByMaterial(AGM_model * model, u_char material, u_char u, u_char v);

extern char compare_bit;

extern long temp_animcounter;
extern SVECTOR temp_animvector;
extern MATRIX temp_identity;
extern short model_bend;
extern short model_arch;
extern short model_head_arch;
extern short model_head_arch_s;

#endif