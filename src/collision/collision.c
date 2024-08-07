/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#include "collision/collision.h"

// Check which cells object can be in based on it's bounding box
//void Collision_ActorCellCheck(struct Actor * actor, struct CollisionGrid * col, struct BoundingBox2D * result) {
  //u_char colgrid_size = col->cell_shift;
  // Map Collision Bounding Box
  //short actorx =  actor->x - col->bbox->min_x;
  //short actorz = actor->z - col->bbox->min_z;
  //struct BoundingBox3D actorbb = actor->bbox;
  // Shift-Divide bounding box position offset by actor current position
  
  //result->min_x = (actorbb.min_x + actorx) >> colgrid_size;
  //result->min_z = (actorbb.min_z + actorz) >> colgrid_size;
  //result->max_x = (actorbb.max_x + actorx) >> colgrid_size;
  //result->max_z = (actorbb.max_z + actorz) >> colgrid_size;
//}

// Gets Y-Axis Ray Current Cell
//void Collision_YRayCellCheck(SVECTOR * ray, struct CollisionGrid * col, CollisionCellResult * result) {
  //u_char colgrid_size = col->cell_shift;
  //result->x = (ray->vx - col->bbox->min_x) >> colgrid_size;
  //result->z = (ray->vz - col->bbox->min_z) >> colgrid_size;
//}

// Check cells this rays collides with
//void Collision_RayCellCheck(SVECTOR * pos, SVECTOR * normal, struct CollisionGrid * col, struct CollisionRayResult * result) {
  //u_char colgrid_size = col->cell_shift;

//}

Col_Box * bbox_list;

Col2 * Col_LoadFile(unsigned long * addr) {
  Col2 * colptr = (Col2*)addr;
  long addr_int = (long)addr;
  int i;

  colptr->vertices = (Col_Vert*)(((long)colptr->vertices)+(addr_int));
  colptr->faces = (Col_Face*)(((long)colptr->faces)+(addr_int));
  colptr->planes = (Col_Plane*)(((long)colptr->planes)+(addr_int));
  colptr->grid_ptr = (unsigned short*)(((long)colptr->grid_ptr)+(addr_int));
  colptr->cell_entry = (Col_Bucket*)(((long)colptr->cell_entry)+(addr_int));
  colptr->poly_entry_list = (Col_PolyList*)(((long)colptr->poly_entry_list)+(addr_int));

  bbox_list = (Col_Box*)LStack_Alloc(sizeof(Col_Box) * colptr->face_num);
  for(i = 0; i < colptr->face_num; i++){
    Col_Box * box = &bbox_list[i];
    Col_Face * face = &colptr->faces[i];
    SVECTOR * v0 = (SVECTOR*)&colptr->vertices[face->vert0];
    SVECTOR * v1 = (SVECTOR*)&colptr->vertices[face->vert1];
    SVECTOR * v2 = (SVECTOR*)&colptr->vertices[face->vert2];
    Col_GenerateBoundingBoxXYZ(box, v0, v1, v2);
  }

  return colptr;
}

