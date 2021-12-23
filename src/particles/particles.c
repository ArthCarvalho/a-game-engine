/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#include "particles/particles.h"

#include "spadstk.h"

/*
SVECTOR pos = { particle.x, particle.y particle.z };
long zdepth;
short final_size, p_sin, p_cos;
gte_ldv0(&pos);
gte_rtps();
gte_stsxy((long*)&pos.vx);
gte_stsz(&zdepth);
final_size = (particle.size << 12) / zdepth;
p_sin = (sin(particle.angle) * final_size)>>12;
p_cos = (cos(particle.angle) * final_size)>>12;
*/

// One shot particle emitter
void Particle3D_Create(struct ParticleEmitter * emitter) {
  if(!emitter->buffer) {
    emitter->buffer = Arena_Malloc(sizeof(struct Particle3D) * emitter->max_particles);
  }
  emitter->flags = 1; // Activate emitter
  struct Particle3D * particle = emitter->buffer;
  short generator_length = emitter->generator_radius << 1;
  // Initialize particles
  for(u_int i = 0; i < emitter->max_particles; i++) {
    // Generate random position from center of emitter
    short local_pos_x = (rand() * generator_length / RAND_MAX);
    short local_pos_y = (rand() * generator_length / RAND_MAX);
    short local_pos_z = (rand() * generator_length / RAND_MAX);
    // Spread between positive and negative axis
    if(rand() > RAND_MAX / 2) local_pos_x = -local_pos_x;
    if(rand() > RAND_MAX / 2) local_pos_y = -local_pos_y;
    if(rand() > RAND_MAX / 2) local_pos_z = -local_pos_z;
    
    // Convert emitter local position into world position
    particle[i].x = emitter->position.vx - local_pos_x;
    particle[i].y = emitter->position.vy - local_pos_y;
    particle[i].z = emitter->position.vz - local_pos_z;
    particle[i].rx = rand() >> 3;
    particle[i].rx = rand() >> 3;
    particle[i].rx = rand() >> 3;
    particle[i].size = emitter->size + ((rand() >> 9) - 31);
    particle[i].lifetime = (emitter->starting_lifetime * (2048 + (rand() >> 4))) >> 12;
    if(emitter->flags & PARTICLE_RENDER_AMBIENT) {
      gte_ldrgb((CVECTOR*)&emitter->r0);
      gte_cc();
      gte_strgb((CVECTOR*)&particle[i].r0);
    } else {
      particle[i].r0 = emitter->r0;
      particle[i].g0 = emitter->g0;
      particle[i].b0 = emitter->b0;
    }
    particle[i].a0 = 0xFF;
    // Set particle initial speed - Explosion type (Spread from center)
    // TODO - Improve initial speed generation
    particle[i].sx = -((local_pos_x + ((rand() >> 9) - 31)) >> 3);
    particle[i].sy = -((local_pos_y + emitter->center_offset + ((rand() >> 9) - 31))>>2);
    particle[i].sz = -((local_pos_z + ((rand() >> 9) - 31)) >> 3);
    // Set particle initial rotation speed
    particle[i].srx = (emitter->rotation.vx * (2048 + (rand() >> 4))) >> 12;
    particle[i].sry = (emitter->rotation.vy * (2048 + (rand() >> 4))) >> 12;
    particle[i].srz = (emitter->rotation.vz * (2048 + (rand() >> 4))) >> 12;
    // randomly swap rotation direction
    if(rand() > RAND_MAX / 2){
      particle[i].srx = -particle[i].srx;
    }
    if(rand() > RAND_MAX / 2){
      particle[i].sry = -particle[i].sry;
    }
    if(rand() > RAND_MAX / 2){
      particle[i].srz = -particle[i].srz;
    }

  }
}

void Particle3D_Destroy(struct ParticleEmitter * emitter) {
  Arena_Free(emitter->buffer);
  emitter->buffer = NULL;
}

