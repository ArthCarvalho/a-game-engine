/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#ifndef camera_H
#define camera_H

#include "global.h"

#include "actor/actors.h"

#define Camera_SetPosition(camera, x, y, z) ({ \
    (camera)->position.vx = x; \
    (camera)->position.vy = y; \
    (camera)->position.vz = z; \
  })

#define Camera_SetTargetPosition(camera, x, y, z) ({  \
    (camera)->target_pos.vx = x; \
    (camera)->target_pos.vy = y; \
    (camera)->target_pos.vz = z; \
  })

#define Camera_SetTarget(camera, actor) ({ \
  (camera)->target = actor; \
})

extern MATRIX m_identity;
extern MATRIX m_inv_identity;

extern SVECTOR camera_up;
extern VECTOR aspect;

extern short player_floor;
extern short player_floor_forward;
extern short player_floor_forward2;

typedef struct Camera {
  VECTOR target_pos;
  VECTOR position;
  MATRIX matrix;
  VECTOR aspect;
  struct Actor * target;
  SVECTOR target_offset;
  int state;
  short fov;
  short fov_s;
	short rotation_h;
	short rotation_v;
	short rotation_h_s;
	short rotation_v_s;
	short distance;
	short min_dist;
	short max_dist;
	SVECTOR xaxis;
  SVECTOR yaxis;
	SVECTOR zaxis;
	long dot_p_x;
  long dot_p_y;
	long dot_p_z;
	VECTOR target_pos_s; // Smoothed Target Position
	SVECTOR target_offset_s; // Smoothed Target Offset
  // Etc
  u_char target_mode;
  // Quake Mode
  u_char quake_state;
  u_short quake_timer;
  short quake_str;
  short quake_mod;
  short quake_decay;
  short quake_speed;
} CAMERA;

void Camera_AddQuake(struct Camera * cam, short str);

void Camera_Create(struct Camera * cam, void * col);

void Camera_LookAt(struct Camera * cam);

void Camera_Update(struct Camera * cam);

void Camera_SetStartValues(struct Camera * cam);

#endif

