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

/* PSX memory settings */
//unsigned long _ramsize   = 0x00200000; /* force 2 megabytes of RAM */
//unsigned long _stacksize = 0x00004000; /* force 16 kilobytes of stack */
//unsigned long _stacksize = 0x00004000; /* force 8 kilobytes of stack */
//unsigned long _stacksize = 0x00001000; /* force 4 kilobytes of stack */

/*extern unsigned long __heapbase;
extern unsigned long __heapsize;
extern unsigned long __bss;
extern unsigned long __bsslen;*/


unsigned long __heap_size = 0x4000;
unsigned long __stack_size = 0x4000;

// Temporary buffer to store transformed vertices for animated models
long TransformBufferSize = 0;
SVECTOR * AGM_TransformBuffer = NULL;
CVECTOR * NormalTransformBuffer = NULL;

/* Global Data */
GS_ENV G;

u_long ot[2][OTSIZE];
PACKET gpu_packet[2][PACKET_SIZE]; // Packet buffer for GS functions

u_short show_load_text = 1;

/* Swaps Pointers according to the active buffer ID */
void ResetGsEnv()
{
	// Set References
	G.pOt = ot[G.OTag_id]; // Otag
	G.pDraw = &G.Draw_env[G.OTag_id]; // Draw Environment
	G.pDisp = &G.Disp_env[G.OTag_id]; // Display Environment
}

void BeginDraw()
{
	DRAWENV *pD;

	ResetGsEnv();
  
  pD = &G.Draw_env[G.OTag_id];
  
	ClearOTagR((u_long*)G.pOt,OTSIZE);

	if(G.clear_mode == 0)
	{
		pD->r0 = G.clear.r;
		pD->g0 = G.clear.g;
		pD->b0 = G.clear.b;
		pD->isbg = 1;
	}
	else pD->isbg = 0;
  
  G.pBuffer = (u_char *)&gpu_packet[G.OTag_id];
}

int EndDraw()
{
  int h;
  
	DrawSync(0);
	h = VSync(G.VSync_rate);

  //ResetGraph(1);

	// Display Previous frame
	PutDispEnv(G.pDisp);
	// Set Current Drawing Buffer
	PutDrawEnv(G.pDraw);

	DrawOTag((u_long*)&G.pOt[OTSIZE - 1]);
 
  G.OTag_id ^= 1;
    
  return h;
}


// Temporary Data Load Functions

/* Load Symbol Settings */
u_short ls_tp_x = 960;
u_short ls_tp_y = 240;
u_short ls_clut_x = 960;
u_short ls_clut_y = 255;
u_char ls_uv_x = 0;
u_char ls_uv_y = 240;
u_char ls_w = 16;
u_char ls_h = 8;
/* Symbol Screen Position */
u_short ls0_pos_x = 285; //457;
u_short ls0_pos_y = 207;
u_short ls2_pos_x = 280 - 3; //449;
u_short ls2_pos_y = 216;
u_short ls1_pos_x = 290 + 3; //465;
u_short ls1_pos_y = 216;
/* Symbol Base Color */
u_char ls_col_r = 255;
u_char ls_col_g = 239;
u_char ls_col_b = 13;
//u_char ls_col_r = 255 - 255;
//u_char ls_col_g = 255 - 239;
//u_char ls_col_b = 255 - 13;
/* Symbol Base Levels */
u_char ls0_lv = 64;
u_char ls1_lv = 64;
u_char ls2_lv = 64;
/* Symbol Animation */
u_short ls_anm_counter = 0;
u_char ls_anm_section = 0;
/* Anim Sections:
 0 - fade in t0
 1 - wait t0
 2 - fade out t0, in t1
 3 - wait t1
 4 - fade out t1, in t2
 5 - wait t2
 6 - fade out t2, in t1, section set to to 1 
*/

void load_screen_setup(){
	// Load "Loading Symbol" Texture to VRAM
  load_texture_pos((u_long)load_symbol, ls_tp_x, ls_tp_y, ls_clut_x, ls_clut_y);
}