// Update and draw particles
u_char * Particle3D_Update(struct ParticleEmitter * emitter, MATRIX * view, u_char * prim_buff) {
  struct Particle3D * particle = emitter->buffer;
  u_long active_particles = 0;
  for(u_int i = 0; i < emitter->max_particles; i++) {
    if(particle[i].lifetime) { // Skip dead particles
      // Update position and rotation
      long otz;
      short size = particle[i].size >> 1;
      particle[i].lifetime--;
      active_particles++;

      particle[i].sx += emitter->gravity.vx;
      particle[i].sy += emitter->gravity.vy;
      particle[i].sz += emitter->gravity.vz;

      // Limit vertical speed
      if(particle[i].sy < emitter->min_gravity_speed) particle[i].sy = emitter->min_gravity_speed;

      particle[i].x += particle[i].sx;
      particle[i].y += particle[i].sy;
      particle[i].z += particle[i].sz;

      particle[i].rx = (particle[i].rx + particle[i].srx) & 0xFFF;
      particle[i].ry = (particle[i].ry + particle[i].sry) & 0xFFF;
      particle[i].rz = (particle[i].ry + particle[i].srz) & 0xFFF;

      if(particle[i].y < emitter->particle_floor) {
        particle[i].lifetime = 0;
        continue;
      }

      // Draw
      // Generate plane vertex
      SVECTOR vertex[4];
      SVECTOR screen_vertex[4];
      vertex[0].vx = -size;
      vertex[0].vy = 0;
      vertex[0].vz = -size;
      vertex[1].vx = size;
      vertex[1].vy = 0;
      vertex[1].vz = -size;
      vertex[2].vx = -size;
      vertex[2].vy = 0;
      vertex[2].vz = size;
      vertex[3].vx = size;
      vertex[3].vy = 0;
      vertex[3].vz = size;

      MATRIX local_identity;
      MATRIX particle_matrix;
      local_identity.t[0] = particle[i].x;
      local_identity.t[1] = particle[i].y;
      local_identity.t[2] = particle[i].z;
      SVECTOR rot;
      rot.vx = particle[i].rx;
      rot.vy = particle[i].ry;
      rot.vz = particle[i].rz;
      RotMatrix_gte(&rot, &local_identity);
      CompMatrixLV(view, &local_identity, &particle_matrix);
      gte_SetRotMatrix(&particle_matrix);
      gte_SetTransMatrix(&particle_matrix);
      // Load screen XY coordinates to GTE Registers
      gte_ldv3c(vertex);
      gte_rtpt();
      gte_avsz3();
      // Store value to otz
      gte_stotz(&otz);
      gte_stsxy3((long *)&screen_vertex[0].vx,(long *)&screen_vertex[1].vx,(long *)&screen_vertex[2].vx);
      gte_ldv0(&vertex[3]);
      gte_rtps();
      gte_stsxy((long *)&screen_vertex[3].vx); 

      // Average Z
      //gte_avsz4();
      // Store value to otz
      //gte_stotz(&otz);
      // Reduce OTZ size
      otz = (otz >> OTSUBDIV);
      //otz = 50;
      if (otz > OTMINCHAR && otz < OTSIZE) {
        POLY_FT4 * particle_prim = (POLY_FT4 *)prim_buff;

        setPolyFT4(particle_prim);
        particle_prim->tpage = emitter->tpage;
        particle_prim->clut = emitter->clut;
        if(particle[i].lifetime < 15) {
          particle_prim->clut += 1<<6;
        }
        setSemiTrans(particle_prim, 1);
        particle_prim->r0 = particle[i].r0;
        particle_prim->g0 = particle[i].g0;
        particle_prim->b0 = particle[i].b0;

        *(long*)(&particle_prim->x0) = *(long*)(&screen_vertex[0].vx);
        *(long*)(&particle_prim->x1) = *(long*)(&screen_vertex[1].vx);
        *(long*)(&particle_prim->x2) = *(long*)(&screen_vertex[2].vx);
        *(long*)(&particle_prim->x3) = *(long*)(&screen_vertex[3].vx);

        u_char u0 = emitter->u0;
        u_char v0 = emitter->v0;
        u_char u0w = u0 + 32;
        u_char v0h = v0 + 32;


        particle_prim->u0 = u0;
        particle_prim->v0 = v0;

        particle_prim->u1 = u0w;
        particle_prim->v1 = v0;

        particle_prim->u2 = u0;
        particle_prim->v2 = v0h;

        particle_prim->u3 = u0w;
        particle_prim->v3 = v0h;
        
        addPrim(G.pOt+otz, particle_prim);
        particle_prim++;
        prim_buff = (u_char*)particle_prim;
      }
    }
  }
  if(active_particles == 0) {
    emitter->flags = 0; // Deactivate emitter
    Particle3D_Destroy(emitter);
    emitter->buffer = NULL;
  }
}