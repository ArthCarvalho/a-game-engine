/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#include "input/input.h"

#include <libpad.h>

static unsigned char padbuff[2][34];
static unsigned char align[6]= {
    0, 1, 0xFF, 0xFF, 0xFF, 0xFF
};
static struct {
	unsigned char Button;
	unsigned char Lock;
	unsigned char Motor0,Motor1;
	unsigned char Send;
} hist;

unsigned char* padi;
u_long padd;
int state;
int ids;

u_long g_pad;
u_long g_pad_press;
u_long g_pad_hold;
u_char pad_analog[4];

/* Connected Controller Type */
/*static char* padstr[] = {
	"NO CONTROLLER", "MOUSE", "NEGI-CON", "KONAMI-GUN", "STANDARD CONTROLLER",
	"ANALOG STICK",	"NAMCO-GUN", "ANALOG CONTROLLER"
};*/

void InputInit()
{  
	PadInitDirect(padbuff[0],padbuff[1]);
	PadStartCom();
  g_pad = 0;
  g_pad_press = 0;
  g_pad_hold = 0;
  pad_analog[ANALOG_L_X] = 0x80;
  pad_analog[ANALOG_L_Y] = 0x80;
  pad_analog[ANALOG_R_X] = 0x80;
  pad_analog[ANALOG_R_Y] = 0x80;
}

void InputUpdate(void)
{
	padi = padbuff[0];
  padd = ~((padi[2]<<8) | (padi[3]));
  
  pad_analog[ANALOG_L_X] = 0x80;
  pad_analog[ANALOG_L_Y] = 0x80;
  pad_analog[ANALOG_R_X] = 0x80;
  pad_analog[ANALOG_R_Y] = 0x80;
  
  state = PadGetState(0x00);
  if(state == PadStateDiscon) {
    //FntPrint("NO CONTROLLER\n" );
    padi = 0;
    padd = 0;
  } else {
    switch((ids=PadInfoMode(0x00,InfoModeCurID,0))) {
      /* Digital Controller */
      case 4:
      //FntPrint("STATE: %08x\n",padd);
      break;
      /* Analog Controller */
      case 7:
      if (PadInfoMode(0x00,InfoModeCurExID,0)) {

        /*// DUAL SHOCK //*/
        //FntPrint("%02x %02x %02x %02x\n", padi[4], padi[5], padi[6], padi[7] );
        //FntPrint("STATE: %08x\n",padd);
        pad_analog[ANALOG_L_X] = padi[6];
        pad_analog[ANALOG_L_Y] = padi[7];
        pad_analog[ANALOG_R_X] = padi[4];
        pad_analog[ANALOG_R_Y] = padi[5];
      }
      break;
      default:
      break;
    }
  }
  g_pad_press = padd & (~g_pad);
  g_pad_hold = padd & g_pad;
  g_pad = padd;
}

/* PAD END */
