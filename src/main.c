/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#include "global.h"
#include "texture/texture.h"

/* TEMPORARY STATIC DATA FILES */
extern unsigned long saveicon[];
extern unsigned long load_symbol[];
//extern unsigned long test_tim[];
//extern unsigned long test_sgm[];
//extern unsigned long test_tim[];
//extern unsigned long test_sgm[];
//extern unsigned long test_playerobj[];
//extern unsigned long test_cube[];
//extern unsigned long test_room_collision[];

/* END OF TEMPORARY FILES */

/* Memory Card Stuff */
/*
// Single Block, 3 icons
typedef struct {
	char	Magic[2];
	char	Type;
	char	BlockEntry;
	char	Title[64];
	char	reserve[28];
	char	Clut[0x20];
	char	Icon[3][0x80];
  char  Data[0x1E00];
} _CARD;
*/

// Single Block, 1 icon
typedef struct {
	char	Magic[2];
	char	Type;
	char	BlockEntry;
	char	Title[64];
	char	reserve[28];
	char	Clut[0x20];
	char	Icon[0x80];
  char  Data[0x1F00];
} _CARD;


typedef struct {
  unsigned short curent_map;
  SVECTOR pos;
  SVECTOR rot;
  unsigned short max_life;
  unsigned short max_magic;
  unsigned short life;
  unsigned short magic;
  char name[16];
  unsigned long checksum;
} _SAVEDATA; // Max 7680 (0x1E00) bytes

TIMIMAGE save_tim;
static _CARD HEAD;

/* TODO: Check for a button combo to start additional debug menu */
void module_debug_start() {
  //printf("module_debug_start\n");
  G.clear.r = 0;
	G.clear.g = 0;
	G.clear.b = 0;
  
  BeginDraw();
  EndDraw();
  FntFlush(-1);
  
  // Nothing to do here, loop into actual main loop
  G.module = MODULE_MAIN_LOOP;
}

void module_main_loop() {
  DslFILE		fp;
  unsigned char com;
  static u_char load_state = 0;
  static int nsector;
  static int cnt;
  //printf("module_main_loop\n");
  G.clear.r = 0;
	G.clear.g = 0;
	G.clear.b = 0;
  //G.clear.r = 180;
	//G.clear.g = 180;
	//G.clear.b = 180;

  if (load_state == 0) {
    load_state = 1;
  }
  
  G.module = MODULE_GAMEPLAY;
}

/* Game Main Loop Table */
/* Actual Table */
void ( * main_loop_table[] ) (void) = {
	module_debug_start,	    // 0x00 Init Debug Menu
	module_main_loop,			  // 0x01 Initialize State
	GameplayMain		        // 0x02 Gameplay State
	//debug_menu_loop,		  // 0x03 DEBUG MENU
	//level_main_loop,
	//map_main_loop,
};

