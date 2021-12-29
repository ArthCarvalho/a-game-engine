/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#ifndef ACTOR_H
#define ACTOR_H

#include "global.h"

typedef struct {
  SVECTOR origin;
  short radius;
  short height;
} CollisionCylinder;

// Actor ID, position, etc. must be filled before calling this function
void ActorCreate(struct Actor * actor);

// Populate basic actor data (Position, Rotation, Scale)
void Actor_PopulateBase(Actor * actor, Actor_Descriptor * desc);

// Compile local matrix
void Actor_CompileMatrix(Actor * actor, MATRIX * dest);

// Actor vs Actor Collision Functions

// Cylinder vs Cylinder check
long ActorCollision_CheckCylinders(CollisionCylinder * a, CollisionCylinder * b, short * dist, short * intersect, short * deltax, short * deltaz);

void ActorCollision_DisplaceActor(Actor * actor, short dist, short intersect, short deltax, short deltaz);

#endif