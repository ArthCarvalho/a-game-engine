/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#include "model/agm.h"

#include <inline_n.h>
#include <gtemac.h>

char compare_bit = 0;

long temp_animcounter = 0;
short model_bend = 0;
short model_arch = 0;
short model_head_arch = 0;
short model_head_arch_s = 0;
SVECTOR temp_animvector;
MATRIX temp_identity = {
	4096,   0,   0,
	  0, 4096,   0,
	  0,   0, 4096,
	  0,   0,   0
};

//long AGM_TransformBufferSize = 0;
//SVECTOR * AGM_TransformBuffer = NULL;
MATRIX * AGM_MatrixStackPointer;



// Parse loaded file in memory and return a model pointer
void AGM_LoadModel(AGM_model * model, u_long * addr){
  // Change the model's file address to the address of the data loaded to memory.
  model->file = (AGM_FILE *)addr;
  
  // Parse the offsets inside the file and change them to memory addresses instead of offsets.
  model->file->vertex_data = (SVECTOR*)((char*)addr + ((long)model->file->vertex_data));
  model->file->normal_data = (SVECTOR*)((char*)addr + ((long)model->file->normal_data));
  model->file->poly_g4 = (AGM_POLYG4*)((char*)addr + ((long)model->file->poly_g4));
  model->file->poly_g3 = (AGM_POLYG3*)((char*)addr + ((long)model->file->poly_g3));
  model->file->poly_gt4 = (AGM_POLYGT4*)((char*)addr + ((long)model->file->poly_gt4));
  model->file->poly_gt3 = (AGM_POLYGT3*)((char*)addr + ((long)model->file->poly_gt3));

  /*for(int i = 0; i < model->file->vertex_count; i++) {
    SVECTOR * vertex = &model->file->vertex_data[i];
    vertex->vx <<= 2;
    vertex->vy <<= 2;
    vertex->vz <<= 2;
  }*/

  /*for(int i = 0; i < model->file->vertex_count; i++) {
    SVECTOR * vertex = &model->file->normal_data[i];
    long len = SquareRoot0(vertex->vx*vertex->vx+vertex->vy*vertex->vy+vertex->vz*vertex->vz);
    vertex->vx = ((vertex->vx)<<12)/len;
    vertex->vy = ((vertex->vy)<<12)/len;
    vertex->vz = ((vertex->vz)<<12)/len;
    ////printf("index %d: normal x %d y %d z %d\n",i, vertex->vx, vertex->vy, vertex->vz);
  }*/
  
  ////printf("checking for skeleton data: %x\n",model->file->skeleton);
  
  if(model->file->skeleton != 0){
    long i,b;
    // Setup Skeleton Data
    model->file->skeleton = (AGM_SKELHEADER*)((char*)addr + ((long)model->file->skeleton));
    
    // Setup pointers to data inside file
    model->file->skeleton->bone_list = (AGM_BONE*)((char*)addr + ((long)model->file->skeleton->bone_list));
    model->file->skeleton->matrix_list = (MATRIX*)((char*)addr + ((long)model->file->skeleton->matrix_list));
    model->file->skeleton->vertex_indices = (u_short*)((char*)addr + ((long)model->file->skeleton->vertex_indices));
    
    ////printf("bone_count=%d\nvertex_count=%d\n",model->file->skeleton->bone_count,model->file->skeleton->vertex_count);
    
    for(i = 0; i<model->file->skeleton->bone_count; i++){
      AGM_BONE * boneptr = &model->file->skeleton->bone_list[i];
      MATRIX * bonemtxptr;
      
      ////printf("matrix_index=%d\nfirst_child=%d\nnext_sibling=%d\nvertex_start=%d\nvertex_count=%d\n",
      //  boneptr->matrix_idx,
      //  boneptr->first_child,
      //  boneptr->next_sibling,
      //  boneptr->vertex_start,
      //  boneptr->vertex_count
      //);
      
      bonemtxptr = &model->file->skeleton->matrix_list[boneptr->matrix_idx];

      //bonemtxptr->t[0] <<=2;
      //bonemtxptr->t[1] <<=2;
      //bonemtxptr->t[2] <<=2;
      
      //PrintMatrix(bonemtxptr);
      
      ////printf("matrix address: %x\n",bonemtxptr);
      
    }
  }
}

/*
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
*/

void AGM_Init(unsigned long buffer_size){
   AGM_TransformBuffer = LStack_Alloc(buffer_size);
   NormalTransformBuffer = LStack_Alloc(buffer_size);
   
   TransformBufferSize = buffer_size;
   
   //printf("AGM_TransformBuffer: %x (size: %d)\n",AGM_TransformBuffer, TransformBufferSize);
}

void AGM_DeInit() {
  //free3(AGM_TransformBuffer);
  TransformBufferSize = 0;
}

void AGM_UnloadModel(AGM_model * model){
}

// Transform Model Into Screen Space/Bones Transformation
// Store data to temporary scratch
void AGM_TransformModel(SVECTOR * source, unsigned short vtx_count) { 
//void AGM_TransformModel(AGM_model * model) {
  unsigned short i, idx;
  //short vtx_count = model->file->vertex_count;
  unsigned short vtx_div3, vtx_rem;
  SVECTOR * vertex_buff_ptr = AGM_TransformBuffer;
  SVECTOR * vertex_source = source;
  
  vtx_div3 = vtx_count / 3;
  vtx_rem = vtx_count % 3;
  ////printf("t vtx count: total=%d, div3=%d, rem=%d\n",vtx_count,vtx_div3,vtx_rem);
  // Transform vertices in groups of 3
  for(i = 0; i < vtx_div3;){
    SVECTOR * z_temp[3];
    long tempz0, tempz1, tempz2;

    /*//printf("vtx %d, v0 x%d y%d z%d, v1 x%d y%d z%d, v2 x%d y%d z%d\n",
    i,
    vertex_source[0].vx, vertex_source[0].vy, vertex_source[0].vz, 
    vertex_source[1].vx, vertex_source[1].vy, vertex_source[1].vz, 
    vertex_source[2].vx, vertex_source[2].vy, vertex_source[2].vz
    );*/
    gte_ldv3c(vertex_source);
    gte_rtpt();
    // I hope this uses the unused cycles after rtpt
    // Advance counter
    i++;
    vertex_source += 3;
    z_temp[0] = vertex_buff_ptr++;
    z_temp[1] = vertex_buff_ptr++;
    z_temp[2] = vertex_buff_ptr++;
    
    // Store Data from GTE registers
    gte_stsxy3((long *)&z_temp[0]->vx,(long *)&z_temp[1]->vx,(long *)&z_temp[2]->vx);

    // TODO: Find a way to directly transfer the contents
    // of the GTE registers to the SVECTOR vz element without need for temporaries.
    gte_stsz3(&tempz0, &tempz1, &tempz2);
    z_temp[0]->vz = tempz0;
    z_temp[1]->vz = tempz1;
    z_temp[2]->vz = tempz2;
  }
  // Transform remaining vertices individually
  for(i = 0; i < vtx_rem;){
    long tempz;
    gte_ldv0(vertex_source);
    gte_rtps();
    vertex_source++;
    i++;
    gte_stsxy((long *)&vertex_buff_ptr->vx);
    gte_stsz(&tempz);
    vertex_buff_ptr->vz = tempz;
    vertex_buff_ptr++;
  }
}


void AGM_TransformByBone(AGM_model * model, MATRIX * mtx, SVECTOR * anim, MATRIX * view){
  // Traverse the bone structure in order to transform each vertex by it's bone
  AGM_SKELHEADER * skeleton = model->file->skeleton;
  
  // Reset the matrix stack pointer to beginning of the stack
  //AGM_MatrixStackPointer = AGM_MatrixStack;
  AGM_MatrixStackPointer = (MATRIX *)0x1f800000;
  
  // Copy base matrix to stack pointer
  *AGM_MatrixStackPointer = *mtx;
 
  // Bone 0 is always the root bone
  AGM_ProcessBone(skeleton->bone_list, 0, model->file->vertex_data,  model->file->normal_data, skeleton->vertex_indices, skeleton->matrix_list, anim, view);

}

void AGM_TransformByBoneCopy(AGM_model * model, MATRIX * mtx, SVECTOR * anim, MATRIX * view, MATRIX * out){
  // Traverse the bone structure in order to transform each vertex by it's bone
  AGM_SKELHEADER * skeleton = model->file->skeleton;
  
  // Reset the matrix stack pointer to beginning of the stack
  //AGM_MatrixStackPointer = AGM_MatrixStack;
  AGM_MatrixStackPointer = (MATRIX *)0x1f800000;
  
  // Copy base matrix to stack pointer
  *AGM_MatrixStackPointer = *mtx;
 
  // Bone 0 is always the root bone
  AGM_ProcessBoneCopy(skeleton->bone_list, 0, model->file->vertex_data, skeleton->vertex_indices, skeleton->matrix_list, anim, view, out);

}

void AGM_TransformBones(AGM_model * model, MATRIX * mtx, SVECTOR * anim, MATRIX * out) {
  // Traverse the bone structure in order to transform each vertex by it's bone
  AGM_SKELHEADER * skeleton = model->file->skeleton;
  
  // Reset the matrix stack pointer to beginning of the stack
  //AGM_MatrixStackPointer = AGM_MatrixStack;
  AGM_MatrixStackPointer = (MATRIX *)0x1f800000;
  
  // Copy base matrix to stack pointer
  *AGM_MatrixStackPointer = *mtx;
 
  // Bone 0 is always the root bone
  AGM_ProcessBoneToWorld(skeleton->bone_list, 0, skeleton->matrix_list, anim, out);
}