void draw_load_screen(){
  u_char * packet_buffer;

  #define LS_FADE_SPEED 4
  #define LS_WAIT_TIMER 16
  #define LS_BLEND_MODE 2
  #define LS_SEMITRANS 0

  switch(ls_anm_section) {
    default:
    case 0:
      ls0_lv+=LS_FADE_SPEED;
      if(ls0_lv > 128){
        ls0_lv = 128;
        ls_anm_section = 1;
        ls_anm_counter = 0;
      }
    break;
    case 1:
      ls_anm_counter++;
      if(ls_anm_counter > LS_WAIT_TIMER){
        ls_anm_section = 2;
        ls_anm_counter = 0;
      }
    break;
    case 2:
      ls0_lv-=LS_FADE_SPEED;
      ls1_lv+=LS_FADE_SPEED;
      if(ls0_lv < 64){
        ls0_lv = 64;
        ls1_lv = 128;
        ls_anm_section = 3;
        ls_anm_counter = 0;
      }
    break;
    case 3:
      ls_anm_counter++;
      if(ls_anm_counter > LS_WAIT_TIMER){
        ls_anm_section = 4;
        ls_anm_counter = 0;
      }
    break;
    case 4:
      ls1_lv-=LS_FADE_SPEED;
      ls2_lv+=LS_FADE_SPEED;
      if(ls1_lv < 64){
        ls1_lv = 64;
        ls2_lv = 128;
        ls_anm_section = 5;
        ls_anm_counter = 0;
      }
    break;
    case 5:
      ls_anm_counter++;
      if(ls_anm_counter > LS_WAIT_TIMER){
        ls_anm_section = 6;
        ls_anm_counter = 0;
      }
    break;
    case 6:
      ls2_lv-=LS_FADE_SPEED;
      ls0_lv+=LS_FADE_SPEED;
      if(ls2_lv < 64){
        ls2_lv = 64;
        ls0_lv = 128;
        ls_anm_section = 1;
        ls_anm_counter = 0;
      }
    break;
  }
  
  BeginDraw();
  
  // Draw Loading Triangle Sprites
  packet_buffer = (u_char *)&gpu_packet[G.OTag_id];
  
  // Draw First Triangle
  setPolyFT4((POLY_FT4*)packet_buffer);
  setRGB0((POLY_FT4*)packet_buffer, (ls_col_r * ls0_lv)>>8, (ls_col_g * ls0_lv)>>8, (ls_col_b * ls0_lv)>>8);
  setXY4((POLY_FT4*)packet_buffer,
          ls0_pos_x, ls0_pos_y,
          ls0_pos_x+ls_w, ls0_pos_y,
          ls0_pos_x, ls0_pos_y+ls_h,
          ls0_pos_x+ls_w, ls0_pos_y+ls_h
        );
  setUV4((POLY_FT4*)packet_buffer,
          ls_uv_x, ls_uv_y,
          ls_uv_x+ls_w, ls_uv_y,
          ls_uv_x, ls_uv_y+ls_h,
          ls_uv_x+ls_w, ls_uv_y+ls_h
        );
  setClut((POLY_FT4*)packet_buffer, ls_clut_x, ls_clut_y);
  setTPage((POLY_FT4*)packet_buffer, 0, LS_BLEND_MODE, ls_tp_x, ls_tp_y);
  setSemiTrans((POLY_FT4*)packet_buffer, LS_SEMITRANS );
  addPrim((u_long*)G.pOt, packet_buffer);
  packet_buffer += sizeof(POLY_FT4);
  
  // Draw Second Triangle
  setPolyFT4((POLY_FT4*)packet_buffer);
  setRGB0((POLY_FT4*)packet_buffer, (ls_col_r * ls1_lv)>>8, (ls_col_g * ls1_lv)>>8, (ls_col_b * ls1_lv)>>8);
  setXY4((POLY_FT4*)packet_buffer,
          ls1_pos_x, ls1_pos_y,
          ls1_pos_x+ls_w, ls1_pos_y,
          ls1_pos_x, ls1_pos_y+ls_h,
          ls1_pos_x+ls_w, ls1_pos_y+ls_h
        );
  setUV4((POLY_FT4*)packet_buffer,
          ls_uv_x, ls_uv_y,
          ls_uv_x+ls_w, ls_uv_y,
          ls_uv_x, ls_uv_y+ls_h,
          ls_uv_x+ls_w, ls_uv_y+ls_h
        );
  setClut((POLY_FT4*)packet_buffer, ls_clut_x, ls_clut_y);
  setTPage((POLY_FT4*)packet_buffer, 0, LS_BLEND_MODE, ls_tp_x, ls_tp_y);
  setSemiTrans((POLY_FT4*)packet_buffer, LS_SEMITRANS );
  addPrim((u_long*)G.pOt, packet_buffer);
  packet_buffer += sizeof(POLY_FT4);
  
  // Draw Third Triangle
  setPolyFT4((POLY_FT4*)packet_buffer);
  setRGB0((POLY_FT4*)packet_buffer, (ls_col_r * ls2_lv)>>8, (ls_col_g * ls2_lv)>>8, (ls_col_b * ls2_lv)>>8);
  setXY4((POLY_FT4*)packet_buffer,
          ls2_pos_x, ls2_pos_y,
          ls2_pos_x+ls_w, ls2_pos_y,
          ls2_pos_x, ls2_pos_y+ls_h,
          ls2_pos_x+ls_w, ls2_pos_y+ls_h
        );
  setUV4((POLY_FT4*)packet_buffer,
          ls_uv_x, ls_uv_y,
          ls_uv_x+ls_w, ls_uv_y,
          ls_uv_x, ls_uv_y+ls_h,
          ls_uv_x+ls_w, ls_uv_y+ls_h
        );
  setClut((POLY_FT4*)packet_buffer, ls_clut_x, ls_clut_y);
  setTPage((POLY_FT4*)packet_buffer, 0, LS_BLEND_MODE, ls_tp_x, ls_tp_y);
  setSemiTrans((POLY_FT4*)packet_buffer, LS_SEMITRANS );
  addPrim((u_long*)G.pOt, packet_buffer);
  packet_buffer += sizeof(POLY_FT4);
  
  
  EndDraw();
}

