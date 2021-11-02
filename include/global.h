/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

/* global.h */
#ifndef _GLOBAL_H
#define _GLOBAL_H
/* CD ROM LBAs */
#include "../cdromdata.h"

/* PSX Library Files */
#include <memory.h>
#include <sys/types.h>
/*#include <libmath.h>*/
#include <stdio.h>
#include <stdlib.h>
#include <kernel.h>
#include <libapi.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libcd.h>
#include <libds.h>
#include <libsnd.h>

#include <inline_n.h>
#include <GTEMAC.h>

extern u_long *DivideGT3( SVECTOR *v0, SVECTOR *v1, SVECTOR *v2, u_long *uv0, u_long *uv1, u_long
*uv2, CVECTOR *rgb0, CVECTOR *rgb1, CVECTOR *rgb2, POLY_GT3 *s, u_long *ot, DIVPOLYGON3
*divp);

extern u_long *DivideGT4( SVECTOR *v0, SVECTOR *v1, SVECTOR *v2, SVECTOR *v3, u_long *uv0, u_long
*uv1, u_long *uv2, u_long *uv3, CVECTOR *rgb0, CVECTOR *rgb1, CVECTOR *rgb2, CVECTOR *rgb3,
POLY_GT4 *s, u_long *ot, DIVPOLYGON4 *divp);

/* Global Types */
#include "types.h"

/* Allocators */
#include "memory/lstack.h"
#include "memory/arena.h"

/* Archive and Compression */
#include "archive/lz77.h"
#include "archive/arc.h"

/* Math */
#include "math/fpmath.h"
#include "math/math.h"
#include "math/isin.h"
#include "math/math.h"

/* Collision */
#include "collision/collision.h"

/* Camera */
#include "camera/camera.h"

/* Game Files */
#include "sound/sound.h"
#include "input/input.h"

/* Models */
#include "model/sgm.h"
#include "model/agm.h"
#include "model/sgm2.h"

/* MODULES */
#include "module/gameplay.h"

/* ACTORS */
#include "actor/actors.h"
#include "actor/actor.h"
#include "actor/a_player.h"

/* Particles */
//#include "particles/particles.h"

/* scratch pad address 0x1f800000 - 0x1f800400 */
#define SCRATCH_PAD 0x1f800000
#define SCRATCH_PAD_END 0x1f800400
#define SCRATCH_PAD_SIZE 0x400

#define RAM_SIZE 0x200000

/* screen resolution */
#define camera_ASPECT 6553
#define SCREEN_W	512
//#define camera_ASPECT 4096
//#define SCREEN_W 320
#define	SCREEN_H 240
#define BUFFER_A_PX 1024-SCREEN_W
#define BUFFER_A_PY 0
#define BUFFER_B_PX 1024-SCREEN_W
#define BUFFER_B_PY 256

/* GPU Packet Area Size */
#define OTSIZE 4096
#define OTSUBDIV 1
#define OTMIN 15
#define OTMINCHAR 1
//#define PACKET_SIZE 0x19000
#define PACKET_SIZE 0x50000

typedef struct tagGsEnv
{
    // rendering related structures
    DRAWENV Draw_env[2];
    DISPENV Disp_env[2];
    DRAWENV *pDraw;
    DISPENV *pDisp;
    u_long *pOt;                    // current OTag pointer
    u_short OTag_id;                // current OTag index, flip every frame
    u_char VSync_rate;              // 0 = 60 fps, 2 = 30 fps
    u_char clear_mode;              // 0 = clear with rgb, 1 = no clear
    short Screen_x, Screen_y;       // option menu adjustments
    u_short Screen_w, Screen_h;     // screen size, internal usage
    u_char * pBuffer;               // Primitive Buffer
    CVECTOR clear;                  // clear color
    RECT screen_rect;
    /* Module Data */
    u_long module;                  // Active Module
    u_long module_state;            // Active Module Current State
} GS_ENV;

enum MODULE_TABLE{
	MODULE_DEBUG_START,
	MODULE_MAIN_LOOP,
  MODULE_GAMEPLAY
};

/* From LIBGS */
/*** packet peripheral pointer ***/
typedef unsigned char PACKET;

/* Player Input Variables - Player 1 */
extern u_long g_pad;
extern u_long g_pad_press;
extern u_long g_pad_hold;
extern u_char pad_analog[4];

extern GS_ENV G;

extern unsigned long _ramsize; /* force 2 megabytes of RAM */
extern unsigned long _stacksize; /* force 16 kilobytes of stack */

extern unsigned long __heap_base;
extern unsigned long __heap_start;
extern unsigned long __heap_size;
extern unsigned long __bss;
extern unsigned long __bss_end;
extern unsigned long __stack_size;
//extern unsigned long __bsslen;

extern unsigned int __DEBUG_TOGGLE;

/* Ordering Table and GPU Packet settings */
/* TODO: find a better place for these */
extern u_long ot[2][OTSIZE];
extern PACKET gpu_packet[2][PACKET_SIZE]; // Packet buffer for GS functions

extern unsigned long load_symbol[];

// Temporary buffer to store transformed vertices for animated models
extern long TransformBufferSize;
extern SVECTOR * TransformBuffer;

void  ResetGsEnv();
void  BeginDraw();
int   EndDraw();

int AsyncLoadFile();

void load_screen_setup();
void draw_load_screen();

void file_load_temp(char * filename, unsigned long ** dest);
unsigned long file_load_temp_noalloc(char * filename, unsigned long * dest);

#endif