Col_Info * Col_Init(Col2 * file) {
  long i;
  long max_cell_x, max_cell_y, max_cell_z = 0;
  unsigned long unnacounted = 0;
  Col_Info * col = (Col_Info*)LStack_Alloc(sizeof(Col_Info));

  col->file = file;

  col->grid_x = (file->box_x >> BACKGROUND_CELL_SHIFT) + 1;
  col->grid_y = (file->box_y >> BACKGROUND_CELL_SHIFT) + 1;
  col->grid_z = (file->box_z >> BACKGROUND_CELL_SHIFT) + 1;

  col->grid_num = col->grid_x * col->grid_z * col->grid_y;

  col->grid_ptr = (unsigned short*)LStack_Alloc(col->grid_num * sizeof(unsigned short));

  //printf("bbox min: x=%d y=%d z=%d\n", file->minx, file->miny, file->minz);

  /* Set grid cells to 0x8000 (no element) */
  for(i = 0; i < col->grid_num; i++) {
    col->grid_ptr[i] = 0xFFFF;
  }

  /* Check if grid cell contain any faces */
  for(i =0; i < file->face_num; i++) {
    
    Col_Vert * vert0, * vert1, * vert2;
    Col_Face * face = &file->faces[i];
    long cvert0_x, cvert0_y, cvert0_z;
    long cvert1_x, cvert1_y, cvert1_z;
    long cvert2_x, cvert2_y, cvert2_z;
    unsigned short cell_pos_v0, cell_pos_v1, cell_pos_v2;

    /* Vertices */
    vert0 = &file->vertices[face->vert0];
    vert1 = &file->vertices[face->vert1];
    vert2 = &file->vertices[face->vert2];

    /* Convert vertex position into cell */
    /* Vertex 1 */
    cvert0_x = ((long)vert0->vx - (long)file->minx) >> BACKGROUND_CELL_SHIFT;
    cvert0_y = ((long)vert0->vy - (long)file->miny) >> BACKGROUND_CELL_SHIFT;
    cvert0_z = ((long)vert0->vz - (long)file->minz) >> BACKGROUND_CELL_SHIFT;
    /* Vertex 2 */
    cvert1_x = ((long)vert1->vx - (long)file->minx) >> BACKGROUND_CELL_SHIFT;
    cvert1_y = ((long)vert1->vy - (long)file->miny) >> BACKGROUND_CELL_SHIFT;
    cvert1_z = ((long)vert1->vz - (long)file->minz) >> BACKGROUND_CELL_SHIFT;
    /* Vertex 3 */
    cvert2_x = ((long)vert2->vx - (long)file->minx) >> BACKGROUND_CELL_SHIFT;
    cvert2_y = ((long)vert2->vy - (long)file->miny) >> BACKGROUND_CELL_SHIFT;
    cvert2_z = ((long)vert2->vz - (long)file->minz) >> BACKGROUND_CELL_SHIFT;

    /* debug you */
    if(cvert0_x > max_cell_x) max_cell_x = cvert0_x;
    if(cvert1_x > max_cell_x) max_cell_x = cvert1_x;
    if(cvert2_x > max_cell_x) max_cell_x = cvert2_x;

    if(cvert0_y > max_cell_y) max_cell_y = cvert0_y;
    if(cvert1_y > max_cell_y) max_cell_y = cvert1_y;
    if(cvert2_y > max_cell_y) max_cell_y = cvert2_y;

    if(cvert0_z > max_cell_z) max_cell_z = cvert0_z;
    if(cvert1_z > max_cell_z) max_cell_z = cvert1_z;
    if(cvert2_z > max_cell_z) max_cell_z = cvert2_z;

    if(cvert0_y >= col->grid_y) {
      //printf("cell y error: %d vertex 0 (index: %d)\n",cvert0_y,i);
    }
    if(cvert1_y >= col->grid_y) {
      //printf("cell y error: %d vertex 1 (index: %d)\n",cvert1_y,i);
    }
    if(cvert2_y >= col->grid_y) {
      //printf("cell y error: %d vertex 2 (index: %d)\n",cvert2_y,i);
    }

    /* If all faces are in the same cell, skip additional checks */

    cell_pos_v0 = cvert0_x + col->grid_x * (cvert0_z + col->grid_z * cvert0_y);
    cell_pos_v1 = cvert1_x + col->grid_x * (cvert1_z + col->grid_z * cvert1_y);
    cell_pos_v2 = cvert2_x + col->grid_x * (cvert2_z + col->grid_z * cvert2_y);

    //printf("face %d, verts [%d] [%d] [%d]\n",i,cell_pos_v0,cell_pos_v1,cell_pos_v2);


    if(cell_pos_v0 == cell_pos_v1 && cell_pos_v0 == cell_pos_v2) {
    //if((cvert0_x && cvert1_x && cvert2_x) && (cvert0_y && cvert1_y && cvert2_y) && (cvert0_z && cvert1_z && cvert2_z)) {
      /* all vertices lie in the same cell */
      /* x + xn * ( y + yn * z ) */
      //unsigned short cell_pos = cvert0_x + col->grid_x * (cvert0_z + col->grid_z * cvert0_y);
      unsigned short cell = col->grid_ptr[cell_pos_v0];

      ////printf("face %d - cell[%d][%d][%d]\n", i, cvert0_x, cvert0_y, cvert0_z);
      if(cell == 0xFFFF) {
        /* cell is empty */
        cell = 1;
        col->grid_ptr[cell_pos_v0] = cell; /* set first element of cell to 0 */
      } else {
        cell = cell + 1;
        col->grid_ptr[cell_pos_v0] = cell;
      }
    } else {
      unnacounted++;
    }
  }

  /* TEMP print out cell counts */
  {
    long z, y, x;
    for(y = 0; y < col->grid_y; y++) {
      //printf("y = %d\n",y);
      for(z = 0; z < col->grid_z; z++) {
        for(x = 0; x < col->grid_x; x++) {
          /* x + xn * ( y + yn * z ) */
          unsigned short cell_pos = x + col->grid_x * (z + col->grid_z * y);
          unsigned short cell = col->grid_ptr[cell_pos];
          if(cell == 0xFFFF){
            //printf("[XXX] ");
          } else {
            //printf("[%03d] ",cell);
          }
        }
        //printf("\n");
      }
      //printf("\n");
    }
  }

  //printf("%d triangles unaccounted for. (belongs to multiple cells)\n", unnacounted);
  //printf("max cell: x=%d y=%d z=%d\n",max_cell_x,max_cell_y,max_cell_z);

  return col;
}

