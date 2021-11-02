/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#include "module/gameplay.h"

u_char GameplayModuleState = 0;

// Initialize/Setup the Gameplay Module
void GameplayInitialize() {
  GameplayModuleState = 1;
  /* Load Scene */
  SceneInitialize();
}

// Deletes/Reset Gameplay Module
void GameplayDestroy() {
  GameplayModuleState = 0;
}

void GameplayMain() {
  switch(GameplayModuleState) {
    default:
      GameplayInitialize();
    case 1:
      GameplayLoop();
      break;
   case 2:
      GameplayDestroy();
      break;
  }
}

// Gameplay Main Loop
void GameplayLoop() {
    SceneMain();

    SceneDraw();
}