// Process all children of the parent bone
void AGM_ProcessBone(AGM_BONE * bones, u_short current_bone, SVECTOR * vtxbuff, SVECTOR * normalbuff, u_short * idxbuff, MATRIX * matrixbuff, SVECTOR * anim, MATRIX * view) {  
  AGM_BONE * cBone;
  MATRIX * parentMat;
  //MATRIX temp;
  MATRIX * temp;
  //SVECTOR anim_vec;
  SVECTOR * anim_vec;

  MATRIX temp_mat;
  SVECTOR temp_anim_vec;
  
  //temp = (MATRIX *)0x1F8003E0;
  //anim_vec = (SVECTOR *)0x1F8003D8;
  temp = (MATRIX *)&temp_mat;
  anim_vec = (SVECTOR *)&temp_anim_vec;
  
  
  // Get bone pointer from bone list
  
  // Set current matrix as the parent matrix
  parentMat = AGM_MatrixStackPointer;
  

  // Advance stack pointer
  AGM_MatrixStackPointer++;

  ////printf("call: bone[%d]\n",current_bone);
  ////printf("addr[%x]\n",AGM_MatrixStackPointer);
  
   while(1) {
    MATRIX * matrix;
    //MATRIX temp;
    //SVECTOR anim_vec;
    //QUATERNION rot_mod;
    
    cBone = &bones[current_bone];
    
    matrix = &matrixbuff[cBone->matrix_idx];
    //temp = *matrix;
    //temp.t[0] = matrix->t[0];
    //temp.t[1] = matrix->t[1];
    //temp.t[2] = matrix->t[2];
    
    ////printf("processing bone[%d]\n",current_bone);
    ////printf("next_sibling[%d]\n",cBone->next_sibling);
    /*if(current_bone <= 9){
      RotMatrix_gte(&temp_animvector,&temp);
      
      gte_CompMatrix(matrix, &temp, AGM_MatrixStackPointer)
      // Make composite matrix from current bone and parent matrix, store to stack
      gte_CompMatrix(parentMat, AGM_MatrixStackPointer, AGM_MatrixStackPointer);
    } else {
      gte_CompMatrix(parentMat, matrix, AGM_MatrixStackPointer);
    }*/
    *anim_vec = anim[current_bone+1];
    //*anim_vec = anim[current_bone+1];
    /*
    if(current_bone == 0 || current_bone == 9)
      anim_vec->vx += model_bend >> 1;
    if(current_bone == 1) {
      anim_vec->vx += model_bend >> 2;
    }

    //anim_vec.vy = 0;
    //anim_vec.vz = 0;
    ////printf("bone[%d] anim_vec: x:%d y:%d z:%d (%d)\n",current_bone,anim_vec.vx,anim_vec.vy,anim_vec.vz,compare_bit);
    if(compare_bit == 0){
      RotMatrixZYX_gte(anim_vec,temp);
    } else {
      RotMatrixZYX(anim_vec,temp);
    }*/


    //if(g_pad & PAD_START) model_bend += 1;
    //if(g_pad & PAD_SELECT) model_bend -= 1;
/*
    if(current_bone == 0){
      SVECTOR rot_mod;
      rot_mod.vx = 0;
      rot_mod.vy = 0;
      rot_mod.vz = -model_bend>>2;
      fix12_toQuaternion((QUATERNION *)&rot_mod, &rot_mod);
      fix12_quaternionMul((QUATERNION *)anim_vec,(QUATERNION *)anim_vec,(QUATERNION *)&rot_mod);
    } else if (current_bone == 1) {
      SVECTOR rot_mod;
      rot_mod.vx = model_bend>>2;
      rot_mod.vy = model_bend>>2;
      rot_mod.vz = 0;
      fix12_toQuaternion((QUATERNION *)&rot_mod, &rot_mod);
      fix12_quaternionMul((QUATERNION *)anim_vec,(QUATERNION *)anim_vec,(QUATERNION *)&rot_mod);
    } else if (current_bone == 9) {
      SVECTOR rot_mod;
      rot_mod.vx = -model_bend>>2;
      rot_mod.vy = model_bend>>3;
      rot_mod.vz = 0;
      fix12_toQuaternion((QUATERNION *)&rot_mod, &rot_mod);
      fix12_quaternionMul((QUATERNION *)anim_vec,(QUATERNION *)anim_vec,(QUATERNION *)&rot_mod);
    }*/

    // Test Quaternion Code
    quaternionGetMatrix((QUATERNION *)anim_vec, temp);

    if(current_bone == 0){
      //temp.t[0] = anim[0].vx;
      //temp.t[1] = anim[0].vy;
      //temp.t[2] = anim[0].vz;
      temp->t[0] = anim[0].vx;
      temp->t[1] = anim[0].vy;
      temp->t[2] = anim[0].vz;
    } else {
      //temp.t[0] = matrix->t[0];
      //temp.t[1] = matrix->t[1];
      //temp.t[2] = matrix->t[2];
      temp->t[0] = matrix->t[0];
      temp->t[1] = matrix->t[1];
      temp->t[2] = matrix->t[2];
    }
    //gte_CompMatrix(parentMat, &temp, AGM_MatrixStackPointer);
    gte_CompMatrix(parentMat, temp, AGM_MatrixStackPointer);

    CompMatrixLV(view, AGM_MatrixStackPointer, temp);
    
    //gte_CompMatrix(parentMat, matrix, AGM_MatrixStackPointer);
    //PrintMatrix(AGM_MatrixStackPointer);
    // Upload current stack matrix to GTE for transformation
    gte_SetTransMatrix(temp);
    gte_SetRotMatrix(temp);
    // Transform vertices for this bone
    AGM_TransformBlock(cBone, vtxbuff, idxbuff, AGM_TransformBuffer);
    //AGM_TransformBlock(cBone, normalbuff, idxbuff, NormalTransformBuffer);
    
    // Check for children
    if(cBone->first_child != 0xFFFF){
      // Recursively process this bone's children
      AGM_ProcessBone(bones, cBone->first_child, vtxbuff, normalbuff, idxbuff, matrixbuff, anim, view);
    }
    
    // Stop loop if current bone has no sibling
    if(cBone->next_sibling == 0xFFFF) break;
    
    // Advance to sibling bone
    current_bone = cBone->next_sibling;
  };
  // Return stack pointer
  AGM_MatrixStackPointer--;
}


void AGM_ProcessBoneCopy(AGM_BONE * bones, u_short current_bone, SVECTOR * vtxbuff, u_short * idxbuff, MATRIX * matrixbuff, SVECTOR * anim, MATRIX * view, MATRIX * out) {  
  AGM_BONE * cBone;
  MATRIX * parentMat;
  //MATRIX temp;
  MATRIX * temp;
  //SVECTOR anim_vec;
  SVECTOR * anim_vec;
  
  temp = (MATRIX *)0x1F8003E0;
  anim_vec = (SVECTOR *)0x1F8003D8;

  // Get bone pointer from bone list
  
  // Set current matrix as the parent matrix
  parentMat = AGM_MatrixStackPointer;
  

  // Advance stack pointer
  AGM_MatrixStackPointer++;
  
   while(1) {
    MATRIX * matrix;    
    cBone = &bones[current_bone];
    
    matrix = &matrixbuff[cBone->matrix_idx];

    *anim_vec = anim[current_bone+1];

    if(current_bone == 0){
      SVECTOR rot_mod;
      rot_mod.vx = model_arch; // Turns back forwards and backwards
      rot_mod.vy = 0;
      rot_mod.vz = -model_bend>>2;
      //FntPrint("ModelArch %d\n",model_arch);
      fix12_toQuaternion((QUATERNION *)&rot_mod, &rot_mod);
      fix12_quaternionMul((QUATERNION *)anim_vec,(QUATERNION *)anim_vec,(QUATERNION *)&rot_mod);
    } else if (current_bone == 1) {
      SVECTOR rot_mod;
      rot_mod.vx = model_bend>>2;
      rot_mod.vy = model_bend>>2;
      rot_mod.vz = model_head_arch; // Turns head up and down
      fix12_toQuaternion((QUATERNION *)&rot_mod, &rot_mod);
      fix12_quaternionMul((QUATERNION *)anim_vec,(QUATERNION *)anim_vec,(QUATERNION *)&rot_mod);
    } else if (current_bone == 9) {
      SVECTOR rot_mod;
      rot_mod.vx = -model_bend>>2;
      rot_mod.vy = model_bend>>3;
      rot_mod.vz = -model_arch;
      fix12_toQuaternion((QUATERNION *)&rot_mod, &rot_mod);
      fix12_quaternionMul((QUATERNION *)anim_vec,(QUATERNION *)anim_vec,(QUATERNION *)&rot_mod);
    }

    // Test Quaternion Code
    quaternionGetMatrix((QUATERNION *)anim_vec, temp);

    if(current_bone == 0){
      temp->t[0] = anim[0].vx;
      temp->t[1] = anim[0].vy;
      temp->t[2] = anim[0].vz;
    } else {
      temp->t[0] = matrix->t[0];
      temp->t[1] = matrix->t[1];
      temp->t[2] = matrix->t[2];
    }
    //gte_CompMatrix(parentMat, &temp, AGM_MatrixStackPointer);
    gte_CompMatrix(parentMat, temp, AGM_MatrixStackPointer);

    out[current_bone] = *AGM_MatrixStackPointer;

    CompMatrixLV(view, AGM_MatrixStackPointer, temp);
    // Upload current stack matrix to GTE for transformation
    gte_SetTransMatrix(temp);
    gte_SetRotMatrix(temp);
    // Transform vertices for this bone
    AGM_TransformBlock(cBone, vtxbuff, idxbuff, AGM_TransformBuffer);
    // Check for children
    if(cBone->first_child != 0xFFFF){
      // Recursively process this bone's children
      AGM_ProcessBoneCopy(bones, cBone->first_child, vtxbuff, idxbuff, matrixbuff, anim, view, out);
    }
    
    // Stop loop if current bone has no sibling
    if(cBone->next_sibling == 0xFFFF) break;
    
    // Advance to sibling bone
    current_bone = cBone->next_sibling;
  };
  // Return stack pointer
  AGM_MatrixStackPointer--;
}