Col_Info * Col_Init2(Col2 * file) {
  long i;
  long max_cell_x, max_cell_y, max_cell_z = 0;
  unsigned long unnacounted = 0;
  Col_Info * col = (Col_Info*)(sizeof(Col_Info));

  col->file = file;

  col->grid_x = (file->box_x >> BACKGROUND_CELL_SHIFT) + 1;
  col->grid_y = (file->box_y >> BACKGROUND_CELL_SHIFT) + 1;
  col->grid_z = (file->box_z >> BACKGROUND_CELL_SHIFT) + 1;

  col->grid_num = col->grid_x * col->grid_z * col->grid_y;

  col->grid_ptr = (unsigned short*)LStack_Alloc(col->grid_num * sizeof(unsigned short));

  //printf("bbox min: x=%d y=%d z=%d\n", file->minx, file->miny, file->minz);

  /* Set grid cells to 0x8000 (no element) */
  for(i = 0; i < col->grid_num; i++) {
    col->grid_ptr[i] = 0xFFFF;
  }
  //printf("entering massive check!");

  /* Check if grid cell contain any faces */
  {
    long z, y, x, i;
    long collide_xyz;
    RECT32 active_cell;
    VECTOR2D temp0, temp1, temp2;
    Col_Face * face;
    SVECTOR * vert0, * vert1, * vert2;
    active_cell.w = 2048;
    active_cell.h = 2048;
    
    for(y = 0; y < col->grid_y; y++) {
      //printf("y level = %d\n",y);
      for(z = 0; z < col->grid_z; z++) {
        ////printf("z level = %d\n",z);
        for(x = 0; x < col->grid_x; x++) {
          unsigned short cell_pos = x + col->grid_x * (z + col->grid_z * y);
          unsigned short cell = col->grid_ptr[cell_pos];
         ////printf("x level = %d\n",x);
          /* BIG CHECK AHEAD! Every Cell for Every Triangle! */
          active_cell.x = ((long)x << 11) + ((long)file->minx);
          for(i = 0; i < col->file->face_num; i++) {
            // Check XZ
            face = &col->file->faces[i];
            vert0 = (SVECTOR*)&file->vertices[face->vert0];
            vert1 = (SVECTOR*)&file->vertices[face->vert1];
            vert2 = (SVECTOR*)&file->vertices[face->vert2];

            active_cell.y = ((long)z << 11) + ((long)file->minz);

            temp0.vx = vert0->vx;
            temp0.vy = vert0->vz;
            temp1.vx = vert1->vx;
            temp1.vy = vert1->vz;
            temp2.vx = vert2->vx;
            temp2.vy = vert2->vz;
            collide_xyz = 0;
            if(!tri_clip_s(&active_cell,
                        (VECTOR2D*)&temp0,
                        (VECTOR2D*)&temp1,
                        (VECTOR2D*)&temp2)) {
              collide_xyz |= 1;
            }
            active_cell.y = ((long)y << 11) + ((long)file->miny);
            temp0.vy = vert0->vy;
            temp1.vy = vert1->vy;
            temp2.vy = vert2->vy;
            if(!tri_clip_s(&active_cell,
                        (VECTOR2D*)&temp0,
                        (VECTOR2D*)&temp1,
                        (VECTOR2D*)&temp2)) {
              collide_xyz |= 2;
            }
            if(collide_xyz == 3) {
              if(cell == 0xFFFF) {
                cell = 0;
              } else {
                cell++;
              }
            }

          }
          col->grid_ptr[cell_pos] = cell;
        }
      }
    }
  }

  /* TEMP print out cell counts */
  {
    long z, y, x;
    for(y = 0; y < col->grid_y; y++) {
      //printf("y = %d\n",y);
      for(z = 0; z < col->grid_z; z++) {
        for(x = 0; x < col->grid_x; x++) {
          /* x + xn * ( y + yn * z ) */
          unsigned short cell_pos = x + col->grid_x * (z + col->grid_z * y);
          unsigned short cell = col->grid_ptr[cell_pos];
          if(cell == 0xFFFF){
            //printf("[XXX] ");
          } else {
            //printf("[%03d] ",cell);
          }
        }
        //printf("\n");
      }
      //printf("\n");
    }
  }

  //printf("%d triangles unaccounted for. (belongs to multiple cells)\n", unnacounted);
  //printf("max cell: x=%d y=%d z=%d\n",max_cell_x,max_cell_y,max_cell_z);

  return col;
}

