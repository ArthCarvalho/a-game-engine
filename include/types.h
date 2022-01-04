/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#ifndef TYPES_H
#define TYPES_H

#define PTRFUNC_1ARG void (*)(void *)
#define PTRFUNC_2ARG void (*)(void *, void *)
#define PTRFUNC_3ARG void (*)(void *, void *, void *)
#define PTRFUNC_3ARGCHAR u_char * (*)(void *, void *, void *)
#define PTRFUNC_4ARGCHAR u_char * (*)(void *, void *, void *, void *)

struct BoundingBox2D {
  short min_x;
  short min_z;
  short max_x;
  short max_z;
};

typedef struct BoundingBox3D {
  short min_x;
  short min_y;
  short min_z;
  short max_x;
  short max_y;
  short max_z;
} BoundingBox3D;

struct CollisionCellResult {
  short x;
  short z;
};

struct CollisionRayResult {
  short first_collide_x;
  short first_collide_z;
  short last_collide_x;
  short last_collide_z;
};

typedef struct {
  short vx;
  short vy;
} ShortVector2D;

typedef struct FlameEffObj {
  SVECTOR pos;
  SVECTOR scale;
  u_char flame_color; // select flame color
  u_char flame_rand; // Randomize flame animation per object
  u_char flicker_frame;
  u_char draw_flare;
  u_short flare_scale;
  short flare_dist; // Distance to display flare effect
  u_char flicker_value;
} FlameEffObj;

#endif