void file_load_temp(char * filename, unsigned long ** dest) {
  DslFILE		fp;
  int nsector, cnt;
  unsigned long * prev;
  //unsigned long * next;
  if((long)DsSearchFile(&fp, filename) <= 0) {
    //printf("File not found: %s\n", filename);
    return;
  }
  //printf("Loading %s\n",filename);
  nsector = (fp.size+2047)/2048;
  *dest = LStack_Alloc(nsector * 2048);
  DsControl(DslSetloc, (u_char *)&fp.pos, 0);
  DsRead(&fp.pos, nsector, *dest, DslModeSpeed);

  while ((cnt = DsReadSync(0)) > 0 ) {
    if(g_pad_press & PAD_SELECT) show_load_text = !show_load_text;
    draw_load_screen();
    if(show_load_text == 1) {
      FntPrint("Loading %s (Sectors: %d)\n",filename,cnt);
      FntFlush(-1);
    }
    InputUpdate();
  }
  prev = *dest;
  // TEMP: After finishing, attempt to realloc
  
  //*dest = realloc3(*dest, fp.size);
  //next = *dest;
  //printf("File %s realloc: %x => %x\n", filename, prev, *dest);
  ////printf("Realloc size: %d => %d\n", nsector * 2048, fp.size);
}

unsigned long file_load_temp_noalloc(char * filename, unsigned long * dest) {
  DslFILE		fp;
  int nsector, cnt;
  if((long)DsSearchFile(&fp, filename) <= 0) {
    //printf("File not found: %s\n", filename);
    return 0;
  }
  //printf("Loading %s\n",filename);
  nsector = (fp.size+2047)/2048;
  DsControl(DslSetloc, (u_char *)&fp.pos, 0);
  DsRead(&fp.pos, nsector, dest, DslModeSpeed);

  while ((cnt = DsReadSync(0)) > 0 ) {
    if(g_pad_press & PAD_SELECT) show_load_text = !show_load_text;
    draw_load_screen();
    if(show_load_text != 0) {
      FntPrint("Loading %s (Sectors: %d)\n",filename,cnt);
      FntFlush(-1);
    }
    InputUpdate();
  }
  return fp.size;
}