void Col_Destruct(Col_Info * col) {
  //free3(col->grid_ptr);
  //free3(col);
}

void Col_GetCell(Col2 * info, Col_CellPos * result, SVECTOR * pos) {
  // skip cell if outside bounds
  result->flag = 0;
  if(pos->vx < info->minx || pos->vx > info->maxy) result->flag = 0x0001;
  if(pos->vy < info->miny || pos->vy > info->maxy) result->flag = 0x0002;
  if(pos->vz < info->minz || pos->vz > info->maxy) result->flag = 0x0004;
  if(result->flag) {
    result->x = result->y = result->z = 0;
    return;
  }
    
  result->x = ((long)(pos->vx - (long)info->minx)) >> BACKGROUND_CELL_SHIFT;
  result->y = ((long)(pos->vy - (long)info->miny)) >> BACKGROUND_CELL_SHIFT;
  result->z = ((long)(pos->vz - (long)info->minz)) >> BACKGROUND_CELL_SHIFT;
}


void Col_GetCellPos(Col2 * info, Col_CellPos * result, long x, long y, long z) {
  // skip cell if outside bounds
  result->flag = 0;
  if(x < info->minx || x > info->maxy) result->flag = 0x0001;
  if(y < info->miny || y > info->maxy) result->flag = 0x0002;
  if(z < info->minz || z > info->maxy) result->flag = 0x0004;
  if(result->flag) {
    result-> x = result->y = result->z = 0;
    return;
  }
    
  result->x = ((long)(x - (long)info->minx)) >> BACKGROUND_CELL_SHIFT;
  result->y = ((long)(y - (long)info->miny)) >> BACKGROUND_CELL_SHIFT;
  result->z = ((long)(z - (long)info->minz)) >> BACKGROUND_CELL_SHIFT;
}

unsigned short Col_GetCellElem(Col2 * col, Col_CellPos * pos) {
  return col->grid_ptr[pos->x + col->grid_x * (pos->z + col->grid_z * pos->y)];
}


void Col_GroundCheck() {
  // Order of operation for rejection tests
  // 1 - Triangle Bounding Box Check (Y rejection first, then XZ rejection)
  // 2 - Distance from plane (Plane intersection test) (tells if point is above or below triangle
  // 3 - Point-in-triangle test
  // Expected results: closest triangle to the point and it's distance.
}

/*int Col_PolyInCell(Col2 * col, unsigned short cell, unsigned short face) {
  unsigned short cell_list_id = col->grid_ptr[cell];
  ColBucket * bucket = &col->cell_entry[cell_list_id];
  bucket->floor
}*/

unsigned long fastabs(long value) {
    long mask = value >> sizeof(long) * CHAR_BIT - 1;
    return (value + mask) ^ mask;
}

long fastmin(long x, long y) {
  return y ^ ((x ^ y) & -(x < y));
}

long fastmax(long x, long y) {
  return x ^ ((x ^ y) & -(x < y));
}

long normalmin(long x, long y) {
    return x < y ? x : y;
}

long normalmax(long x, long y) {
    return x > y ? x : y;
}

// From Kornel Kisielewicz: https://stackoverflow.com/questions/2049582/how-to-determine-if-a-point-is-in-a-2d-triangle
long pit_sign (SVECTOR * p1, SVECTOR * p2, SVECTOR * p3)
{
    return (p1->vx - p3->vx) * (p2->vz - p3->vz) - (p2->vx - p3->vx) * (p1->vz - p3->vz);
}

int PointInTriangle (SVECTOR * pt, SVECTOR * v1, SVECTOR * v2, SVECTOR * v3)
{
    long d1, d2, d3;
    long has_neg, has_pos;

    d1 = pit_sign(pt, v1, v2);
    d2 = pit_sign(pt, v2, v3);
    d3 = pit_sign(pt, v3, v1);

    has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(has_neg && has_pos);
}