void AGM_ProcessBoneToWorld(AGM_BONE * bones, u_short current_bone, MATRIX * matrixbuff, SVECTOR * anim, MATRIX * out) {  
  AGM_BONE * cBone;
  MATRIX * parentMat;
  //MATRIX temp;
  MATRIX * temp;
  //SVECTOR anim_vec;
  SVECTOR * anim_vec;
  
  temp = (MATRIX *)0x1F8003E0;
  anim_vec = (SVECTOR *)0x1F8003D8;

  // Get bone pointer from bone list
  
  // Set current matrix as the parent matrix
  parentMat = AGM_MatrixStackPointer;

  // Advance stack pointer
  AGM_MatrixStackPointer++;
  
   while(1) {
    MATRIX * matrix;    
    cBone = &bones[current_bone];
    
    matrix = &matrixbuff[cBone->matrix_idx];

    *anim_vec = anim[current_bone+1];

    if(current_bone == 0){
      SVECTOR rot_mod;
      rot_mod.vx = model_arch; // Turns back forwards and backwards
      rot_mod.vy = 0;
      rot_mod.vz = -model_bend>>2;
      //FntPrint("ModelArch %d\n",model_arch);
      fix12_toQuaternion((QUATERNION *)&rot_mod, &rot_mod);
      fix12_quaternionMul((QUATERNION *)anim_vec,(QUATERNION *)anim_vec,(QUATERNION *)&rot_mod);
    } else if (current_bone == 1) {
      SVECTOR rot_mod;
      rot_mod.vx = model_bend>>2;
      rot_mod.vy = model_bend>>2;
      rot_mod.vz = model_head_arch; // Turns head up and down
      fix12_toQuaternion((QUATERNION *)&rot_mod, &rot_mod);
      fix12_quaternionMul((QUATERNION *)anim_vec,(QUATERNION *)anim_vec,(QUATERNION *)&rot_mod);
    } else if (current_bone == 9) {
      SVECTOR rot_mod;
      rot_mod.vx = -model_bend>>2;
      rot_mod.vy = model_bend>>3;
      rot_mod.vz = -model_arch;
      fix12_toQuaternion((QUATERNION *)&rot_mod, &rot_mod);
      fix12_quaternionMul((QUATERNION *)anim_vec,(QUATERNION *)anim_vec,(QUATERNION *)&rot_mod);
    }

    // Test Quaternion Code
    quaternionGetMatrix((QUATERNION *)anim_vec, temp);

    if(current_bone == 0){
      temp->t[0] = anim[0].vx;
      temp->t[1] = anim[0].vy;
      temp->t[2] = anim[0].vz;
    } else {
      temp->t[0] = matrix->t[0];
      temp->t[1] = matrix->t[1];
      temp->t[2] = matrix->t[2];
    }
    //gte_CompMatrix(parentMat, &temp, AGM_MatrixStackPointer);
    //static VECTOR scale = { 4096>>1, 4096>>1, 4096>>1, 0 };
    //ScaleMatrixL(temp, &scale);
    gte_CompMatrix(parentMat, temp, AGM_MatrixStackPointer);
    
    //CompMatrixLV(parentMat, temp, AGM_MatrixStackPointer);

    out[current_bone] = *AGM_MatrixStackPointer;

    // Check for children
    if(cBone->first_child != 0xFFFF){
      // Recursively process this bone's children
      AGM_ProcessBoneToWorld(bones, cBone->first_child, matrixbuff, anim, out);
    }
    
    // Stop loop if current bone has no sibling
    if(cBone->next_sibling == 0xFFFF) break;
    
    // Advance to sibling bone
    current_bone = cBone->next_sibling;
  };
  // Return stack pointer
  AGM_MatrixStackPointer--;
}
// AGM_model * model, MATRIX * mtx

void AGM_TransformModelOnly(AGM_model * model, MATRIX * mtx, MATRIX * view, MATRIX * light) {
  // Traverse the bone structure in order to transform each vertex by it's bone
  AGM_SKELHEADER * skeleton = model->file->skeleton;
  
  // Reset the matrix stack pointer to beginning of the stack
  //AGM_MatrixStackPointer = AGM_MatrixStack;
  AGM_MatrixStackPointer = (MATRIX *)0x1f800000;
  
  // Copy base matrix to stack pointer
  *AGM_MatrixStackPointer = *mtx;


 //for(int i = 0; i < model->file->vertex_count; i++) {
 //  NormalTransformBuffer[i] = model->file->normal_data[i];
 //}

  // Bone 0 is always the root bone
  AGM_ProcessTransformModel(skeleton->bone_list, 0, model->file->vertex_data, model->file->normal_data, skeleton->vertex_indices, mtx, view, light);
}

void AGM_ProcessTransformModel(AGM_BONE * bones, u_short current_bone, SVECTOR * vtxbuff, SVECTOR * normalbuff, u_short * idxbuff, MATRIX * matrixbuff, MATRIX * view, MATRIX * light) {
  AGM_BONE * cBone;
  MATRIX * temp = (MATRIX *)0x1F800000;

  while(1) {
    MATRIX * matrix;    
    cBone = &bones[current_bone];
    
    // Read LocalToWorld Matrix
    matrix = &matrixbuff[cBone->matrix_idx];

    //gte_SetTransMatrix(temp);
    //gte_SetRotMatrix(temp);
    //AGM_TransformBlock(cBone, normalbuff, idxbuff, NormalTransformBuffer);

    /*MATRIX debug_color = {
      4096, 0, 0,
      0, 4096, 0,
      0, 0, 4096
    };

    MATRIX tempmp = {
      4096, 0, 0,
      0, 4096, 0,
      0, 0, 4096
    };*/

    /*temp->m[0][1] = 0;
    temp->m[0][2] = 0;
    temp->m[1][0] = 0;
    temp->m[1][2] = 0;
    temp->m[2][0] = 0;
    temp->m[2][1] = 0;
    temp->m[2][2] = temp->m[1][1] = temp->m[0][0] = 4096;*/
    //MulMatrix0(light, &tempmp, temp);
    //MulMatrix0(&tempmp, matrix, temp);
    MulMatrix0(light, matrix, temp);
    //MulMatrix0(&tempmp, light, temp);
    //MulMatrix0(&tempmp, matrix, temp);

    //gte_SetColorMatrix(&debug_color);
    //gte_SetLightMatrix(temp);
    gte_SetLightMatrix(temp);

    AGM_LightBlock(cBone, normalbuff, idxbuff, NormalTransformBuffer);

    matrix = &matrixbuff[cBone->matrix_idx];

    CompMatrixLV(view, matrix, temp);
    
    // Upload current stack matrix to GTE for transformation
    gte_SetTransMatrix(temp);
    gte_SetRotMatrix(temp);
    // Transform vertices for this bone
    AGM_TransformBlock(cBone, vtxbuff, idxbuff, AGM_TransformBuffer);
    // Check for children
    if(cBone->first_child != 0xFFFF){
      // Recursively process this bone's children
      AGM_ProcessTransformModel(bones, cBone->first_child, vtxbuff, normalbuff, idxbuff, matrixbuff, view, light);
    }
    
    // Stop loop if current bone has no sibling
    if(cBone->next_sibling == 0xFFFF) break;
    
    // Advance to sibling bone
    current_bone = cBone->next_sibling;
  };
}


void AGM_TransformBlock(AGM_BONE * bone, SVECTOR * vtxbuff, u_short * idxbuff, SVECTOR * dest){
  long i;
  short vtx_div3, vtx_rem, vtx_current;
  u_short * vertex_indices;
  
  vtx_div3 = bone->vertex_count / 3;
  vtx_rem = bone->vertex_count % 3;
  
  ////printf("bone count=%d\n",bone->vertex_count);
  ////printf("bone vtx_div3=%d vtx_rem=%d\n",vtx_div3,vtx_rem);
  
  vertex_indices = &idxbuff[bone->vertex_start];
  
  // Transform vertices in groups of 3
  for(i = 0; i < vtx_div3;){
    SVECTOR * src_vec[3];
    SVECTOR * dest_vec[3];
    long tempz0, tempz1, tempz2;
    u_short idx_list[3];
    
    // Get Vertex Indices
    idx_list[0] = *vertex_indices++;
    idx_list[1] = *vertex_indices++;
    idx_list[2] = *vertex_indices++;
    
    // Get Pointer to Vertices from Vertex Indices
    src_vec[0] = &vtxbuff[idx_list[0]];
    src_vec[1] = &vtxbuff[idx_list[1]];
    src_vec[2] = &vtxbuff[idx_list[2]];
    
    // Use GTE to transform vertices
    gte_ldv3(src_vec[0],src_vec[1],src_vec[2]);
    gte_rtpt();
    // Use idle cycles waiting for the GTE to finish to do something else
    i++;
    // Get pointer to transform buffer address
    dest_vec[0] = &dest[idx_list[0]];
    dest_vec[1] = &dest[idx_list[1]];
    dest_vec[2] = &dest[idx_list[2]];

    gte_stsxy3((long *)&dest_vec[0]->vx,(long *)&dest_vec[1]->vx,(long *)&dest_vec[2]->vx);
    
    // TODO: Find a way to directly transfer the contents
    // of the GTE registers to the SVECTOR vz element without need for temporaries.
    gte_stsz3(&tempz0, &tempz1, &tempz2);
    dest_vec[0]->vz = tempz0;
    dest_vec[1]->vz = tempz1;
    dest_vec[2]->vz = tempz2;
  }
  // Transform remaining vertices individually
  for(i = 0; i < vtx_rem;){
    SVECTOR * src_vec;
    SVECTOR * dest_vec;
    long tempz;
    short idx;
    
    idx = *vertex_indices++;
    src_vec = &vtxbuff[idx];

    gte_ldv0(src_vec);
    gte_rtps();
    i++;
    dest_vec = &dest[idx];
    gte_stsxy((long *)&dest_vec->vx);
    gte_stsz(&tempz);
    dest_vec->vz = tempz;
  }
}

