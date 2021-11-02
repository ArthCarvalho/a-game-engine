/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#ifndef _ARC_H_
#define _ARC_H_

#include "archive/lz77.h"
/*
  ARC - Compressed Archive File Format
  Uses LZ77 compression
*/

typedef struct ARC_Header {
    unsigned long id;
    unsigned long compressed_size;
    unsigned long decompressed_size;
    unsigned long hash;
} ARC_Header;

void ARC_Decompress(unsigned long * dest, ARC_Header * src);

#endif