/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#ifndef _PARTICLES_H_
#define _PARTICLES_H_

/*#include <sys/types.h>
#include <libgte.h>
#include <libgpu.h>

#include "types.h"*/

#include "global.h"

typedef struct ParticleEmitter {
  SVECTOR position;         // Particle source
  SVECTOR rotation;         // Particle rotation speed
  SVECTOR gravity;          // Grativy / Wind
  short initial_velocity;   // Particle starting speed
  short friction;           // Particle deacceleration factor
  short starting_lifetime;  // Emitted particle lifetime
  short size;               // Particle size
  short min_gravity_speed;  // Minimum speed when falling
  short tpage;              // Particle texture page
  short clut;               // Particle clut
  u_char u0, v0;            // Particle texture coordinates
  u_char r0, g0, b0;        // Base particle color;
  u_char max_particles;     // Maximum live particles at once
  u_short generator_radius; // Size of the spherical area where particles will be generated
  short center_offset;      // Starting point of particle direction vector (vertical offset only for now)
  short particle_floor;     // Lowest a particle can go, in world space
  void * buffer;            // Pointer to allocated particle buffer
  u_long flags;             // Status flags, if true is active
} ParticleEmitter;

typedef struct Particle2D {
  short x, y, z;        // World position
  short angle;          // Screen angle of rotation
  short size;           // World size
  short sx, sy, sz;     // Speed vector
  short lifetime;       // Particle lifetime
  u_char r0, g0, b0;    // Particle color
  u_char a0;            // Particle alpha / transparency level
  u_char pad[2];
} Particle2D;

typedef struct Particle3D {
  short x, y, z;        // World position
  short rx, ry, rz;     // Screen angle of rotation
  short size;           // World size
  short sx, sy, sz;     // Speed vector
  short srx, sry, srz;  // Rotation speed
  short lifetime;       // Particle lifetime
  u_char r0, g0, b0;    // Particle color
  u_char a0;            // Particle alpha / transparency level
} Particle3D;


void Particle3D_Create(struct ParticleEmitter * emitter);

void Particle3D_Destroy(struct ParticleEmitter * emitter);

u_char * Particle3D_Update(struct ParticleEmitter * emitter, MATRIX * view, u_char * prim_buff);

#endif // _PARTICLES_H_