void AGM_LightBlock(AGM_BONE * bone, SVECTOR * normalbuff, u_short * idxbuff, CVECTOR * dest){
  long i;
  short vtx_div3, vtx_rem, vtx_current;
  u_short * vertex_indices;
  
  vtx_div3 = bone->vertex_count / 3;
  vtx_rem = bone->vertex_count % 3;
  
  ////printf("bone count=%d\n",bone->vertex_count);
  ////printf("bone vtx_div3=%d vtx_rem=%d\n",vtx_div3,vtx_rem);
  
  vertex_indices = &idxbuff[bone->vertex_start];

  long neutralrgb = 0x00808080;
  gte_ldrgb(&neutralrgb);
  
  // Transform vertices in groups of 3
  for(i = 0; i < vtx_div3;){
    SVECTOR * src_vec[3];
    CVECTOR * dest_vec[3];
    u_short idx_list[3];
    
    // Get Vertex Indices
    idx_list[0] = *vertex_indices++;
    idx_list[1] = *vertex_indices++;
    idx_list[2] = *vertex_indices++;
    
    // Get Pointer to Vertices from Vertex Indices
    src_vec[0] = &normalbuff[idx_list[0]];
    src_vec[1] = &normalbuff[idx_list[1]];
    src_vec[2] = &normalbuff[idx_list[2]];
    
    // Use GTE to transform vertices
    gte_ldv3(src_vec[0],src_vec[1],src_vec[2]);
    gte_ncct();
    i++;
    // Get pointer to transform buffer address
    dest_vec[0] = &dest[idx_list[0]];
    dest_vec[1] = &dest[idx_list[1]];
    dest_vec[2] = &dest[idx_list[2]];

    gte_strgb3(dest_vec[0], dest_vec[1], dest_vec[2]);
  }
  // Transform remaining vertices individually
  for(i = 0; i < vtx_rem;){
    SVECTOR * src_vec;
    CVECTOR * dest_vec;
    long tempz;
    short idx;
    
    idx = *vertex_indices++;
    src_vec = &normalbuff[idx];

    gte_ldv0(src_vec);
    gte_nccs();
    i++;
    dest_vec = &dest[idx];
    gte_strgb(dest_vec);
  }
}

