#include "actor/flame_common.h"

struct SGM2 * obj_flame_model;
struct SGM2 * obj_flame_high_model;

SVECTOR flare_verts[SCREENFLARE_SEGMENTS2];
u_char flare_index_outter_ring[SCREENFLARE_SEGMENTS_OUTTER];
u_char flare_index_inner[SCREENFLARE_SEGMENTS_INNER];

u_char flicker_frame[SCREENFLARE_FLICKER_FRAMES];

// Flame Color IDs

CVECTOR flare_colors[] = {
  // 0 White
  { 64, 64, 64, 0 },
  // 1 Orange
  { 80, 67, 40, 0 },
  // 2 Yellow
  { 80, 80, 40, 0 },
  // 3 Green
  { 67, 80, 40, 0 },
  // 4 Blue
  { 40, 67, 80, 0 },
  // 5 Purple
  { 80, 40, 80, 0 }
};

CVECTOR flame_colors[] = {
  // 0 White
  { (64/80.0)*256.0, (64/80.0)*256.0, (64/80.0)*256.0, 0 },
  // 1 Orange
  { (80/80.0)*256.0, (67/80.0)*256.0, (40/80.0)*256.0, 0 },
  // 2 Yellow
  { (80/80.0)*256.0, (80/80.0)*256.0, (40/80.0)*256.0, 0 },
  // 3 Green
  { (67/80.0)*256.0, (80/80.0)*256.0, (40/80.0)*256.0, 0 },
  // 4 Blue
  { (40/80.0)*256.0, (67/80.0)*256.0, (80/80.0)*256.0, 0 },
  // 5 Purple
  { (80/80.0)*256.0, (40/80.0)*256.0, (80/80.0)*256.0, 0 }
};