void Col_GenerateBoundingBoxXYZ(Col_Box * result, SVECTOR * v0, SVECTOR * v1, SVECTOR * v2) {
  result->min_x = fastmin(fastmin(v0->vx, v1->vx), v2->vx);
  result->max_x = fastmax(fastmax(v0->vx, v1->vx), v2->vx);
  result->min_y = fastmin(fastmin(v0->vy, v1->vy), v2->vy);
  result->max_y = fastmax(fastmax(v0->vy, v1->vy), v2->vy);
  result->min_z = fastmin(fastmin(v0->vz, v1->vz), v2->vz);
  result->max_z = fastmax(fastmax(v0->vz, v1->vz), v2->vz);
}

void Col_GenerateBoundingBoxXYZNormal(Col_Box * result, SVECTOR * v0, SVECTOR * v1, SVECTOR * v2) {
  result->min_x = normalmin(normalmin(v0->vx, v1->vx), v2->vx);
  result->max_x = normalmax(normalmax(v0->vx, v1->vx), v2->vx);
  result->min_y = normalmin(normalmin(v0->vy, v1->vy), v2->vy);
  result->max_y = normalmax(normalmax(v0->vy, v1->vy), v2->vy);
  result->min_z = normalmin(normalmin(v0->vz, v1->vz), v2->vz);
  result->max_z = normalmax(normalmax(v0->vz, v1->vz), v2->vz);
}

void Col_GenerateBoundingBoxXZ(Col_Box * result, SVECTOR * v0, SVECTOR * v1, SVECTOR * v2) {
  result->min_x = fastmin(fastmin(v0->vx, v1->vx), v2->vx);
  result->max_x = fastmax(fastmax(v0->vx, v1->vx), v2->vx);
  result->min_z = fastmin(fastmin(v0->vz, v1->vz), v2->vz);
  result->max_z = fastmax(fastmax(v0->vz, v1->vz), v2->vz);
}

void Col_GenerateBoundingBoxY(Col_Box * result, SVECTOR * v0, SVECTOR * v1, SVECTOR * v2) {
  result->min_y = fastmin(fastmin(v0->vy, v1->vy), v2->vy);
  result->max_y = fastmax(fastmax(v0->vy, v1->vy), v2->vy);
}

unsigned short Col_TriBoundingCheck(SVECTOR * pos, SVECTOR * v0, SVECTOR * v1, SVECTOR * v2, Col_Box * box) {
  // Generate a bounding box for this triangle
  Col_GenerateBoundingBoxXYZ(box, v0, v1, v2);

  if(pos->vx < box->min_x || pos->vx > box->max_x) return 0; // outside x axis
  if(pos->vz < box->min_z || pos->vz > box->max_z) return 0; // outside z axis
  // Point is inside bounding box
  return 1;
}



unsigned short Col_TriBoundingCheckPrecalc(SVECTOR * pos, Col_Box * box) {
  if(pos->vx < box->min_x || pos->vx > box->max_x) return 0; // outside x axis
  if(pos->vz < box->min_z || pos->vz > box->max_z) return 0; // outside z axis
  // Point is inside bounding box
  return 1;
}

struct RayPlaneScratch {
  long tnom;
  long tdenom;
  VECTOR sp_p_n, sp_r_p, sp_r_n, sp_r_q;
  VECTOR d;
  long k; 
};

#define BITLIST_POS(a) (a >> 5)
#define BITLIST_BIT(a) (1 << (a & 0x0000001F))

unsigned long test_list [256];