// Generate Ordering Table entries
// Must be called after AGM_TransformModel() for the same model
u_char * AGM_DrawModel(AGM_model * model, u_char * packet_ptr, u_long * ot, short zoffset) {
  AGM_FILE * file = model->file;
  AGM_POLYG4 * pg4_ptr = file->poly_g4;
  AGM_POLYG3 * pg3_ptr = file->poly_g3;
  AGM_POLYGT4 * pgt4_ptr = file->poly_gt4;
  AGM_POLYGT3 * pgt3_ptr = file->poly_gt3;
  POLY_G4 * dest_pg4_ptr = (POLY_G4*)packet_ptr;
  POLY_G3 * dest_pg3_ptr;
  POLY_GT4 * dest_pgt4_ptr;
  POLY_GT3 * dest_pgt3_ptr;
  long outer_product, otz;
  long tempz0,tempz1,tempz2,tempz3;
  short pg4_count = file->poly_g4_count;
  short pg3_count = file->poly_g3_count;
  short pgt4_count = file->poly_gt4_count;
  short pgt3_count = file->poly_gt3_count;
  short i;
  u_short tpageid;
  //u_short clutid[] = {GetClut(128,251), GetClut(128,252),GetClut(128,254),GetClut(128,253),GetClut(128,255)};
  u_short clutid[] = {GetClut(0,256+251), GetClut(0,256+252),GetClut(0,256+254),GetClut(0,256+253),GetClut(0,256+255)};

  tpageid = getTPage(0, 0, 0, 256);
  //clutid = GetClut(128,251);
  
  for(i = 0; i < pg4_count; i++, pg4_ptr++) {
      SVECTOR * vec0 = &AGM_TransformBuffer[pg4_ptr->idx0];
      SVECTOR * vec1 = &AGM_TransformBuffer[pg4_ptr->idx1];
      SVECTOR * vec2 = &AGM_TransformBuffer[pg4_ptr->idx2];
      SVECTOR * vec3 = &AGM_TransformBuffer[pg4_ptr->idx3];
      
      // Clip quads that are offscreen before any other operation
      //if(quad_clip( (DVECTOR*)vec0,
      //              (DVECTOR*)vec1,
      //              (DVECTOR*)vec2,
      //              (DVECTOR*)vec3)) continue;
      
      // Load screen XY coordinates to GTE Registers
      gte_ldsxy3(*(long*)&vec0->vx,*(long*)&vec1->vx,*(long*)&vec2->vx);
      // Perform clipping calculation
      gte_nclip();
      // Store Outer Product
      gte_stopz(&outer_product);
      // Check side
      if(outer_product <= 0) continue; // Skip back facing polys
      ////printf("rendering face %d\n",i);
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
      //otz = 50;
      if (otz > OTMINCHAR && otz < OTSIZE) {
        long temp0, temp1, temp2, temp3;
        // If all tests have passed, now process the rest of the primitive.
        // Copy values already in the registers
        gte_stsxy3((long*)&dest_pg4_ptr->x0,
                   (long*)&dest_pg4_ptr->x1,
                   (long*)&dest_pg4_ptr->x2);
        // Store 4th vertex from buffer directly
        //*(long*)(&dest_pg4_ptr->x0) = *(long*)(&vec0->vx);
        //*(long*)(&dest_pg4_ptr->x1) = *(long*)(&vec1->vx);
        //*(long*)(&dest_pg4_ptr->x2) = *(long*)(&vec2->vx);
        
        temp0 = *(long*)(&vec3->vx);
        *(long*)(&dest_pg4_ptr->x3) = temp0;
        //*(long*)(&dest_pg4_ptr->x3) = *(long*)(&vec3->vx);
        // Vertex Colors
        /*temp1 = *(long*)(&pg4_ptr->r0);
        temp2 = *(long*)(&pg4_ptr->r1);
        temp3 = *(long*)(&pg4_ptr->r2);
        temp0 = *(long*)(&pg4_ptr->r3);
        *(long*)(&dest_pg4_ptr->r0) = temp1;
        *(long*)(&dest_pg4_ptr->r1) = temp2;
        *(long*)(&dest_pg4_ptr->r2) = temp3;
        *(long*)(&dest_pg4_ptr->r3) = temp0;*/

        /*gte_ldrgb((CVECTOR*)&pg4_ptr->r0);
        gte_cc();
        gte_strgb((CVECTOR*)&dest_pg4_ptr->r0);
        gte_ldrgb((CVECTOR*)&pg4_ptr->r1);
        gte_cc();
        gte_strgb((CVECTOR*)&dest_pg4_ptr->r1);
        gte_ldrgb((CVECTOR*)&pg4_ptr->r2);
        gte_cc();
        gte_strgb((CVECTOR*)&dest_pg4_ptr->r2);
        gte_ldrgb((CVECTOR*)&pg4_ptr->r3);
        gte_cc();
        gte_strgb((CVECTOR*)&dest_pg4_ptr->r3);*/

        /*gte_ldv3(
            &NormalTransformBuffer[pg4_ptr->idx0],
            &NormalTransformBuffer[pg4_ptr->idx1],
            &NormalTransformBuffer[pg4_ptr->idx2]
            );
        gte_ldrgb((CVECTOR*)&pg4_ptr->r0);
        gte_ncct();
        gte_strgb3(&dest_pg4_ptr->r0, &dest_pg4_ptr->r1, &dest_pg4_ptr->r2);

        gte_ldv0(&NormalTransformBuffer[pg4_ptr->idx3]);
        gte_nccs();
        gte_strgb(&dest_pg4_ptr->r3);*/

        *(CVECTOR*)&dest_pg4_ptr->r0 = *(CVECTOR*)&NormalTransformBuffer[pg4_ptr->idx0];
        *(CVECTOR*)&dest_pg4_ptr->r1 = *(CVECTOR*)&NormalTransformBuffer[pg4_ptr->idx1];
        *(CVECTOR*)&dest_pg4_ptr->r2 = *(CVECTOR*)&NormalTransformBuffer[pg4_ptr->idx2];
        *(CVECTOR*)&dest_pg4_ptr->r3 = *(CVECTOR*)&NormalTransformBuffer[pg4_ptr->idx3];

        /*dest_pg4_ptr->r0 = (pg4_ptr->r0 * 64) >> 8;
        dest_pg4_ptr->g0 = (pg4_ptr->g0 * 64) >> 8;
        dest_pg4_ptr->b0 = (pg4_ptr->b0 * 64) >> 8;

        dest_pg4_ptr->r1 = (pg4_ptr->r1 * 64) >> 8;
        dest_pg4_ptr->g1 = (pg4_ptr->g1 * 64) >> 8;
        dest_pg4_ptr->b1 = (pg4_ptr->b1 * 64) >> 8;

        dest_pg4_ptr->r2 = (pg4_ptr->r2 * 64) >> 8;
        dest_pg4_ptr->g2 = (pg4_ptr->g2 * 64) >> 8;
        dest_pg4_ptr->b2 = (pg4_ptr->b2 * 64) >> 8;

        dest_pg4_ptr->r3 = (pg4_ptr->r3 * 64) >> 8;
        dest_pg4_ptr->g3 = (pg4_ptr->g3 * 64) >> 8;
        dest_pg4_ptr->b3 = (pg4_ptr->b3 * 64) >> 8;*/

        /**(long*)(&dest_pg4_ptr->r0) = *(long*)(&pg4_ptr->r0);
        *(long*)(&dest_pg4_ptr->r1) = *(long*)(&pg4_ptr->r1);
        *(long*)(&dest_pg4_ptr->r2) = *(long*)(&pg4_ptr->r2);
        *(long*)(&dest_pg4_ptr->r3) = *(long*)(&pg4_ptr->r3);*/

        setPolyG4(dest_pg4_ptr);
        
        ////printf("Poly Address: %x\n",dest_pg4_ptr);
        
        // Add primitive to Ordering Table and advance pointer
        // to point to the next primitive to be processed.
        addPrim(ot+otz+zoffset, dest_pg4_ptr);
        dest_pg4_ptr++;
      }
      
  }
  dest_pg3_ptr = (POLY_G3*)dest_pg4_ptr;
  
  ////printf("rendering tris count[%d]\n",pg3_count);
  
  for(i = 0; i < pg3_count; i++, pg3_ptr++) {
      SVECTOR * vec0 = &AGM_TransformBuffer[pg3_ptr->idx0];
      SVECTOR * vec1 = &AGM_TransformBuffer[pg3_ptr->idx1];
      SVECTOR * vec2 = &AGM_TransformBuffer[pg3_ptr->idx2];
      
      // Clip quads that are offscreen before any other operation
      //if(tri_clip((DVECTOR*)vec0,
      //            (DVECTOR*)vec1,
      //            (DVECTOR*)vec2)) continue;
      
      // Load screen XY coordinates to GTE Registers
      gte_ldsxy3(*(long*)&vec0->vx,*(long*)&vec1->vx,*(long*)&vec2->vx);
      // Perform clipping calculation
      gte_nclip();
      // Store Outer Product
      gte_stopz(&outer_product);
      // Check side
      if(outer_product <= 0) continue; // Skip back facing polys
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
      otz = (otz >> OTSUBDIV);
      //otz = 50;
      if (otz > OTMINCHAR && otz < OTSIZE) {
        // If all tests have passed, now process the rest of the primitive.
        // Copy values already in the registers
        gte_stsxy3((long*)&dest_pg3_ptr->x0,
                   (long*)&dest_pg3_ptr->x1,
                   (long*)&dest_pg3_ptr->x2);
        // Vertex Colors
        //*(long*)(&dest_pg3_ptr->r0) = *(long*)(&pg3_ptr->r0);
        //*(long*)(&dest_pg3_ptr->r1) = *(long*)(&pg3_ptr->r1);
        //*(long*)(&dest_pg3_ptr->r2) = *(long*)(&pg3_ptr->r2);

        /*gte_ldrgb((CVECTOR*)&pg3_ptr->r0);
        gte_cc();
        gte_strgb((CVECTOR*)&dest_pg3_ptr->r0);
        gte_ldrgb((CVECTOR*)&pg3_ptr->r1);
        gte_cc();
        gte_strgb((CVECTOR*)&dest_pg3_ptr->r1);
        gte_ldrgb((CVECTOR*)&pg3_ptr->r2);
        gte_cc();
        gte_strgb((CVECTOR*)&dest_pg3_ptr->r2);*/

        /*gte_ldv3(
            &NormalTransformBuffer[pg3_ptr->idx0],
            &NormalTransformBuffer[pg3_ptr->idx1],
            &NormalTransformBuffer[pg3_ptr->idx2]
            );
        gte_ldrgb((CVECTOR*)&pg3_ptr->r0);
        gte_ncct();
        gte_strgb3(&dest_pg3_ptr->r0, &dest_pg3_ptr->r1, &dest_pg3_ptr->r2);*/

        *(CVECTOR*)&dest_pg3_ptr->r0 = *(CVECTOR*)&NormalTransformBuffer[pg3_ptr->idx0];
        *(CVECTOR*)&dest_pg3_ptr->r1 = *(CVECTOR*)&NormalTransformBuffer[pg3_ptr->idx1];
        *(CVECTOR*)&dest_pg3_ptr->r2 = *(CVECTOR*)&NormalTransformBuffer[pg3_ptr->idx2];

        /*dest_pg3_ptr->r0 = (pg3_ptr->r0 * 64) >> 8;
        dest_pg3_ptr->g0 = (pg3_ptr->g0 * 64) >> 8;
        dest_pg3_ptr->b0 = (pg3_ptr->b0 * 64) >> 8;

        dest_pg3_ptr->r1 = (pg3_ptr->r1 * 64) >> 8;
        dest_pg3_ptr->g1 = (pg3_ptr->g1 * 64) >> 8;
        dest_pg3_ptr->b1 = (pg3_ptr->b1 * 64) >> 8;

        dest_pg3_ptr->r2 = (pg3_ptr->r2 * 64) >> 8;
        dest_pg3_ptr->g2 = (pg3_ptr->g2 * 64) >> 8;
        dest_pg3_ptr->b2 = (pg3_ptr->b2 * 64) >> 8;*/

        setPolyG3(dest_pg3_ptr);
        
        // Add primitive to Ordering Table and advance pointer
        // to point to the next primitive to be processed.
        addPrim(ot+otz+zoffset, dest_pg3_ptr);
        dest_pg3_ptr++;
      }
      
  }

  dest_pgt4_ptr = (POLY_GT4*)dest_pg3_ptr;

  for(i = 0; i < pgt4_count; i++, pgt4_ptr++) {
      SVECTOR * vec0 = &AGM_TransformBuffer[pgt4_ptr->idx0];
      SVECTOR * vec1 = &AGM_TransformBuffer[pgt4_ptr->idx1];
      SVECTOR * vec2 = &AGM_TransformBuffer[pgt4_ptr->idx2];
      SVECTOR * vec3 = &AGM_TransformBuffer[pgt4_ptr->idx3];
      
      // Clip quads that are offscreen before any other operation
      //if(quad_clip( (DVECTOR*)vec0,
      //              (DVECTOR*)vec1,
      //              (DVECTOR*)vec2,
      //              (DVECTOR*)vec3)) continue;
      
      // Load screen XY coordinates to GTE Registers
      gte_ldsxy3(*(long*)&vec0->vx,*(long*)&vec1->vx,*(long*)&vec2->vx);
      // Perform clipping calculation
      gte_nclip();
      // Store Outer Product
      gte_stopz(&outer_product);
      // Check side
      if(outer_product <= 0) continue; // Skip back facing polys
      ////printf("rendering face %d\n",i);
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
      //otz = 50;
      if (otz > OTMINCHAR && otz < OTSIZE) {
        long temp0, temp1, temp2, temp3;
        // If all tests have passed, now process the rest of the primitive.
        // Copy values already in the registers
        gte_stsxy3((long*)&dest_pgt4_ptr->x0,
                   (long*)&dest_pgt4_ptr->x1,
                   (long*)&dest_pgt4_ptr->x2);
        // Store 4th vertex from buffer directly
        //*(long*)(&dest_pgt4_ptr->x0) = *(long*)(&vec0->vx);
        //*(long*)(&dest_pgt4_ptr->x1) = *(long*)(&vec1->vx);
        //*(long*)(&dest_pgt4_ptr->x2) = *(long*)(&vec2->vx);
        temp0 = *(long*)(&vec3->vx);
        *(long*)(&dest_pgt4_ptr->x3) = temp0;
        //*(long*)(&dest_pgt4_ptr->x3) = *(long*)(&vec3->vx);
        // Vertex Colors


        /*temp1 = *(long*)(&pgt4_ptr->r0);
        temp2 = *(long*)(&pgt4_ptr->r1);
        temp3 = *(long*)(&pgt4_ptr->r2);
        temp0 = *(long*)(&pgt4_ptr->r3);
        *(long*)(&dest_pgt4_ptr->r0) = temp1;
        *(long*)(&dest_pgt4_ptr->r1) = temp2;
        *(long*)(&dest_pgt4_ptr->r2) = temp3;
        *(long*)(&dest_pgt4_ptr->r3) = temp0;*/

        /*gte_ldrgb((CVECTOR*)&pgt4_ptr->r0);
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
        gte_strgb((CVECTOR*)&dest_pgt4_ptr->r3);*/

        /*gte_ldv3(
            &NormalTransformBuffer[pgt4_ptr->idx0],
            &NormalTransformBuffer[pgt4_ptr->idx1],
            &NormalTransformBuffer[pgt4_ptr->idx2]
            );
        gte_ldrgb((CVECTOR*)&pgt4_ptr->r0);
        gte_ncct();
        gte_strgb3(&dest_pgt4_ptr->r0, &dest_pgt4_ptr->r1, &dest_pgt4_ptr->r2);

        gte_ldv0(&NormalTransformBuffer[pgt4_ptr->idx3]);
        gte_nccs();
        gte_strgb(&dest_pgt4_ptr->r3);*/

        *(CVECTOR*)&dest_pgt4_ptr->r0 = *(CVECTOR*)&NormalTransformBuffer[pgt4_ptr->idx0];
        *(CVECTOR*)&dest_pgt4_ptr->r1 = *(CVECTOR*)&NormalTransformBuffer[pgt4_ptr->idx1];
        *(CVECTOR*)&dest_pgt4_ptr->r2 = *(CVECTOR*)&NormalTransformBuffer[pgt4_ptr->idx2];
        *(CVECTOR*)&dest_pgt4_ptr->r3 = *(CVECTOR*)&NormalTransformBuffer[pgt4_ptr->idx3];

        /*dest_pgt4_ptr->r0 = (pgt4_ptr->r0 * 64) >> 8;
        dest_pgt4_ptr->g0 = (pgt4_ptr->g0 * 64) >> 8;
        dest_pgt4_ptr->b0 = (pgt4_ptr->b0 * 64) >> 8;

        dest_pgt4_ptr->r1 = (pgt4_ptr->r1 * 64) >> 8;
        dest_pgt4_ptr->g1 = (pgt4_ptr->g1 * 64) >> 8;
        dest_pgt4_ptr->b1 = (pgt4_ptr->b1 * 64) >> 8;

        dest_pgt4_ptr->r2 = (pgt4_ptr->r2 * 64) >> 8;
        dest_pgt4_ptr->g2 = (pgt4_ptr->g2 * 64) >> 8;
        dest_pgt4_ptr->b2 = (pgt4_ptr->b2 * 64) >> 8;

        dest_pgt4_ptr->r3 = (pgt4_ptr->r3 * 64) >> 8;
        dest_pgt4_ptr->g3 = (pgt4_ptr->g3 * 64) >> 8;
        dest_pgt4_ptr->b3 = (pgt4_ptr->b3 * 64) >> 8;*/
        /*
        *(long*)(&dest_pgt4_ptr->r0) = *(long*)(&pgt4_ptr->r0);
        *(long*)(&dest_pgt4_ptr->r1) = *(long*)(&pgt4_ptr->r1);
        *(long*)(&dest_pgt4_ptr->r2) = *(long*)(&pgt4_ptr->r2);
        *(long*)(&dest_pgt4_ptr->r3) = *(long*)(&pgt4_ptr->r3);
        */

        // Texture Coordinates
        // Set Texture Coordinates
        temp1 = *(short*)(&pgt4_ptr->u0);
        temp2 = *(short*)(&pgt4_ptr->u1);
        temp3 = *(short*)(&pgt4_ptr->u2);
        temp0 = *(short*)(&pgt4_ptr->u3);
        *(short*)(&dest_pgt4_ptr->u0) = temp1;
        *(short*)(&dest_pgt4_ptr->u1) = temp2;
        *(short*)(&dest_pgt4_ptr->u2) = temp3;
        *(short*)(&dest_pgt4_ptr->u3) = temp0;
        /*
        *(short*)(&dest_pgt4_ptr->u0) = *(short*)(&pgt4_ptr->u0);
        *(short*)(&dest_pgt4_ptr->u1) = *(short*)(&pgt4_ptr->u1);
        *(short*)(&dest_pgt4_ptr->u2) = *(short*)(&pgt4_ptr->u2);
        *(short*)(&dest_pgt4_ptr->u3) = *(short*)(&pgt4_ptr->u3);
        */

        // Set CLUT
        dest_pgt4_ptr->clut = clutid[pgt4_ptr->clutid];
        // Set Texture Page
        dest_pgt4_ptr->tpage = tpageid;

        setPolyGT4(dest_pgt4_ptr);
        
        ////printf("Poly Address: %x\n",dest_pgt4_ptr);
        
        // Add primitive to Ordering Table and advance pointer
        // to point to the next primitive to be processed.
        addPrim(ot+otz+zoffset, dest_pgt4_ptr);
        dest_pgt4_ptr++;
      }
      
  }
  dest_pgt3_ptr = (POLY_GT3*)dest_pgt4_ptr;
  
  for(i = 0; i < pgt3_count; i++, pgt3_ptr++) {
      SVECTOR * vec0 = &AGM_TransformBuffer[pgt3_ptr->idx0];
      SVECTOR * vec1 = &AGM_TransformBuffer[pgt3_ptr->idx1];
      SVECTOR * vec2 = &AGM_TransformBuffer[pgt3_ptr->idx2];
      
      // Clip quads that are offscreen before any other operation
      //if(tri_clip((DVECTOR*)vec0,
      //            (DVECTOR*)vec1,
      //            (DVECTOR*)vec2)) continue;
      
      // Load screen XY coordinates to GTE Registers
      gte_ldsxy3(*(long*)&vec0->vx,*(long*)&vec1->vx,*(long*)&vec2->vx);
      // Perform clipping calculation
      gte_nclip();
      // Store Outer Product
      gte_stopz(&outer_product);
      // Check side
      if(outer_product <= 0) continue; // Skip back facing polys
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
      otz = (otz >> OTSUBDIV);
      //otz = 50;
      if (otz > OTMINCHAR && otz < OTSIZE) {
        long temp0, temp1, temp2;
        // If all tests have passed, now process the rest of the primitive.
        // Copy values already in the registers
        gte_stsxy3((long*)&dest_pgt3_ptr->x0,
                   (long*)&dest_pgt3_ptr->x1,
                   (long*)&dest_pgt3_ptr->x2);
        // Vertex Colors
        /*temp0 = *(long*)(&pgt3_ptr->r0);
        temp1 = *(long*)(&pgt3_ptr->r1);
        temp2 = *(long*)(&pgt3_ptr->r2);
        *(long*)(&dest_pgt3_ptr->r0) = temp0;
        *(long*)(&dest_pgt3_ptr->r1) = temp1;
        *(long*)(&dest_pgt3_ptr->r2) = temp2;*/
        /*gte_ldrgb((CVECTOR*)&pgt3_ptr->r0);
        gte_cc();
        gte_strgb((CVECTOR*)&dest_pgt3_ptr->r0);
        gte_ldrgb((CVECTOR*)&pgt3_ptr->r1);
        gte_cc();
        gte_strgb((CVECTOR*)&dest_pgt3_ptr->r1);
        gte_ldrgb((CVECTOR*)&pgt3_ptr->r2);
        gte_cc();
        gte_strgb((CVECTOR*)&dest_pgt3_ptr->r2);*/

        /*gte_ldv3(
            &NormalTransformBuffer[pgt3_ptr->idx0],
            &NormalTransformBuffer[pgt3_ptr->idx1],
            &NormalTransformBuffer[pgt3_ptr->idx2]
            );
        gte_ldrgb((CVECTOR*)&pgt3_ptr->r0);
        gte_ncct();
        gte_strgb3(&dest_pgt3_ptr->r0, &dest_pgt3_ptr->r1, &dest_pgt3_ptr->r2);*/

        *(CVECTOR*)&dest_pgt3_ptr->r0 = *(CVECTOR*)&NormalTransformBuffer[pgt3_ptr->idx0];
        *(CVECTOR*)&dest_pgt3_ptr->r1 = *(CVECTOR*)&NormalTransformBuffer[pgt3_ptr->idx1];
        *(CVECTOR*)&dest_pgt3_ptr->r2 = *(CVECTOR*)&NormalTransformBuffer[pgt3_ptr->idx2];

        /*dest_pgt3_ptr->r0 = (pgt3_ptr->r0 * 64) >> 8;
        dest_pgt3_ptr->g0 = (pgt3_ptr->g0 * 64) >> 8;
        dest_pgt3_ptr->b0 = (pgt3_ptr->b0 * 64) >> 8;

        dest_pgt3_ptr->r1 = (pgt3_ptr->r1 * 64) >> 8;
        dest_pgt3_ptr->g1 = (pgt3_ptr->g1 * 64) >> 8;
        dest_pgt3_ptr->b1 = (pgt3_ptr->b1 * 64) >> 8;

        dest_pgt3_ptr->r2 = (pgt3_ptr->r2 * 64) >> 8;
        dest_pgt3_ptr->g2 = (pgt3_ptr->g2 * 64) >> 8;
        dest_pgt3_ptr->b2 = (pgt3_ptr->b2 * 64) >> 8;*/

        /*
        *(long*)(&dest_pgt3_ptr->r0) = *(long*)(&pgt3_ptr->r0);
        *(long*)(&dest_pgt3_ptr->r1) = *(long*)(&pgt3_ptr->r1);
        *(long*)(&dest_pgt3_ptr->r2) = *(long*)(&pgt3_ptr->r2);
        */

        // Set Texture Coordinates
        temp0 = *(short*)(&pgt3_ptr->u0);
        temp1 = *(short*)(&pgt3_ptr->u1);
        temp2 = *(short*)(&pgt3_ptr->u2);
        *(short*)(&dest_pgt3_ptr->u0) = temp0;
        *(short*)(&dest_pgt3_ptr->u1) = temp1;
        *(short*)(&dest_pgt3_ptr->u2) = temp2;

        /*
        *(short*)(&dest_pgt3_ptr->u0) = *(short*)(&pgt3_ptr->u0);
        *(short*)(&dest_pgt3_ptr->u1) = *(short*)(&pgt3_ptr->u1);
        *(short*)(&dest_pgt3_ptr->u2) = *(short*)(&pgt3_ptr->u2);
        */
        
        // Set CLUT
        dest_pgt3_ptr->clut = clutid[pgt3_ptr->clutid];

        // Set Texture Page
        dest_pgt3_ptr->tpage = tpageid;

        setPolyGT3(dest_pgt3_ptr);
        
        // Add primitive to Ordering Table and advance pointer
        // to point to the next primitive to be processed.
        addPrim(ot+otz+zoffset, dest_pgt3_ptr);
        dest_pgt3_ptr++;
      }
      
  }
  return (char*)dest_pgt3_ptr;


}

