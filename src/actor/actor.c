/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#include "actor/actor.h"

void ActorCreate(Actor * actor) {
  // Create Actor type: Player
  //if(actor->id == ACTOR_PLAYER){
 //   PlayerCreateInstance(actor);
  //}
}

void Actor_PopulateBase(Actor * actor, Actor_Descriptor * desc) {
  actor->pos.vx = desc->x;
  actor->pos.vy = desc->y;
  actor->pos.vz = desc->z;

  actor->rot.vx = desc->rot_x;
  actor->rot.vy = desc->rot_y;
  actor->rot.vz = desc->rot_z;

  actor->scale.vx = desc->scale_x;
  actor->scale.vy = desc->scale_y;
  actor->scale.vz = desc->scale_z;
  actor->room = desc->room;
  actor->type = desc->actor_type;
}
  
// Checks between two cylinders if they intersect,
// returns true if they do, false if they do not,
// also returns the distance between cylinders and how much they intersect eachother
long ActorCollision_CheckCylinders(CollisionCylinder * a, CollisionCylinder * b, short * dist, short * intersect, short * deltax, short * deltaz) {
  // Y Axis Rejection Test
  short ayheight = a->origin.vy + a->height;
  short byheight = b->origin.vy + b->height;
  if((ayheight < b->origin.vy) || (byheight < a->origin.vy)) {
    // Cylinders not overlapping in Y axys
    return 0;
  }
  short cylradius = a->radius + b->radius;
  // Calculate axis distance between two cylinders in the XZ axis
  *deltax = b->origin.vx - a->origin.vx;
  *deltaz = b->origin.vz - a->origin.vz;
  // Calculate the actual distance between cylinders
  *dist = SquareRoot0((*deltax) * (*deltax) + (*deltaz) * (*deltaz));
  *intersect = cylradius - *dist;
  if(cylradius < *dist) {
    // Cylinders not overlapping in XZ axis
    return 0;
  }
  return 1;
}

void ActorCollision_DisplaceActor(Actor * actor, short dist, short intersect, short deltax, short deltaz) {
  if(dist == 0) return;
  u_long dispfactor = ((u_long)intersect<<12) / dist;
  short disp_x = (deltax * dispfactor) >> 12;
  short disp_z = (deltaz * dispfactor) >> 12;
  actor->collisionData.displacement_x += disp_x;
  actor->collisionData.displacement_z += disp_z;
}