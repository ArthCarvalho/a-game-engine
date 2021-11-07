#include "screen/screen.h"


//short life_max = 0x0140;
//short life_current = 0x0140;
short life_max = 0x0030;
short life_current = 0x0030;

u_char screen_transparent = 0;
u_short clut[2] = {getClut(LIFEMETER_CLUT_X, LIFEMETER_CLUT_Y), getClut(LIFEMETER_CLUT_X, LIFEMETER_CLUT_Y+1)};
u_long heart_color[2] = { RGBPACK32(128,128,128), RGBPACK32(107/2,173/2,255/2) };

void Screen_LifeMeterInit() {

}

char * Screen_LifeMeterDraw(char * buff) {
  short max_hearts = life_max >> 4;
  short pos_x = LIFEMETER_POS_X;
  short pos_y = LIFEMETER_POS_Y;

  if(g_pad & PAD_SELECT) life_current-=5;
  if(g_pad & PAD_START) life_current+=5;


  for(int i = 0; i < max_hearts; i++) {
    u_char life_percent = life_current & 0x0F;
    short in = i << 4;
    if(i == LIFEMETER_HEARTS_LINE) {
      pos_x = LIFEMETER_POS_X;
      pos_y += 10;
    }
    // Percent:
    // 11~15  - 3/4
    // 6~10   - 2/4
    // 1~5    - 1/4
    // 0      - full
    buff = Screen_RenderHeart(buff, pos_x, pos_y, (in < life_current) + ((in >= (life_current-0x10)) && in < life_current),life_percent);
    pos_x += 16;
  }
  pos_x = LIFEMETER_POS_X;
  pos_y += 11;

  buff = Screen_MagicBarDraw(buff, pos_x, pos_y, 86, 4096);

  //setDrawTPage((DR_TPAGE*)buff, 1, 0, getTPage(0, 0, LIFEMETER_TEX_X, LIFEMETER_TEX_Y));
  //addPrim(G.pOt, buff);
  //buff += sizeof(DR_TPAGE);

  return buff;
}

char * Screen_RenderHeart(char * buff, short x, short y, char state, u_char pct) {
  SPRT * piece = (SPRT*)buff;
  u_short cl = clut[screen_transparent];
  if(state == 2){
    if(pct == 0) { // Full, Active
      setXY0(piece, x, y-1);
      setWH(piece, 16, 10);
      setUV0(piece, LIFEMETER_VRAM_X, LIFEMETER_VRAM_A_Y);
      *(long*)(&piece->r0) = heart_color[0];
      piece->clut = cl;
      setSprt(piece);
      setSemiTrans(piece, 1);
      addPrim(G.pOt, piece);
      piece++;
    } else { // Active
      // Draw 1st piece
      y--;
      setXY0(piece, x, y);
      setWH(piece, 8, 5);
      setUV0(piece, LIFEMETER_VRAM_X, LIFEMETER_VRAM_A_Y);
      *(long*)(&piece->r0) = heart_color[0];
      piece->clut = cl;
      setSprt(piece);
      setSemiTrans(piece, 1);
      addPrim(G.pOt, piece);
      piece++;

      // Draw 2nd piece
      setXY0(piece, x, y+5);
      setWH(piece, 8, 5);
      setUV0(piece, LIFEMETER_VRAM_X, LIFEMETER_VRAM_A_Y+5);
      *(long*)(&piece->r0) = heart_color[pct < 6];
      piece->clut = cl;
      setSprt(piece);
      setSemiTrans(piece, 1);
      addPrim(G.pOt, piece);
      piece++;

      // Draw 3rd piece
      setXY0(piece, x+8, y+5);
      setWH(piece, 8, 5);
      setUV0(piece, LIFEMETER_VRAM_X+8, LIFEMETER_VRAM_A_Y+5);
      *(long*)(&piece->r0) = heart_color[pct < 11];
      piece->clut = cl;
      setSprt(piece);
      setSemiTrans(piece, 1);
      addPrim(G.pOt, piece);
      piece++;

      // Draw 4th piece
      setXY0(piece, x+8, y);
      setWH(piece, 8, 5);
      setUV0(piece, LIFEMETER_VRAM_X+8, LIFEMETER_VRAM_A_Y);
      *(long*)(&piece->r0) = heart_color[1];
      piece->clut = cl;
      setSprt(piece);
      setSemiTrans(piece, 1);
      addPrim(G.pOt, piece);
      piece++;
    }
  } else{
    setXY0(piece, x, y);
    setWH(piece, 16, 8);
    setUV0(piece, LIFEMETER_VRAM_X, LIFEMETER_VRAM_Y);
    *(long*)(&piece->r0) = heart_color[state == 0];
    piece->clut = cl;
    setSprt(piece);
    setSemiTrans(piece, 1);
    addPrim(G.pOt, piece);
    piece++;
  }

  return (char *)piece;
}

char * Screen_MagicBarDraw(char * buff, short x, short y, short w, short magic) {
  SPRT * sprite = (SPRT*) buff;
  u_short cl = clut[screen_transparent];

  short barw = ((w-10) * magic) >> 12;

  // Bar Left
  setXY0(sprite, x, y);
  setWH(sprite, 5, 11);
  setUV0(sprite, LIFEMETER_VRAM_X, LIFEMETER_VRAM_Y+9);
  setRGB0(sprite, 128, 128, 128);
  sprite->clut = cl;
  setSprt(sprite);
  setSemiTrans(sprite, 1);
  addPrim(G.pOt, sprite);
  sprite++;
  // Bar Right
  setXY0(sprite, x + w - 5, y);
  setWH(sprite, 5, 11);
  setUV0(sprite, LIFEMETER_VRAM_X+16-5, LIFEMETER_VRAM_Y+9);
  setRGB0(sprite, 128, 128, 128);
  sprite->clut = cl;
  setSprt(sprite);
  setSemiTrans(sprite, 1);
  addPrim(G.pOt, sprite);
  sprite++;
  POLY_FT4 * bar = (POLY_FT4*)sprite;
  // Magic bar
  setXYWH(bar, x+5, y+2, barw, 7);
  setUVWH(bar, LIFEMETER_VRAM_X+5, LIFEMETER_VRAM_Y+24, 5, 7);
  setPolyFT4(bar);
  bar->tpage = getTPage(0, 0, LIFEMETER_TEX_X, LIFEMETER_TEX_Y);
  bar->clut = cl;
  setRGB0(bar, 128, 128, 128);
  setSemiTrans(bar, 0);
  addPrim(G.pOt, bar);
  bar++;
  // Back
  POLY_FT4 * barcpy = bar;
  setXYWH(bar, x+5, y, w-10, 11);
  setUVWH(bar, LIFEMETER_VRAM_X+5, LIFEMETER_VRAM_Y+9, 5, 11);
  setPolyFT4(bar);
  bar->tpage = getTPage(0, 0, LIFEMETER_TEX_X, LIFEMETER_TEX_Y);
  bar->clut = cl;
  setRGB0(bar, 128, 128, 128);
  setSemiTrans(bar, 1);
  addPrim(G.pOt, bar);
  bar++;
  *bar = *barcpy;
  addPrim(G.pOt, bar);
  bar++;
  return (char*)bar;
}