void ANM_LoadAnimation(ANM_ANIMATION ** anim, u_char * addr) {
  ANM_ANIMATION * temp = (ANM_ANIMATION*)addr;
  *anim = temp;
  
  temp->animation_list = (ANM_ENTRY*) ((char*)addr + ((long)temp->animation_list));
  temp->animation_data = (SVECTOR*) ((char*)addr + ((long)temp->animation_data));
}

/*void PrintMatrix(MATRIX * mat) {
  //printf("matrix:\n[%d,%d,%d]m\n",
        mat->m[0][0],mat->m[0][1],mat->m[0][2]
      );
      //printf("[%d,%d,%d]m\n",
        mat->m[1][0],mat->m[1][1],mat->m[1][2]
      );
      //printf("[%d,%d,%d]m\n",
        mat->m[2][0],mat->m[2][1],mat->m[2][2]
      );
      //printf("[%d,%d,%d]t\n",
        mat->t[0],mat->t[1],mat->t[2]
      );
}
*/

void ANM_InterpolateSVECTOR( SVECTOR * dest, SVECTOR * a, SVECTOR * b, short factor) {
  QUATERNION qa,qb;
  fix12_toQuaternion(&qa,a); // This Euler to Quaternion conversion is temporary
  fix12_toQuaternion(&qb,b); // since animation data (for now) is being exported in Euler
  fix12_quaternionSlerp((QUATERNION*)dest, &qa, &qb, factor);

  //FntPrint("QXYZW: %d %d %d %d\n",dest->vx,dest->vy,dest->vz,dest->pad);
}

void ANM_InterpolatePSVECTOR( SVECTOR * dest, SVECTOR * a, SVECTOR * b, short factor) {
  dest->vx = fix12_lerp(a->vx, b->vx, factor);
  dest->vy = fix12_lerp(a->vy, b->vy, factor);
  dest->vz = fix12_lerp(a->vz, b->vz, factor);
  
  //dest->vx = a->vx;
  //dest->vy = a->vy;
  //dest->vz = a->vz;
  
  // This one is faster but too low accuracy so animations look bad, this is used once per model
  // so why not go with something better?
  // fix12_gtesvlerp(dest, a, b, factor);
}

void ANM_InterpolateFrames(SVECTOR * frameF, SVECTOR * frameA, SVECTOR * frameB, short factor, int size) {
  int idx;
  ANM_InterpolatePSVECTOR(&frameF[0], &frameA[0], &frameB[0], factor);
  for(idx = 1; idx < size; idx++){
    SVECTOR * ptra = &frameA[idx]; // Frame A
    SVECTOR * ptrb = &frameB[idx]; // Frame B
    SVECTOR * ptrf = &frameF[idx]; // Frame Final (Result)
/*
    if(idx > 1){
      ptra->vx >>= 5;
      ptra->vy >>= 5;
      ptra->vz >>= 5;
      ptra->pad >>= 5;

      ptrb->vx >>= 5;
      ptrb->vy >>= 5;
      ptrb->vz >>= 5;
      ptrb->pad >>= 5;

      ptra->vx <<= 5;
      ptra->vy <<= 5;
      ptra->vz <<= 5;
      ptra->pad <<= 5;

      ptrb->vx <<= 5;
      ptrb->vy <<= 5;
      ptrb->vz <<= 5;
      ptrb->pad <<= 5;
    }*/
    //ANM_InterpolateSVECTOR(ptrf, ptra, ptrb, factor);
    fix12_quaternionSlerp((QUATERNION*)ptrf, (QUATERNION*)ptra, (QUATERNION*)ptrb, factor);
    //fix12_normalizeQuaternion(ptrf, ptrf);
    //FntPrint("QXYZW: %d %d %d %d\n",ptrf->vx,ptrf->vy,ptrf->vz,ptrf->pad);
  }
}