// Temporary test function
short Col_GroundCheckPoint(Col2 * col, SVECTOR * pos, unsigned short dist, Col_Result * result) {
  Col_CellPos cell_pos, cell_pos_end;
  unsigned short cell_element;
  Col_Bucket * bucket;
  Col_PolyList * bucket_node;
  short shortest_depth = 0x8000; // Minimum depth
  short shortest_id = -1;
  unsigned short next;
  unsigned short face_id, i;
  Col_Plane * plane_id = 0;
  //unsigned long * test_list = (unsigned long*)SCRATCH_PAD + sizeof(struct RayPlaneScratch);
  short y_current;
  short y_depth;
  for(i = 0; i < 256; i++){
    test_list[i] = 0;
  }
  // Get current cell id from position
  Col_GetCell(col, &cell_pos, pos); // Get ray top
  Col_GetCellPos(col, &cell_pos_end, pos->vx, pos->vy - dist, pos->vz); // Get ray bottom
  
  y_depth = cell_pos_end.y;
  y_current = cell_pos.y;
  if(y_depth < 0 || y_depth > 32){
    y_depth = 0;
  }
  // if shortest_id != -1 then stop the loop, as we don't need to check lower cells since all of them are below the current one.
  for(; y_current >= y_depth && shortest_id == -1;) {
    cell_pos.y = y_current;

    // Get cell bucket
    cell_element = Col_GetCellElem(col, &cell_pos);

    if(cell_element == 0xFFFF){
      y_current--;
      continue; // No elements in this cell
    } 
    bucket = &col->cell_entry[cell_element];
    if(bucket->floor == 0xFFFF){
      y_current--;
      continue; // No floors to collide against
    }
    // There are floors to collide with, get first node
    bucket_node = &col->poly_entry_list[bucket->floor];
    // Loop until the end of the linked list
    do {
      Col_Box * box;
      Col_Face * face;
      Col_Vert * v0, * v1, * v2;
      unsigned short collide; 

      if(test_list[BITLIST_POS(bucket_node->face)] & BITLIST_BIT(bucket_node->face)) {
        ////printf("testing same face? %d pos: %d bit: %x cont: %x\n", bucket_node->face, BITLIST_POS(bucket_node->face), BITLIST_BIT(bucket_node->face),test_list[BITLIST_POS(bucket_node->face)]);
        if(bucket_node->next == 0xFFFF) break;
          bucket_node++;
          continue; // skip of outside triangle
      }
      test_list[BITLIST_POS(bucket_node->face)] |= BITLIST_BIT(bucket_node->face);
      // Get face
      face = &col->faces[bucket_node->face];
      // Get vertices
      v0 = &col->vertices[face->vert0];
      v1 = &col->vertices[face->vert1];
      v2 = &col->vertices[face->vert2];
      // Generate bounding box and reject
      //collide = Col_TriBoundingCheck(pos, (SVECTOR*)v0, (SVECTOR*)v1, (SVECTOR*)v2, &box);
      box = &bbox_list[bucket_node->face];
      collide = Col_TriBoundingCheckPrecalc(pos, box);
      // If there is collision, do further tests, otherwise, skip
      if(collide != 0){
        // check closest
        
        // Get Plane
        struct RayPlaneScratch * scratch = (struct RayPlaneScratch*) SCRATCH_PAD;
        Col_Plane * plane = &col->planes[face->plane];

        int in_tri = PointInTriangle(pos, (SVECTOR*)v0, (SVECTOR*)v1, (SVECTOR*)v2);

        if(!in_tri){
          if(bucket_node->next == 0xFFFF) break;
          bucket_node++;
          continue; // skip of outside triangle
        }
        
        scratch->sp_p_n.vx = plane->nx0;
        scratch->sp_p_n.vy = plane->ny0;
        scratch->sp_p_n.vz = plane->nz0;
        scratch->sp_r_p.vx = pos->vx;
        scratch->sp_r_p.vy = pos->vy;
        scratch->sp_r_p.vz = pos->vz;
        scratch->sp_r_n.vx = scratch->sp_r_n.vz = 0;
        scratch->sp_r_n.vy = -dist; 

        // Ray Plane Intersection Test
        // plane_distance - dot(plane_normal, ray_position)
        scratch->tnom = plane->distance - ((scratch->sp_p_n.vx * scratch->sp_r_p.vx + scratch->sp_p_n.vy * scratch->sp_r_p.vy + scratch->sp_p_n.vz * scratch->sp_r_p.vz)>>12);
        // dot(plane_normal, ray_normal), since it's only the Y axis, just multiply n * y, as others are always going to result in 0
        scratch->tdenom = (scratch->sp_p_n.vy * scratch->sp_r_n.vy) >> 12;
        // Exit if segment is parallel to plane
        if (scratch->tdenom != 0) {
          // Ensure denominator is positive so it can be multiplied through throughout
          if (scratch->tdenom < 0) {
            scratch->tnom = -scratch->tnom;
            scratch->tdenom = -scratch->tdenom;
          }
            // If t not in [0..1], no intersection
          if (scratch->tnom < 0 || scratch->tnom > scratch->tdenom) {
            if(bucket_node->next == 0xFFFF) break;
            bucket_node++;
            continue;
          } else {
            // Line segment is definitely intersecting plane. Compute vector d to adjust
            // the computation of q, biasing the result to lie on the side of point a
            scratch->k = scratch->tdenom - 1;
            // If a lies behind plane p, round division other way
            // If a lies behind plane p, round division other way
            if (scratch->tdenom > 0) scratch->k = -scratch->k;
            if (scratch->sp_p_n.vx > 0) scratch->d.vx = scratch->k; else if (scratch->sp_p_n.vx < 0) scratch->d.vx = -scratch->k;
            if (scratch->sp_p_n.vy > 0) scratch->d.vy = scratch->k; else if (scratch->sp_p_n.vy < 0) scratch->d.vy = -scratch->k;
            if (scratch->sp_p_n.vz > 0) scratch->d.vz = scratch->k; else if (scratch->sp_p_n.vz < 0) scratch->d.vz = -scratch->k;
            // Compute and return adjusted intersection point
            //ray_q = a + (tnom * ab + d) / tdenom;
            scratch->sp_r_q.vy = scratch->sp_r_p.vy + (scratch->tnom * scratch->sp_r_n.vy + scratch->d.vy) / scratch->tdenom;
            //ray_q = sp_r_q->vy;
            //floor_height = ray_q;

            if(scratch->sp_r_q.vy > shortest_depth) {
              shortest_depth = scratch->sp_r_q.vy;
              shortest_id = bucket_node->face;
              plane_id = plane;
              //FntPrint("tnom=%d\ntdenom=%d\nsdepth=%d\n",scratch->tnom, scratch->tdenom, shortest_depth);
            }
          }
        }

      } 
      if(bucket_node->next == 0xFFFF) break;
      bucket_node++;
    } while(1);
    y_current--;
  }

  result->face = shortest_id;
  result->position = *pos;
  if(shortest_id >= 0){
    result->position.vy = shortest_depth;
  }
  result->plane = plane_id;

  return shortest_id;
}


