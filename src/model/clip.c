/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

// From PSn00bSDK
/* Polygon clip detection code
 *
 * The polygon clipping logic is based on the Cohen-Sutherland algorithm, but
 * only the off-screen detection logic is used to determine which polygon edges
 * are off-screen.
 *
 * In tri_clip, the following edges are checked as follows:
 *
 *  |\
 *  |  \
 *  |    \
 *  |      \
 *  |--------
 *
 * In quad_clip, the following edges are checked as follows:
 *
 *  |---------|
 *  | \     / |
 *  |   \ /   |
 *  |   / \   |
 *  | /     \ |
 *  |---------|
 *
 * The inner portion of the quad is checked, otherwise the quad will be
 * culled out if the camera faces right into it, where all four edges
 * are off-screen at once.
 *
 */
 
#include "model/clip.h"

#define clip_LEFT	1
#define clip_RIGHT	2
#define clip_TOP	4
#define clip_BOTTOM	8

RECT local_clip;

unsigned int test_clip(short x, short y) {
	
	// Tests which corners of the screen a point lies outside of
	
	unsigned int result = 0;

	if ( x < local_clip.x ) {
		result |= clip_LEFT;
	}
	
	if ( x >= (local_clip.x+(local_clip.w-1)) ) {
		result |= clip_RIGHT;
	}
	
	if ( y < local_clip.y ) {
		result |= clip_TOP;
	}
	
	if ( y >= (local_clip.y+(local_clip.h-1)) ) {
		result |= clip_BOTTOM;
	}

	return result;
	
}

#define clip_W (SCREEN_W - 1)
#define clip_H (SCREEN_H - 1)

inline unsigned int test_clip_fixed(short x, short y) {
	unsigned int result = 0;
	if ( x < 0 ) {
		result |= clip_LEFT;
	}
	
	if ( x >= clip_W ) {
		result |= clip_RIGHT;
	}
	
	if ( y < 0 ) {
		result |= clip_TOP;
	}
	
	if ( y >= clip_H ) {
		result |= clip_BOTTOM;
	}
	return result;
}

inline unsigned int tri_clip(DVECTOR *v0, DVECTOR *v1, DVECTOR *v2) {
	
	// Returns non-zero if a triangle is outside the screen boundaries
	unsigned int c[3];

	c[0] = test_clip_fixed(v0->vx, v0->vy);
	c[1] = test_clip_fixed(v1->vx, v1->vy);
	c[2] = test_clip_fixed(v2->vx, v2->vy);

	if ( ( c[0] & c[1] ) == 0 )
		return 0;
	if ( ( c[1] & c[2] ) == 0 )
		return 0;
	if ( ( c[2] & c[0] ) == 0 )
		return 0;

	return 1;
}



inline unsigned int quad_clip(DVECTOR *v0, DVECTOR *v1, DVECTOR *v2, DVECTOR *v3) {
	
	// Returns non-zero if a quad is outside the screen boundaries
	
	unsigned int c[4];

	c[0] = test_clip_fixed(v0->vx, v0->vy);
	c[1] = test_clip_fixed(v1->vx, v1->vy);
	c[2] = test_clip_fixed(v2->vx, v2->vy);
	c[3] = test_clip_fixed(v3->vx, v3->vy);

	if ( ( c[0] & c[1] ) == 0 )
		return 0;
	if ( ( c[1] & c[2] ) == 0 )
		return 0;
	if ( ( c[2] & c[3] ) == 0 )
		return 0;
	if ( ( c[3] & c[0] ) == 0 )
		return 0;
	if ( ( c[0] & c[2] ) == 0 )
		return 0;
	if ( ( c[1] & c[3] ) == 0 )
		return 0;

	return 1;
}

int test_clip_s(RECT32 * clip, long x, long y) {
	
	// Tests which corners of the screen a point lies outside of
	
	unsigned int result = 0;

	if ( x < clip->x ) {
		result |= clip_LEFT;
	}
	
	if ( x >= (clip->x+(clip->w-1)) ) {
		result |= clip_RIGHT;
	}
	
	if ( y < clip->y ) {
		result |= clip_TOP;
	}
	
	if ( y >= (clip->y+(clip->h-1)) ) {
		result |= clip_BOTTOM;
	}

	return result;
	
}

int tri_clip_s(RECT32 * clip, VECTOR2D *v0, VECTOR2D *v1, VECTOR2D *v2) {
	// Returns non-zero if a triangle is outside the screen boundaries
	unsigned int c[3];

	c[0] = test_clip_s(clip, v0->vx, v0->vy);
	c[1] = test_clip_s(clip, v1->vx, v1->vy);
	c[2] = test_clip_s(clip, v2->vx, v2->vy);

	if ( ( c[0] & c[1] ) == 0 )
		return 0;
	if ( ( c[1] & c[2] ) == 0 )
		return 0;
	if ( ( c[2] & c[0] ) == 0 )
		return 0;

	return 1;
}