u_char * AGM_DrawModelTPage(AGM_model * model, u_char * packet_ptr, u_long * ot, short zoffset, u_short tpageid, u_short * clutid, u_short * mat) {
  AGM_FILE * file = model->file;
  AGM_POLYG4 * pg4_ptr = file->poly_g4;
  AGM_POLYG3 * pg3_ptr = file->poly_g3;
  AGM_POLYGT4 * pgt4_ptr = file->poly_gt4;
  AGM_POLYGT3 * pgt3_ptr = file->poly_gt3;
  POLY_G4 * dest_pg4_ptr = (POLY_G4*)packet_ptr;
  POLY_G3 * dest_pg3_ptr;
  POLY_GT4 * dest_pgt4_ptr;
  POLY_GT3 * dest_pgt3_ptr;
  long outer_product, otz;
  long tempz0,tempz1,tempz2,tempz3;
  short pg4_count = file->poly_g4_count;
  short pg3_count = file->poly_g3_count;
  short pgt4_count = file->poly_gt4_count;
  short pgt3_count = file->poly_gt3_count;
  short i;
  
  for(i = 0; i < pg4_count; i++, pg4_ptr++) {
      u_short mat_flags = mat[pg4_ptr->clutid];
      if(mat_flags & AGM_MATERIAL_NORENDER) continue;
      SVECTOR * vec0 = &AGM_TransformBuffer[pg4_ptr->idx0];
      SVECTOR * vec1 = &AGM_TransformBuffer[pg4_ptr->idx1];
      SVECTOR * vec2 = &AGM_TransformBuffer[pg4_ptr->idx2];
      SVECTOR * vec3 = &AGM_TransformBuffer[pg4_ptr->idx3];

      // Load screen XY coordinates to GTE Registers
      gte_ldsxy3(*(long*)&vec0->vx,*(long*)&vec1->vx,*(long*)&vec2->vx);
      // Perform clipping calculation
      gte_nclip();
      // Store Outer Product
      gte_stopz(&outer_product);
      // Check side
      if(outer_product <= 0 && !(mat_flags & AGM_MATERIAL_NOCULLING)) continue; // Skip back facing polys
      ////printf("rendering face %d\n",i);
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
      //otz = 50;
      if (otz > OTMINCHAR && otz < OTSIZE) {
        long temp0, temp1, temp2, temp3;
        // If all tests have passed, now process the rest of the primitive.
        // Copy values already in the registers
        gte_stsxy3((long*)&dest_pg4_ptr->x0,
                   (long*)&dest_pg4_ptr->x1,
                   (long*)&dest_pg4_ptr->x2);
        // Store 4th vertex from buffer directly

        temp0 = *(long*)(&vec3->vx);
        *(long*)(&dest_pg4_ptr->x3) = temp0;

        if(mat_flags & AGM_MATERIAL_NOLIGHTING) {
          *(CVECTOR*)&dest_pg4_ptr->r0 = *(CVECTOR*)&pg4_ptr->r0;
          *(CVECTOR*)&dest_pg4_ptr->r1 = *(CVECTOR*)&pg4_ptr->r1;
          *(CVECTOR*)&dest_pg4_ptr->r2 = *(CVECTOR*)&pg4_ptr->r2;
          *(CVECTOR*)&dest_pg4_ptr->r3 = *(CVECTOR*)&pg4_ptr->r3;
        } else {
          /*gte_ldrgb((CVECTOR*)&pg4_ptr->r0);
          gte_cc();
          gte_strgb((CVECTOR*)&dest_pg4_ptr->r0);
          gte_ldrgb((CVECTOR*)&pg4_ptr->r1);
          gte_cc();
          gte_strgb((CVECTOR*)&dest_pg4_ptr->r1);
          gte_ldrgb((CVECTOR*)&pg4_ptr->r2);
          gte_cc();
          gte_strgb((CVECTOR*)&dest_pg4_ptr->r2);
          gte_ldrgb((CVECTOR*)&pg4_ptr->r3);
          gte_cc();
          gte_strgb((CVECTOR*)&dest_pg4_ptr->r3);*/

          gte_ldv3(
            &model->file->normal_data[pg4_ptr->idx0],
            &model->file->normal_data[pg4_ptr->idx1],
            &model->file->normal_data[pg4_ptr->idx2]
            );
          gte_ldrgb((CVECTOR*)&pg4_ptr->r0);
          gte_ncct();
          gte_strgb3(&dest_pg4_ptr->r0, &dest_pg4_ptr->r1, &dest_pg4_ptr->r2);

          gte_ldv0(&model->file->normal_data[pg4_ptr->idx3]);
          gte_nccs();
          gte_strgb(&dest_pg4_ptr->r3);
        }
/*
        gte_ldrgb((CVECTOR*)&pg4_ptr->r0);
        gte_cc();
        gte_strgb((CVECTOR*)&dest_pg4_ptr->r0);
        gte_ldrgb((CVECTOR*)&pg4_ptr->r1);
        gte_cc();
        gte_strgb((CVECTOR*)&dest_pg4_ptr->r1);
        gte_ldrgb((CVECTOR*)&pg4_ptr->r2);
        gte_cc();
        gte_strgb((CVECTOR*)&dest_pg4_ptr->r2);
        gte_ldrgb((CVECTOR*)&pg4_ptr->r3);
        gte_cc();
        gte_strgb((CVECTOR*)&dest_pg4_ptr->r3);
*/
        setPolyG4(dest_pg4_ptr);
        
        // Add primitive to Ordering Table and advance pointer
        // to point to the next primitive to be processed.
        addPrim(ot+otz+zoffset, dest_pg4_ptr);
        dest_pg4_ptr++;
      }
      
  }
  dest_pg3_ptr = (POLY_G3*)dest_pg4_ptr;
  

  for(i = 0; i < pg3_count; i++, pg3_ptr++) {
      u_short mat_flags = mat[pg3_ptr->clutid];
      if(mat_flags & AGM_MATERIAL_NORENDER) continue;
      SVECTOR * vec0 = &AGM_TransformBuffer[pg3_ptr->idx0];
      SVECTOR * vec1 = &AGM_TransformBuffer[pg3_ptr->idx1];
      SVECTOR * vec2 = &AGM_TransformBuffer[pg3_ptr->idx2];
      
      // Load screen XY coordinates to GTE Registers
      gte_ldsxy3(*(long*)&vec0->vx,*(long*)&vec1->vx,*(long*)&vec2->vx);
      // Perform clipping calculation
      gte_nclip();
      // Store Outer Product
      gte_stopz(&outer_product);
      // Check side
      if(outer_product <= 0 && !(mat_flags & AGM_MATERIAL_NOCULLING)) continue; // Skip back facing polys
      ////printf("rendering face %d\n",i);
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
      if (otz > OTMINCHAR && otz < OTSIZE) {
        // If all tests have passed, now process the rest of the primitive.
        // Copy values already in the registers
        gte_stsxy3((long*)&dest_pg3_ptr->x0,
                   (long*)&dest_pg3_ptr->x1,
                   (long*)&dest_pg3_ptr->x2);
        // Vertex Colors
        if(mat_flags & AGM_MATERIAL_NOLIGHTING) {
          *(CVECTOR*)&dest_pg3_ptr->r0 = *(CVECTOR*)&pg3_ptr->r0;
          *(CVECTOR*)&dest_pg3_ptr->r1 = *(CVECTOR*)&pg3_ptr->r1;
          *(CVECTOR*)&dest_pg3_ptr->r2 = *(CVECTOR*)&pg3_ptr->r2;
        } else {
          /*gte_ldrgb((CVECTOR*)&pg3_ptr->r0);
          gte_cc();
          gte_strgb((CVECTOR*)&dest_pg3_ptr->r0);
          gte_ldrgb((CVECTOR*)&pg3_ptr->r1);
          gte_cc();
          gte_strgb((CVECTOR*)&dest_pg3_ptr->r1);
          gte_ldrgb((CVECTOR*)&pg3_ptr->r2);
          gte_cc();
          gte_strgb((CVECTOR*)&dest_pg3_ptr->r2);*/

          gte_ldv3(
            &model->file->normal_data[pg3_ptr->idx0],
            &model->file->normal_data[pg3_ptr->idx1],
            &model->file->normal_data[pg3_ptr->idx2]
            );
          gte_ldrgb((CVECTOR*)&pg3_ptr->r0);
          gte_ncct();
          gte_strgb3(&dest_pg3_ptr->r0, &dest_pg3_ptr->r1, &dest_pg3_ptr->r2);
        }
        /*
        gte_ldrgb((CVECTOR*)&pg3_ptr->r0);
        gte_cc();
        gte_strgb((CVECTOR*)&dest_pg3_ptr->r0);
        gte_ldrgb((CVECTOR*)&pg3_ptr->r1);
        gte_cc();
        gte_strgb((CVECTOR*)&dest_pg3_ptr->r1);
        gte_ldrgb((CVECTOR*)&pg3_ptr->r2);
        gte_cc();
        gte_strgb((CVECTOR*)&dest_pg3_ptr->r2);
*/
        setPolyG3(dest_pg3_ptr);
        
        // Add primitive to Ordering Table and advance pointer
        // to point to the next primitive to be processed.
        addPrim(ot+otz+zoffset, dest_pg3_ptr);
        dest_pg3_ptr++;
      }
      
  }

  dest_pgt4_ptr = (POLY_GT4*)dest_pg3_ptr;

  for(i = 0; i < pgt4_count; i++, pgt4_ptr++) {
      u_short clutval = clutid[pgt4_ptr->clutid];
      u_short mat_flags = mat[pgt4_ptr->clutid];
      if(mat_flags & AGM_MATERIAL_NORENDER) continue;
      SVECTOR * vec0 = &AGM_TransformBuffer[pgt4_ptr->idx0];
      SVECTOR * vec1 = &AGM_TransformBuffer[pgt4_ptr->idx1];
      SVECTOR * vec2 = &AGM_TransformBuffer[pgt4_ptr->idx2];
      SVECTOR * vec3 = &AGM_TransformBuffer[pgt4_ptr->idx3];
            
      // Load screen XY coordinates to GTE Registers
      gte_ldsxy3(*(long*)&vec0->vx,*(long*)&vec1->vx,*(long*)&vec2->vx);
      // Perform clipping calculation
      gte_nclip();
      // Store Outer Product
      gte_stopz(&outer_product);
      // Check side
      if(outer_product <= 0 && !(mat_flags & AGM_MATERIAL_NOCULLING)) continue; // Skip back facing polys
      ////printf("rendering face %d\n",i);
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
      //otz = 50;
      if (otz > OTMINCHAR && otz < OTSIZE) {
        long temp0, temp1, temp2, temp3;
        // If all tests have passed, now process the rest of the primitive.
        // Copy values already in the registers
        gte_stsxy3((long*)&dest_pgt4_ptr->x0,
                   (long*)&dest_pgt4_ptr->x1,
                   (long*)&dest_pgt4_ptr->x2);
        // Store 4th vertex from buffer directly
        temp0 = *(long*)(&vec3->vx);
        *(long*)(&dest_pgt4_ptr->x3) = temp0;
        // Vertex Colors
        if(mat_flags & AGM_MATERIAL_NOLIGHTING) {
          *(CVECTOR*)&dest_pgt4_ptr->r0 = *(CVECTOR*)&pgt4_ptr->r0;
          *(CVECTOR*)&dest_pgt4_ptr->r1 = *(CVECTOR*)&pgt4_ptr->r1;
          *(CVECTOR*)&dest_pgt4_ptr->r2 = *(CVECTOR*)&pgt4_ptr->r2;
          *(CVECTOR*)&dest_pgt4_ptr->r3 = *(CVECTOR*)&pgt4_ptr->r3;
        } else {
          /*gte_ldrgb((CVECTOR*)&pgt4_ptr->r0);
          gte_cc();
          gte_strgb((CVECTOR*)&dest_pgt4_ptr->r0);
          gte_ldrgb((CVECTOR*)&pgt4_ptr->r1);
          gte_cc();
          gte_strgb((CVECTOR*)&dest_pgt4_ptr->r1);
          gte_ldrgb((CVECTOR*)&pgt4_ptr->r2);
          gte_cc();
          gte_strgb((CVECTOR*)&dest_pgt4_ptr->r2);*/
        
          gte_ldv3(
            &model->file->normal_data[pgt4_ptr->idx0],
            &model->file->normal_data[pgt4_ptr->idx1],
            &model->file->normal_data[pgt4_ptr->idx2]
            );
          gte_nct();
          gte_strgb3(&dest_pgt4_ptr->r0, &dest_pgt4_ptr->r1, &dest_pgt4_ptr->r2);

          gte_ldv0(&model->file->normal_data[pgt4_ptr->idx3]);
          gte_ncs();
          gte_strgb(&dest_pgt4_ptr->r3);

          //gte_ldrgb((CVECTOR*)&pgt4_ptr->r3);
          //gte_cc();
          //gte_strgb((CVECTOR*)&dest_pgt4_ptr->r3);
          //*(long*)&dest_pgt4_ptr->r3 = 0x00000000;
        }

        // Texture Coordinates
        // Set Texture Coordinates
        temp1 = *(short*)(&pgt4_ptr->u0);
        temp2 = *(short*)(&pgt4_ptr->u1);
        temp3 = *(short*)(&pgt4_ptr->u2);
        temp0 = *(short*)(&pgt4_ptr->u3);
        *(short*)(&dest_pgt4_ptr->u0) = temp1;
        *(short*)(&dest_pgt4_ptr->u1) = temp2;
        *(short*)(&dest_pgt4_ptr->u2) = temp3;
        *(short*)(&dest_pgt4_ptr->u3) = temp0;

        // Set CLUT
        //dest_pgt4_ptr->clut = clutid[pgt4_ptr->clutid];
        dest_pgt4_ptr->clut = clutval;
        // Set Texture Page
        dest_pgt4_ptr->tpage = tpageid;

        setPolyGT4(dest_pgt4_ptr);
        
        // Add primitive to Ordering Table and advance pointer
        // to point to the next primitive to be processed.
        addPrim(ot+otz+zoffset, dest_pgt4_ptr);
        dest_pgt4_ptr++;
      }
      
  }
  dest_pgt3_ptr = (POLY_GT3*)dest_pgt4_ptr;
  
  for(i = 0; i < pgt3_count; i++, pgt3_ptr++) {
      u_short clutval = clutid[pgt3_ptr->clutid];
      u_short mat_flags = mat[pgt3_ptr->clutid];
      if(mat_flags & AGM_MATERIAL_NORENDER) continue;
      SVECTOR * vec0 = &AGM_TransformBuffer[pgt3_ptr->idx0];
      SVECTOR * vec1 = &AGM_TransformBuffer[pgt3_ptr->idx1];
      SVECTOR * vec2 = &AGM_TransformBuffer[pgt3_ptr->idx2];
      
      // Load screen XY coordinates to GTE Registers
      gte_ldsxy3(*(long*)&vec0->vx,*(long*)&vec1->vx,*(long*)&vec2->vx);
      // Perform clipping calculation
      gte_nclip();
      // Store Outer Product
      gte_stopz(&outer_product);
      // Check side
      if(outer_product <= 0  && !(mat_flags & AGM_MATERIAL_NOCULLING)) continue; // Skip back facing polys
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
      if (otz > OTMINCHAR && otz < OTSIZE) {
        long temp0, temp1, temp2;
        // If all tests have passed, now process the rest of the primitive.
        // Copy values already in the registers
        gte_stsxy3((long*)&dest_pgt3_ptr->x0,
                   (long*)&dest_pgt3_ptr->x1,
                   (long*)&dest_pgt3_ptr->x2);
        // Vertex Colors
        if(mat_flags & AGM_MATERIAL_NOLIGHTING) {
          *(CVECTOR*)&dest_pgt3_ptr->r0 = *(CVECTOR*)&pgt3_ptr->r0;
          *(CVECTOR*)&dest_pgt3_ptr->r1 = *(CVECTOR*)&pgt3_ptr->r1;
          *(CVECTOR*)&dest_pgt3_ptr->r2 = *(CVECTOR*)&pgt3_ptr->r1;
        } else {
          /*gte_ldrgb((CVECTOR*)&pgt3_ptr->r0);
          gte_cc();
          gte_strgb((CVECTOR*)&dest_pgt3_ptr->r0);
          gte_ldrgb((CVECTOR*)&pgt3_ptr->r1);
          gte_cc();
          gte_strgb((CVECTOR*)&dest_pgt3_ptr->r1);
          gte_ldrgb((CVECTOR*)&pgt3_ptr->r2);
          gte_cc();
          gte_strgb((CVECTOR*)&dest_pgt3_ptr->r2);*/
          gte_ldv3(
            &model->file->normal_data[pgt3_ptr->idx0],
            &model->file->normal_data[pgt3_ptr->idx1],
            &model->file->normal_data[pgt3_ptr->idx2]
            );
          gte_nct();
          gte_strgb3(&dest_pgt3_ptr->r0, &dest_pgt3_ptr->r1, &dest_pgt3_ptr->r2);
        }
        /*
        gte_ldrgb((CVECTOR*)&pgt3_ptr->r0);
        gte_cc();
        gte_strgb((CVECTOR*)&dest_pgt3_ptr->r0);
        gte_ldrgb((CVECTOR*)&pgt3_ptr->r1);
        gte_cc();
        gte_strgb((CVECTOR*)&dest_pgt3_ptr->r1);
        gte_ldrgb((CVECTOR*)&pgt3_ptr->r2);
        gte_cc();
        gte_strgb((CVECTOR*)&dest_pgt3_ptr->r2);
        */

        // Set Texture Coordinates
        temp0 = *(short*)(&pgt3_ptr->u0);
        temp1 = *(short*)(&pgt3_ptr->u1);
        temp2 = *(short*)(&pgt3_ptr->u2);
        *(short*)(&dest_pgt3_ptr->u0) = temp0;
        *(short*)(&dest_pgt3_ptr->u1) = temp1;
        *(short*)(&dest_pgt3_ptr->u2) = temp2;        
        // Set CLUT
        dest_pgt3_ptr->clut = clutval;

        // Set Texture Page
        dest_pgt3_ptr->tpage = tpageid;

        setPolyGT3(dest_pgt3_ptr);
        
        // Add primitive to Ordering Table and advance pointer
        // to point to the next primitive to be processed.
        addPrim(ot+otz+zoffset, dest_pgt3_ptr);
        dest_pgt3_ptr++;
      }
      
  }
  return (char*)dest_pgt3_ptr;


}

