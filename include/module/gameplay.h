/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#ifndef MODULE_GAMEPLAY_H
#define MODULE_GAMEPlAY_H

#include "global.h"

#include "scene/scene.h"

extern u_char GameplayModuleState;

void GameplayInitialize(); // Gameplay Initialize Module
void GameplayDestroy(); // Gameplay Deinitialize Module

void GameplayMain(); // Gameplay Main Loop

void GameplayLoop();

#endif