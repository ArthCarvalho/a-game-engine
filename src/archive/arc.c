/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#include "archive/arc.h"


void ARC_Decompress(unsigned long * dest, ARC_Header * src){
  asm_lzdecompress((unsigned long *)src, dest);
}