void AGM_OffsetTexByMaterial(AGM_model * model, u_char material, u_char u, u_char v) {
  for(int i = 0; i < model->file->poly_gt4_count; i++) {
    AGM_POLYGT4 * poly = &model->file->poly_gt4[i];
    if(poly->clutid != material) continue;
    poly->u0 = (poly->u0 + u) % 256;
    poly->v0 = (poly->v0 + v) % 256;
    poly->u1 = (poly->u1 + u) % 256;
    poly->v1 = (poly->v1 + v) % 256;
    poly->u2 = (poly->u2 + u) % 256;
    poly->v2 = (poly->v2 + v) % 256;
    poly->u3 = (poly->u3 + u) % 256;
    poly->v3 = (poly->v3 + v) % 256;
  }

  for(int i = 0; i < model->file->poly_gt3_count; i++) {
    AGM_POLYGT3 * poly = &model->file->poly_gt3[i];
    if(poly->clutid != material) continue;
    poly->u0 = (poly->u0 + u) % 256;
    poly->v0 = (poly->v0 + v) % 256;
    poly->u1 = (poly->u1 + u) % 256;
    poly->v1 = (poly->v1 + v) % 256;
    poly->u2 = (poly->u2 + u) % 256;
    poly->v2 = (poly->v2 + v) % 256;
  }
}

// Transform by Bone Vertex and Normals
