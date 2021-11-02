/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#ifndef _HISIN_H_
#define _HISIN_H_
// From PSn00bSDK
/* Based on isin_S4 implementation from coranac:
 *	http://www.coranac.com/2009/07/sines/
 *
 */
 
#define qN	15
#define qA	12
#define B	19900
#define	C	3516

extern int hisin(int x);
extern int hicos(int x);

#endif