struct GroundCheckScratch {
  SVECTOR pos;
  Col_PolyList bucket_node;
  short shortest_depth; // Minimum depth
  short shortest_id;
  Col_Box box;
  Col_Face face;
  Col_Vert v0, v1, v2;
  unsigned short collide;
  int in_tri;
};

struct PointInTriangleScratchStruct {
  long d1, d2, d3;
  long has_neg, has_pos;
};

int PointInTriangleScratch (SVECTOR * pt, SVECTOR * v1, SVECTOR * v2, SVECTOR * v3)
{
    struct PointInTriangleScratchStruct * scratch = (struct PointInTriangleScratchStruct*) SCRATCH_PAD + sizeof(struct GroundCheckScratch);

    scratch->d1 = pit_sign(pt, v1, v2);
    scratch->d2 = pit_sign(pt, v2, v3);
    scratch->d3 = pit_sign(pt, v3, v1);

    scratch->has_neg = (scratch->d1 < 0) || (scratch->d2 < 0) || (scratch->d3 < 0);
    scratch->has_pos = (scratch->d1 > 0) || (scratch->d2 > 0) || (scratch->d3 > 0);

    return !(scratch->has_neg && scratch->has_pos);
}

short Col_GroundCheckPointScratch(Col2 * col, SVECTOR * pos) {
    struct GroundCheckScratch scratchPadLocal;
  struct GroundCheckScratch * scratchPad = &scratchPadLocal;
  Col_CellPos cell_pos;
  unsigned short cell_element;
  Col_Bucket * bucket;
  //Col_PolyList * bucket_node;
  //short shortest_depth = 0x8000; // Minimum depth
  //short shortest_id = -1;
  scratchPad->pos = *pos;
  scratchPad->shortest_depth = 0x8000;
  scratchPad->shortest_id = -1;
  // Get current cell id from position
  Col_GetCell(col, &cell_pos, &scratchPad->pos);
  // Get cell bucket
  cell_element = Col_GetCellElem(col, &cell_pos);
  if(cell_element == 0xFFFF) return -1; // No elements in this cell
  bucket = &col->cell_entry[cell_element];
  if(bucket->floor == 0xFFFF) return -1; // No floors to collide against
  // There are floors to collide with, get first node
  scratchPad->bucket_node = col->poly_entry_list[bucket->floor];
  // Loop until the end of the linked list
  do {
    //Col_Box * box;
    //Col_Face * face;
    //Col_Vert * v0, * v1, * v2;
    //unsigned short collide;
    // Get face
    scratchPad->face = col->faces[scratchPad->bucket_node.face];
    // Get vertices
    scratchPad->v0 = col->vertices[scratchPad->face.vert0];
    scratchPad->v1 = col->vertices[scratchPad->face.vert1];
    scratchPad->v2 = col->vertices[scratchPad->face.vert2];
    // Generate bounding box and reject
    //collide = Col_TriBoundingCheck(pos, (SVECTOR*)v0, (SVECTOR*)v1, (SVECTOR*)v2, &box);
    scratchPad->box = bbox_list[scratchPad->bucket_node.face];
    scratchPad->collide = Col_TriBoundingCheckPrecalc(&scratchPad->pos, &scratchPad->box);
    // If there is collision, do further tests, otherwise, skip
    if(scratchPad->collide != 0){
      // check closest
      scratchPad->in_tri = PointInTriangle(&scratchPad->pos, (SVECTOR*)&scratchPad->v0, (SVECTOR*)&scratchPad->v1, (SVECTOR*)&scratchPad->v2);
      if(!scratchPad->in_tri){
        if(scratchPad->bucket_node.next == 0xFFFF) break;
        scratchPad->bucket_node = col->poly_entry_list[scratchPad->bucket_node.next];
        continue; // skip of outside triangle
      }
      if(scratchPad->box.max_y > scratchPad->shortest_depth){
        scratchPad->shortest_depth = scratchPad->box.max_y;
        scratchPad->shortest_id = scratchPad->bucket_node.face;
      }
    } 
    if(scratchPad->bucket_node.next == 0xFFFF) break;
    scratchPad->bucket_node = col->poly_entry_list[scratchPad->bucket_node.next];
  } while(1);

  return scratchPad->shortest_id;
}

