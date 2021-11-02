/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */
 
/* SOUND SYSTEM */
#ifndef _SOUND_H
#define _SOUND_H

#include "global.h"


// Initialize Sound System
int SoundInit();

// Load Static Background Music (Test Function)
int LoadSoundTest(u_char * vab_hdata, u_char * vab_bdata, u_long * seq_data);

#endif