u_char * Draw_ScreenFlare(SVECTOR * pos, MATRIX * view, u_char * packet_ptr, void * scene, u_char flicker, u_short scale, u_char color) {
  POLY_G4 * dest_pg4_ptr = (POLY_G4*) packet_ptr;
  POLY_G3 * dest_pg3_ptr;
  // Draw Outter Ring
  int step4 = 0;
  int step3 = 0;
  u_char flicker_level = flicker_frame[flicker];

  CVECTOR flare_color = flare_colors[color];
  flare_color.r -= flicker_level;
  flare_color.g -= flicker_level;
  flare_color.b -= flicker_level;

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
  long temp0 = (((((camera->aspect.vx * camera->fov_s) / dest.vz) * SCREENFLARE_SIZE) >> 12) * scale) >> 12;
  long temp1 = (((((camera->aspect.vy * camera->fov_s) / dest.vz) * SCREENFLARE_SIZE) >> 12) * scale) >> 12;
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

void Draw_SetupFlame() {
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

void Draw_CreateFlame(FlameEffObj * flame, SVECTOR * pos, SVECTOR * scale, u_char color, u_char show_flare)  {
  flame->flame_color = color;
  flame->flame_rand = rand() >> 9;
  flame->flicker_frame = rand() % SCREENFLARE_FLICKER_FRAMES;
  flame->draw_flare = show_flare;
  flame->pos = *pos;
  flame->scale = *scale;
  flame->flare_scale = 4096;
  flame->flare_dist = -1;
}

// Simplified SGM renderer for flames
u_char * Draw_Flame(FlameEffObj * flame, MATRIX * view, u_char * packet_ptr, void * scene) {
  MATRIX local_identity;
  POLY_GT3 * dest_pgt3_ptr;
  SGM2_POLYGT3 * pgt3_ptr;
  struct SGM2 * model = obj_flame_model;
  u_char show_flare = 1;

  {
    MATRIX flame_matrix;
    // Update Flame
    struct Scene_Ctx * scene_ctx = (struct Scene_Ctx*)scene;
    struct Camera * camera = scene_ctx->camera;

    flame_matrix.m[0][0] = camera->zaxis.vx;
    flame_matrix.m[0][1] = camera->zaxis.vy;
    flame_matrix.m[0][2] = camera->zaxis.vz;
    flame_matrix.m[1][0] = 0;
    flame_matrix.m[1][1] = 4096;
    flame_matrix.m[1][2] = 0;
    flame_matrix.m[2][0] = camera->zaxis.vz;
    flame_matrix.m[2][1] = camera->zaxis.vy;
    flame_matrix.m[2][2] = -camera->zaxis.vx;
    flame_matrix.t[0] = flame->pos.vx;
    flame_matrix.t[1] = flame->pos.vy;
    flame_matrix.t[2] = flame->pos.vz;
    flame->flicker_frame++;
    flame->flicker_frame %= SCREENFLARE_FLICKER_FRAMES;
    flame->flicker_value = flicker_frame[flame->flicker_frame];

    VECTOR scale = {flame->scale.vx, flame->scale.vy, flame->scale.vz, 0 };
    ScaleMatrix(&flame_matrix, &scale);
    CompMatrixLV(view, &flame_matrix, &local_identity);
  }

  gte_SetRotMatrix(&local_identity);
  gte_SetTransMatrix(&local_identity);

  {
    u_int i;
    u_short tpageid, tpageido;
    u_short clutid, clutido;
    u_short pg_count;
    u_char v_offs = flame->flame_rand;
    
    SVECTOR vec0, vec1;
    long otzv;
    vec0.vx = 0;
    vec0.vy = 0;
    vec0.vz = 0;
    gte_ldv0(&vec0);
    gte_rtps();
    gte_stsxy((long*)&vec1.vx);
    gte_stsz(&otzv);
    // Z culling
    if(otzv < 0) return packet_ptr;
    // Frustum culling
    if(vec1.vx < -64 || vec1.vx > SCREEN_W+64) return packet_ptr;
    if(vec1.vy < -64 || vec1.vy > SCREEN_H+64) return packet_ptr;
    if(otzv < 4000) model = obj_flame_high_model;

    if(flame->flare_dist >= 0 && otzv > flame->flare_dist ) {
      show_flare = 0;
    }

    clutido = clutid = model->material[0].clut;
    tpageido = tpageid = model->material[0].tpage;

    clutid += (flame->flame_color << 6);
    tpageido = (tpageido & ~(1<<5)) | (2 << 5);
    
    // Transform to buffer
    AGM_TransformModel(model->vertex_data, model->vertex_count);

    pgt3_ptr = model->poly_gt3;

    dest_pgt3_ptr = (POLY_GT3*)packet_ptr;
    pg_count = model->poly_gt3_count;

    for(i = 0; i < pg_count; i++, pgt3_ptr++) {
        long outer_product, otz;
        long tempz0,tempz1,tempz2;
        SVECTOR * vec0 = &AGM_TransformBuffer[pgt3_ptr->idx0];
        SVECTOR * vec1 = &AGM_TransformBuffer[pgt3_ptr->idx1];
        SVECTOR * vec2 = &AGM_TransformBuffer[pgt3_ptr->idx2];
              
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
          
          {
            POLY_GT3 * prev = dest_pgt3_ptr;
          
            addPrim(G.pOt+otz, dest_pgt3_ptr);

            dest_pgt3_ptr++;

            *dest_pgt3_ptr = *prev; // Copy Contents
            addPrim(G.pOt+otz, dest_pgt3_ptr);
            dest_pgt3_ptr++;

            //*dest_pgt3_ptr = *prev; // Copy Contents
            //addPrim(G.pOt+otz, dest_pgt3_ptr);
            //dest_pgt3_ptr++;
          }

        }
    }

    packet_ptr = (u_char*) dest_pgt3_ptr;
  }

  if(flame->draw_flare && show_flare) {
    SVECTOR flare_pos = flame->pos;
    flare_pos.vy += 32;
    return Draw_ScreenFlare(&flame->pos, view, packet_ptr, scene, flame->flicker_frame, (flame->scale.vy * flame->flare_scale) >> 12, flame->flame_color);
  } else {
    return packet_ptr;
  }
}