void main() {
  RECT rect;
  unsigned int * alt;
  
  ////printf("Heap base: %x\nHeap size: %x\n",__heapbase,__heapsize);
  ////printf("BSS start: %x\nBSS length: %x\n",__bss,__bsslen);
  
  // EnterCriticalSection();
  // InitHeap3((void*)__heapbase, __heapsize);
  // ExitCriticalSection();  
  
  SetDispMask(0);
  
  /* FORCE NTSC */
	SetVideoMode(VIDEO_MODE);
  /* Initializes all system callbacks. OTHERWISE THE SYSTEM MAY HANG */
	ResetCallback();
  /* Init Decompressor */
	//DecDCTReset(0);
  /* Init CD. Should be init only after SS/SPU */
  //CdInit();
  //CdSetDebug(0);
  DsInit();
  DsSetDebug(0);
  /* Initialize Input Related Stuff Here*/
  MemCardInit(1);
  MemCardStart();
  InputInit();
  /* Init Graph */
	ResetGraph(0);	
  /* Sound */
  SoundInit();
  /* Init GTE */
	InitGeom();
  /* Set Viewport Settings */
  gte_SetGeomOffset(SCREEN_W/2,SCREEN_H/2);
	gte_SetGeomScreen(256);
  /* Init Debug */
	//DsSetDebug(0);
  /* Disable Graphics Debug */
	SetGraphDebug(0);
  /* Clear Whole VRAM/Framebuffer to black */
	rect.x = 0;
	rect.y = 0;
	rect.w = 1024;
	rect.h = 512;
	ClearImage(&rect,0,0,0);
  
  //G.clear.r = G.clear.g = G.clear.b = 0;
  //G.clear.r = 255;
	G.clear.r = 63;
	G.clear.g = 204;
	G.clear.b = 252;
	G.VSync_rate = 0; // Framerate: 0: 60FPS, 2: 30FPS, 3: 20FPS
	G.clear_mode = 0;
  
  /* Set Display/Draw Environment */
  SetDefDrawEnv(&G.Draw_env[0], BUFFER_A_PX, BUFFER_A_PY, SCREEN_W, SCREEN_H);
  SetDefDispEnv(&G.Disp_env[0], BUFFER_B_PX, BUFFER_B_PY, SCREEN_W, SCREEN_H);
  SetDefDrawEnv(&G.Draw_env[1], BUFFER_B_PX, BUFFER_B_PY, SCREEN_W, SCREEN_H);
  SetDefDispEnv(&G.Disp_env[1], BUFFER_A_PX, BUFFER_A_PY, SCREEN_W, SCREEN_H);
  
  G.Disp_env[0].isinter = G.Disp_env[1].isinter = 0; // Interlace
	G.Disp_env[0].isrgb24 = G.Disp_env[1].isrgb24= 0; // 24 bit

  if(VIDEO_MODE) {
    // PAL Vertical Fix
    G.Disp_env[0].screen.y += 24;
    G.Disp_env[1].screen.y += 24;
  }
  
  G.Draw_env[0].dfe = G.Draw_env[1].dfe = 1; // Enable Drawing
	G.Draw_env[0].dtd = G.Draw_env[1].dtd = 1; // Enable Dithering

  G.VSync_rate = 2; // Framerate: 0: 60FPS, 2: 30FPS, 3: 20FPS
  //G.VSync_rate = 3; // Framerate: 0: 60FPS, 2: 30FPS, 3: 20FPS

  //G.VSync_rate = 8; // Framerate: 0: 60FPS, 2: 30FPS, 3: 20FPS
  
  /* Set clip region */
	setRECT( &G.screen_rect, 0, 0, SCREEN_W, SCREEN_H );

  local_clip = G.screen_rect;
  
  /* load debug font */
	FntLoad(1024-64-SCREEN_W, 0);
	SetDumpFnt(FntOpen(4, 12, SCREEN_W-(8), SCREEN_H-(12), 0, 2048)); 
  
  load_screen_setup();

  ///* Load and play test BGM */
  //LoadSoundTest();
  
  /* Waits for ClearImage to finish */	
	DrawSync(0);
  
  /* Set Current Display Environment */
	PutDispEnv(&G.Disp_env[0]);
	/* Set Current Drawing Buffer */
	PutDrawEnv(&G.Draw_env[0]);
  
  /* Set Display Mask to Display (enable display output) */
	SetDispMask(1);

  
  /* Memory Card Debug Generate Save */
  GetTimInfo(saveicon,&save_tim);
  //printf("Waiting for memory card...\n");
  {
    long channel = 0;
    unsigned long result;

    _SAVEDATA save;

    typedef struct {
  unsigned short curent_map;
  SVECTOR pos;
  SVECTOR rot;
  unsigned short max_life;
  unsigned short max_magic;
  unsigned short life;
  unsigned short magic;
  char name[16];
  unsigned long checksum;
} _SAVEDATA; // Max 7680 (0x1E00) bytes


    save.curent_map = 0xFF27;
    save.pos.vx = 5161;
    save.pos.vy = 512;
    save.pos.vz = -5661;
    save.rot.vx = 0;
    save.rot.vy = 3072;
    save.rot.vz = 0;
    save.max_life = 16;
    save.max_magic = 128;
    save.life = 14;
    save.magic = 128;
    save.checksum = 0xFAEFFAEF;
    strcpy(save.name, "Kirbo");



    // Open Channel 0 (Memory Card A)

    MemCardSync(0,0,&result);
    //printf("Mem result 0x%02X\n", result);

    //printf("Opening Memory Card A...\n");
    MemCardAccept(channel);
    // Wait for results, stall
    MemCardSync(0,0,&result);
    //printf("Syncing...\n");

    if(result != McErrNone && result != McErrNewCard) {
      // Memory card error
      //printf("Memory card error: 0x%02x\n", result);
    } else {
      //printf("Creating Memory Card File...\n");
      // Create file
      result = MemCardCreateFile(channel, "BASLUS-27588ZELDAMM", 1);
      if(result != McErrNone) {
        // Error!
        //printf("Memory card error creating file: 0x%02x\n", result);
      } else {
        HEAD.Magic[0] = 'S';
        HEAD.Magic[1] = 'C';
        HEAD.Type = 0x11;
        HEAD.BlockEntry = 1;
        strcpy(HEAD.Title, "\x82\x73\x82\x88\x82\x85\x81\x40\x82\x6B\x82\x85\x82\x87\x82\x85\x82\x8E\x82\x84\x81\x40\x82\x8F\x82\x86\x81\x40\x82\x73\x82\x85\x82\x93\x82\x94\x81\x40\x81\x7C\x81\x40\x82\x65\x82\x89\x82\x8C\x82\x85\x81\x40\x30\x31");
        memcpy( HEAD.Clut, save_tim.clut, 32 );
        memcpy( HEAD.Icon[0], save_tim.pixel, 128);
        memcpy( HEAD.Icon[1], save_tim.pixel, 128);
        memcpy( HEAD.Icon[2], save_tim.pixel, 128);
        memcpy( HEAD.Data, &save, sizeof(_SAVEDATA));

        MemCardWriteFile(channel, "BASLUS-27588ZELDAMM", (long*)&HEAD, 0, sizeof(HEAD));

        MemCardSync(0,0,&result);

        if(result != McErrNone) {
          //printf("Memory card error writing: 0x%02x", result);
          return;
        }
      }
    }
  }
  
  /* Start on DEBUG module */
  G.module = MODULE_DEBUG_START;
  
  /* main game loop */
	while ( 1 ) {
    // Run Module
    ( * main_loop_table[ G.module ] )( );
    /* Start Rendering */
    //BeginDraw();
    /* End Rendering and Swap Buffers */
    //EndDraw();
    /* Update Player Input */
    InputUpdate();
  }
}