/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#ifndef COLLISION_H
#define COLLISION_H

#include "global.h"

/* Collision Grid Cell Size, power of two values only */
#define BACKGROUND_CELL_SIZE 4096
//#define BACKGROUND_CELL_SIZE 8192
/* Ammount of bits to shift to get the same value as BACKGROUND_CELL_SIZE */
#define BACKGROUND_CELL_SHIFT 11
//#define BACKGROUND_CELL_SHIFT 13

struct Actor;

typedef struct {
  short vx, vy, vz, pad;
} Background_Vertex;

typedef struct {
  short v0, v1, v2, pad; // Vertex ID
  /* PAD: to be used as triangle metadata */
} Background_Triangle;

typedef struct {
  unsigned short tri_id; // Triangle content, if 0xFFFF, then it's the end of this linked list
  unsigned short next; // Next Background_Cell index
} Background_Cell;

// Fixed 4096 unit sized 3D grid
typedef struct {
  /* Collision bounding box max-min */
  short box_min_x;
  short box_min_y;
  short box_min_z;
  short box_max_x;
  short box_max_y;
  short box_max_z;

  /* Total length of the collision bounding box on all axis (power of two) */
  unsigned short box_len_x;
  unsigned short box_len_y;
  unsigned short box_len_z;

  /* Total number of vertices in collision mesh */
  unsigned short vtx_num;

  /* Pointer to vertex data array */
  Background_Vertex * vtx_data;

  /* Total number of triangles in collision mesh */
  unsigned short tri_num;

  /* Pointer to triangle data array */
  Background_Triangle * tri_data;

  /*
   * Pointer to array containing all cells in the scene
   * Size: cell_num_x * cell_num_y * cell_num_z = total_cell_num
   * Content: if = 0xFFFF, empty cell, anything else: index of cell list
   */
  unsigned short * cell_array;

  unsigned char cell_num_x;
  unsigned char cell_num_y;
  unsigned char cell_num_z;

  /* Total number of cells entries */
  unsigned short total_cell_num;

  /* Cell Linked List */
  Background_Cell * cell_list;
  unsigned short cell_list_num;

} Background_CollisionInfo;


// 2D Collision Grid
struct CollisionGrid {
  u_char cell_size; // shifted by 8
  u_char cell_shift; // ammount of bits for shift division
  short cell_x; // Total cells in X axis
  short cell_y;
  short cell_z; // Total cells in Z axis
  // Collision Bounding box
  struct BoundingBox3D bbox;
};

typedef struct COL_HEADER {
  long ID;
  long version;
  long quad_count;
  long tri_count;
  short minx,miny,minz;
  short maxx,maxy,maxz;
  long padding;
} COL_HEADER;

typedef struct COL_FACE {
  short x0, y0, z0;
  short x1, y1, z1;
  short x2, y2, z2;
  short flags;
} COL_FACE;

typedef struct COL_PLANE {
  short nx0, ny0, nz0;
  short distance;
} COL_PLANE;

typedef struct COL {
  unsigned long * file;
  COL_HEADER * header;
  COL_FACE * faces;
  COL_PLANE * planes;
} COL;

/* COL2 Structs */
typedef struct ColVert {
  short vx, vy, vz;
  short pad; /* might be removed in the future? */
} Col_Vert;

typedef struct ColFace {
  unsigned short vert0, vert1, vert2; /* index to face vertices */
  unsigned short plane; /* index to face plane */
  unsigned int flags;
} Col_Face;

typedef struct ColPlane {
  short nx0, ny0, nz0, pad;
  long distance;
} Col_Plane;

typedef struct ColBucket {
  unsigned short floor; // index to linked list entry of floor polygons
  unsigned short wall; // index to linked list entry of wall polygons
  unsigned short roof; // index to linked list entry of roof polygons
  unsigned short pad;
} Col_Bucket;

typedef struct ColPolyList {
  unsigned short face; // face index
  unsigned short next; // If == 0xFFFF: end of linked list
} Col_PolyList;

typedef struct COL2 {
  unsigned long ID;
  unsigned short vertex_num;
  unsigned short face_num;
  unsigned short plane_num;
  short minx,miny,minz;
  short maxx,maxy,maxz;
  unsigned short box_x, box_y, box_z;
  unsigned long padding0;
  Col_Vert * vertices;
  Col_Face * faces;
  Col_Plane * planes;
  unsigned short * grid_ptr;
  Col_Bucket * cell_entry;
  Col_PolyList * poly_entry_list;
  unsigned short grid_num;
  unsigned short cell_num;
  unsigned short poly_entry_num;
  unsigned char grid_x, grid_y, grid_z, padding1;
} Col2;

typedef struct COLLISION_INFO {
  Col2 * file;
  /* Collision Grid Cell Count: X,Y,Z */
  unsigned char grid_x, grid_y, grid_z;
  /* total number of grid cells */
  unsigned short grid_num; 
  /* Grid Cell Pointer
   * size = grid_num << 1 (grid_num * 2)
   * 
   */
  unsigned short * grid_ptr;

} Col_Info;

typedef struct COLLISION_CELLPOS {
  unsigned short x, y, z;
  unsigned short flag; // Out of bounds flag
} Col_CellPos;

Col2 * Col_LoadFile(unsigned long * addr);

Col_Info * Col_Init(Col2 * file);
Col_Info * Col_Init2(Col2 * file);

void Col_Destruct(Col_Info * col);

void Col_GetCell(Col2 * info, Col_CellPos * result, SVECTOR * pos);

void Col_GetCellPos(Col2 * info, Col_CellPos * result, long x, long y, long z);


unsigned short Col_GetCellElem(Col2 * col, Col_CellPos * pos);

typedef struct ColBox {
  short min_x, min_y, min_z;
  short max_x, max_y, max_z;
} Col_Box;

void Col_BoxTriangleCheck(Col_Face * face);

int Col_PolyInCell(Col2 * col, unsigned short cell, unsigned short face);

unsigned short Col_TriBoundingCheck(SVECTOR * pos, SVECTOR * v0, SVECTOR * v1, SVECTOR * v2, Col_Box * box);
unsigned short Col_TriBoundingCheckPrecalc(SVECTOR * pos, Col_Box * box);


typedef struct ColResult {
  short face;
  Col_Plane * plane;
  SVECTOR normal;
  SVECTOR position;
} Col_Result;

short Col_GroundCheckPoint(Col2 * col, SVECTOR * pos, unsigned short dist, Col_Result * result);


short Col_GroundCheckPointScratch(Col2 * col, SVECTOR * pos);

// Check which cells object can be in based on it's bounding box
//void Collision_ActorCellCheck(struct Actor * actor, struct CollisionGrid * col, struct BoundingBox2D * result);

// Gets Y-Axis Ray Current Cell,
//void Collision_YRayCellCheck(SVECTOR * pos, struct CollisionGrid * col, CollisionCellResult * result);

// Check cells this rays collides with
//void Collision_RayCellCheck(SVECTOR * pos, SVECTOR * normal, struct CollisionGrid * col, struct CollisionRayResult * result);

void Col_GenerateBoundingBoxXYZ(Col_Box * result, SVECTOR * v0, SVECTOR * v1, SVECTOR * v2);

int PointInTriangleScratch (SVECTOR * pt, SVECTOR * v1, SVECTOR * v2, SVECTOR * v3);

unsigned long fastabs(long value);

#endif