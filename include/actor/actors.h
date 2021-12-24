/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#ifndef ACTORS_H
#define ACTORS_H

#define ACTOR_PLAYER 0x01

//#define ACTOR_GROUP_MAX 8

typedef enum {
    ACTOR_GROUP_PLAYER,
    ACTOR_GROUP_BG,
    ACTOR_GROUP_NPC,
    ACTOR_GROUP_ENEMY,
    ACTOR_GROUP_SWITCH,
    ACTOR_GROUP_DOOR,
    ACTOR_GROUP_CHEST,
    ACTOR_GROUP_OTHER,
    ACTOR_GROUP_MAX
} ActorGroup;

// Common Actor Descriptor
typedef struct Actor_Descriptor {
  short x, y, z;
  short rot_x, rot_y, rot_z;
  short scale_x, scale_y, scale_z;
  unsigned char room;
  unsigned char pad;
  unsigned int actor_type;
  int init_variables[9]; // 36 bytes for initialization
} Actor_Descriptor;

typedef struct {
  short displacement_x;
  short displacement_y;
  short displacement_z;
  short radius;
  short height;
  u_char mass;
  u_char flags;
} Actor_CollisionData;

typedef struct Actor {
  long id; // Actor id
  u_char type; // Actor type
  u_char room; // Room it belongs to, 0xFF = Persistent/No Room
  SVECTOR pos;
  SVECTOR rot;
  SVECTOR scale;
  //short x,y,z; // Map Position
  //short rx,ry,rz; // Map Rotation
  u_short xzDistance;
  u_long xzDistanceSq;
  struct Actor * prev; // Previous actor in actor list
  struct Actor * next; // Next actor in actor list
  struct Actor * parent; // Parent actor if any
  struct Actor * child; // Child actor if any
  unsigned int flags;
  unsigned int visible;
  unsigned int size; // Actor's structure byte size
  struct BoundingBox3D bbox;
  Actor_CollisionData collisionData;
  void (*Initialize)(void*,void*,void*); // Actor's instance initialization function.
  void (*Destroy)(void*,void*); // Actor's destroy instance function.
  void (*Update)(void*,void*); // Actor's main loop function.
  u_char * (*Draw)(void*,void*,void*,void*); // Actor's rendering function.
} Actor;

typedef struct ActorList {
  u_short length;
  struct Actor * start; // Beginning of actor linked list
  struct Actor * end; // End of actor linked list, to accelerate appending new actors
} ActorList;

// Actor Flags
#define ACTOR_STATIC       (1 << 1) // Actor does not interact with the player in any meaningful way, skips calculating distance
#define ACTOR_ACCURATEDIST (1 << 2) // Needs to calculate accurate distance (not settings this may reduce CPU load)
#define ACTOR_INTERACTABLE (1 << 3) // Can interact
#define ACTOR_TARGETABLE   (1 << 4) // Can be targetted

#endif