short Col_GroundCheckPointScratch2(Col2 * col, SVECTOR * pos) {
  struct GroundCheckScratch * scratchPad = (struct GroundCheckScratch *) SCRATCH_PAD;
  Col_CellPos cell_pos;
  unsigned short cell_element;
  Col_Bucket * bucket;
  //Col_PolyList * bucket_node;
  //short shortest_depth = 0x8000; // Minimum depth
  //short shortest_id = -1;
  scratchPad->shortest_depth = 0x8000;
  scratchPad->shortest_id = -1;
  // Get current cell id from position
  Col_GetCell(col, &cell_pos, pos);
  // Get cell bucket
  cell_element = Col_GetCellElem(col, &cell_pos);
  if(cell_element == 0xFFFF) return -1; // No elements in this cell
  bucket = &col->cell_entry[cell_element];
  if(bucket->floor == 0xFFFF) return -1; // No floors to collide against
  // There are floors to collide with, get first node
  scratchPad->bucket_node = col->poly_entry_list[bucket->floor];
  // Loop until the end of the linked list
  do {
    //Col_Box * box;
    //Col_Face * face;
    //Col_Vert * v0, * v1, * v2;
    //unsigned short collide;
    // Get face
    scratchPad->face = col->faces[scratchPad->bucket_node.face];
    // Get vertices
    scratchPad->v0 = col->vertices[scratchPad->face.vert0];
    scratchPad->v1 = col->vertices[scratchPad->face.vert1];
    scratchPad->v2 = col->vertices[scratchPad->face.vert2];
    // Generate bounding box and reject
    //collide = Col_TriBoundingCheck(pos, (SVECTOR*)v0, (SVECTOR*)v1, (SVECTOR*)v2, &box);
    scratchPad->box = bbox_list[scratchPad->bucket_node.face];
    scratchPad->collide = Col_TriBoundingCheckPrecalc(&scratchPad->pos, &scratchPad->box);
    // If there is collision, do further tests, otherwise, skip
    if(scratchPad->collide != 0){
      // check closest
      scratchPad->in_tri = PointInTriangleScratch(pos, (SVECTOR*)&scratchPad->v0, (SVECTOR*)&scratchPad->v1, (SVECTOR*)&scratchPad->v2);
      if(!scratchPad->in_tri){
        if(scratchPad->bucket_node.next == 0xFFFF) break;
        scratchPad->bucket_node = col->poly_entry_list[scratchPad->bucket_node.next];
        continue; // skip of outside triangle
      }
      if(scratchPad->box.max_y > scratchPad->shortest_depth){
        scratchPad->shortest_depth = scratchPad->box.max_y;
        scratchPad->shortest_id = scratchPad->bucket_node.face;
      }
    } 
    if(scratchPad->bucket_node.next == 0xFFFF) break;
    scratchPad->bucket_node = col->poly_entry_list[scratchPad->bucket_node.next];
  } while(1);

  return scratchPad->shortest_id;
}