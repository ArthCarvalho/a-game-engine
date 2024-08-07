/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#include "texture/texture.h"
/*
void generate_clut(u_short * input, u_long p, u_short * output) {
  int i;
  CVECTOR orgc, newc;
  u_short stp;
  
  for(i = 0; i < ccnt; i++){
    if(input[i]==0){
      output[i] = input[i];
    } else {
      // Decompose clut into RGB
      orgc.r = (input[i] & 0x1F) << 3;
      orgc.g = ((input[i] >> 5) & 0x1F) << 3;
      orgc.b = ((input[i] >> 10) & 0x1F) << 3;
      // Save semi-transparent bit
      stp = input[i] & 0x8000;
      // Interpolate FarColor and original color
      gte_DpqColor(&orgc, p, &newc);
      
      // Reconstruct color
      output[i] = stp | (newc.r >> 3) |
      (((unsigned long)(newc.g&0xf8))<<2) |
      (((unsigned long)(newc.b&0xf8))<<7);
    }
  }
}
*/
void make_clut(RECT *clutrect, u_short *clut, long num) {
  long i,j;
  CVECTOR orgc, newc;
  RECT rect;
  long stp;
  long p;
  u_short newclut[256];
  
  rect.x=clutrect->x;
  rect.w=clutrect->w;
  rect.h=clutrect->h;
    
  for (i=0; i<num; i++) {
    p=i*4096/num;
    for (j = 0; j < clutrect->w; j++) {
      if (clut[j]==0){
        newclut[j]=clut[j];
	    } else{
        orgc.r= (clut[j] & 0x1f)<<3;
        orgc.g= ((clut[j] >>5) & 0x1f)<<3;
        orgc.b= ((clut[j] >>10) & 0x1f)<<3;
        stp= clut[j] & 0x8000;
        
        DpqColor(&orgc, p, &newc);
        
        newclut[j]= stp | (newc.r>>3) | (((unsigned long)(newc.g&0xf8))<<2) | (((unsigned long)(newc.b&0xf8))<<7);
      }
    }
    rect.y=clutrect->y+i;
    LoadImage(&rect,(u_long *)newclut);
  }
}

void load_texture(u_long addr) {
  RECT	rect;
  TIMIMAGE	tim;

  GetTimInfo((u_long *)addr,&tim);
  rect.x = tim.px;
  rect.y = tim.py;
  rect.w = tim.pw;
  rect.h = tim.ph;
  LoadImage(&rect,tim.pixel);

	if((tim.pmode>>3)&0x01) {
		rect.x = tim.cx;
		rect.y = tim.cy;

		rect.w = tim.cw;
		rect.h = tim.ch;

		LoadImage(&rect,tim.clut);
   }
}
// Loads textures and clut with arbitrary position in vram
void load_texture_pos(u_long addr, u_long x, u_long y, u_long c_x, u_long c_y) {
  RECT	rect;
  TIMIMAGE	tim;

  GetTimInfo((u_long *)addr,&tim);
  rect.x = x;
  rect.y = y;
  rect.w = tim.pw;
  rect.h = tim.ph;
  LoadImage(&rect,tim.pixel);

  if((tim.pmode>>3)&0x01) {
    rect.x = c_x;
    rect.y = c_y;

    rect.w = tim.cw;
    rect.h = tim.ch;

    LoadImage(&rect,tim.clut);
  }
}

// Loads textures and clut with arbitrary position in vram, plus generate clut fog
void load_texture_pos_fog(u_long addr, u_long x, u_long y, u_long c_x, u_long c_y, u_short fog) {
  RECT	rect;
  TIMIMAGE	tim;

  GetTimInfo((u_long *)addr,&tim);
  rect.x = x;
  rect.y = y;
  rect.w = tim.pw;
  rect.h = tim.ph;
  LoadImage(&rect,tim.pixel);

  if((tim.pmode>>3)&0x01) {
    rect.x = c_x;
    rect.y = c_y;

    rect.w = tim.cw;
    rect.h = tim.ch;

    ////printf('clut address: %x\n', &tim.clut);

    make_clut(&rect, (u_short*)tim.clut, fog);

    //LoadImage(&rect,tim.clut);
  }
}

// Load textures without CLUT
void load_tex_noclut_pos(u_long addr, u_long x, u_long y, u_long c_x, u_long c_y) {
  RECT	rect;
  TIMIMAGE	tim;

  GetTimInfo((u_long *)addr,&tim);
  rect.x = x;
  rect.y = y;
  rect.w = tim.pw;
  rect.h = tim.ph;
  LoadImage(&rect,tim.pixel);
}

void load_clut_pos(u_long addr, u_long c_x, u_long c_y, u_char c_w, u_char c_h) {
  RECT rect;
  rect.x = c_x;
  rect.y = c_y;
  rect.w = c_w;
  rect.h = c_h;
  LoadImage(&rect, (u_long*)addr);
}

void GetTimInfo(unsigned long *im, TIMIMAGE * tim) {
  u_long * imptr;
  u_short * pptr;
  tim->pmode = im[1];
  imptr = im+2;
  
  // CLUT Image
  if(tim->pmode & 0x08) {
    pptr = (u_short *)(imptr+1);
    tim->cx = pptr[0];
    tim->cy = pptr[1];
    tim->cw = pptr[2];
    tim->ch = pptr[3];
    tim->clut  = (imptr+3);
    imptr += imptr[0]>>2;
  } else {
    tim->clut = 0;
  }
  
  pptr = (u_short *)(imptr+1);
  tim->px = pptr[0];
  tim->py = pptr[1];
  tim->pw = pptr[2];
  tim->ph = pptr[3];
  tim->pixel = (imptr+3);
}