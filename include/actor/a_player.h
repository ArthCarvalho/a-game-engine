/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#ifndef ACTOR_PLAYER_H
#define ACTOR_PLAYER_H

#include "global.h"
#include "model/agm.h"

//#include "collision/collision.h"

#define PLAYER_ENABLE_CONTROL 0x80000000 // This bit enables player control
//#define PLAYER_STATE_MOVING 0x00000001 // Player is moving
//#define PLAYER_STATE_ROT_LEFT 0x00000002 // Player is rotating left
//#define PLAYER_STATE_ROT_RIGHT 0x00000004 // Player is rotating right

//#define PLAYER_STATE_ON_AIR 0x00000010 // Player is not on solid ground

// Test with: ((state & 0x0000000F) == PLAYER_STATE_NAME)
#define PLAYER_STATE_MASK         0xFFFFFFF0
#define PLAYER_STATE_WAIT         0x00000000
#define PLAYER_STATE_MOVE         0x00000001
#define PLAYER_STATE_ROLL         0x00000002
#define PLAYER_STATE_ATTACK       0x00000003
#define PLAYER_STATE_JUMP         0x00000004
#define PLAYER_STATE_JUMPGRAB     0x00000005
#define PLAYER_STATE_HOLDLEDGE    0x00000006
#define PLAYER_STATE_CLIMBING     0x00000007 
#define PLAYER_STATE_HOLDSHIELD   0x00000008
#define PLAYER_STATE_FIRSTPERSON  0x00000009
#define PLAYER_STATE_AIMING       0x0000000A
#define PLAYER_STATE_ONAIR        0x00000010

/*
typedef struct NewPlayerActor {
  struct Actor actor; // Base class

  struct AGM_Model player_model;
  struct ANM_Animation * player_anim;
  struct ANM_Animation * player_anim_dynamic; // Dynamically loaded animation


} Player_Actor;
*/
typedef struct PlayerActor{
  struct Actor base;
  struct Actor * holding; // Object being held by Player
  struct Actor * interact; // Actor/Object Player will/is interacting
  short interact_dist; // Distance from interact object/actor
  // Character States
  u_int state; // State Flags
  u_int state_sub;
  u_short current_anim;
  u_short anim_frame; // Animation Frame Decimal
  u_short anim_frame_sub; // Animation Frame Mantissa (for interpolated animations)
  u_short anim_no_loop;
  // Camera Direction Data (Used for player movement)
  // Horizontal Plane (XZ)
  SVECTOR xaxis; // Camera View X-Axis
  SVECTOR zaxis; // Camera View Z-Axis
  // Movement Related
  DVECTOR xzvector; // XZ direction
  VECTOR move;
  short y_move_dir;
  long xzspeed; // XZ speed, stored as fixed point (<< 12);
  long maxspeed; // Maximum Movment Speed
  long x_position;
  long y_position;
  long z_position;
  short y_rotation;
  long speed_fall;
  long gravity;
  long max_speed_y;

  char void_out;
  short last_floor_pos_x;
  short last_floor_pos_y;
  short last_floor_pos_z;
  short last_floor_dir;
  unsigned char last_floor_room;

  MATRIX * L_Hand_matrix;

  // Collision
  struct ColResult * floor;

  void (*process)(Actor *);
} PlayerActor;

// Player Data Load Status
// 0x0001 Model Loaded
// 0x0002 Textures Loaded
extern int PlayerDataLoaded;

extern struct AGM_Model PlayerMdl;
extern struct ANM_Animation * PlayerAnm;

// Prepare Data (Load Assets)
void PlayerLoadData();
void PlayerSetupData(u_long * mdl, u_long * anim);
// Unload/Remove Player Model/Texture data from RAM/VRAM
void PlayerUnloadData();

// Initialize a player instance
void PlayerCreateInstance(struct Actor * a, void * col_ctx);

// Player Functions
void PlayerInitialize(struct Actor * a);
void PlayerDestroy(struct Actor * a);
void PlayerUpdate(struct Actor * a, void * scene);
u_char * PlayerDraw(struct Actor * a, MATRIX * view, u_char * pbuff, void * scene);

// Player Internal Functions
extern long accel_up;
extern long accel_down;
long PlayerAccelerate(long accel, long current);

extern long anim_spd;

extern SVECTOR frameBuffer[20];

// State function process
void Player_Normal(Actor * player);
void Player_Move(Actor * player);
void Player_Jump(Actor * player);
void Player_Falling(Actor * player);



#endif