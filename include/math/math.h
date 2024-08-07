/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#ifndef _MATH_H_
#define _MATH_H_

#include "global.h"

#define CHAR_BIT 8

// Divide/Multiply by 4096 with shift logic
#define DIV_BY_ONE >> 12
#define MUL_BY_ONE << 12

#define FOUR_DIV_PI					0x145F3		/*!< Fix16 value of 4/PI */
#define _FOUR_DIV_PI2			 0xFFFF9840		/*!< Fix16 value of -4/PI? */
#define X4_CORRECTION_COMPONENT		 0x399A		/*!< Fix16 value of 0.225 */

#define PI_DIV_4				 0x0000C90F		/*!< Fix16 value of PI/4 */
#define THREE_PI_DIV_4			 0x00025B2F		/*!< Fix16 value of 3PI/4 */

#define PI_DIV_4_F12				 0x00000C90		/*!< Fix16 value of PI/4 */
#define THREE_PI_DIV_4_F12	 0x000025B2		/*!< Fix16 value of 3PI/4 */

#define fix16_max		0x7FFFFFFF	/*!< the maximum value of fix16_t */
#define fix16_min		0x80000000	/*!< the minimum value of fix16_t */
#define fix16_overflow	0x80000000	/*!< the value used to indicate overflows when FIXMATH_NO_OVERFLOW is not specified */

#define fix16_pi  205887     /*!< fix16_t value of pi */
#define fix16_e   178145     /*!< fix16_t value of e */
#define fix16_one 0x00010000 /*!< fix16_t value of 1 */

#define fix12_2pi 25736
#define fix12_pi  12867     /*!< fix16_t value of pi */
#define fix12_e   11134     /*!< fix16_t value of e */
#define fix12_one 0x00001000 /*!< fix16_t value of 1 */

typedef long long int64;
typedef unsigned long long u_int64;

typedef struct QUATERNION {
  short vx;
  short vy;
  short vz;
  short vw;
} QUATERNION;

/* From https://gist.github.com/shaunlebron/8832585 */
extern short shortAngleDist(short a0, short a1);

short dotProductXYZ(SVECTOR * a, SVECTOR * b);
/* 2D Dot Product of a SVECTOR, calculates on XZ plane */
//extern short dotProductXZ(SVECTOR * a, SVECTOR * b);
extern short dotProductXZ(VECTOR * a, SVECTOR * b);

extern long dotProductXZex(VECTOR * a, SVECTOR * b);
extern long dotProductXZex2(VECTOR * a, SVECTOR * b);

extern long dotProductXYex2(VECTOR * a, SVECTOR * b);

extern short dotProduct2D(short ax, short ay, short bx, short by);
extern long dotProduct2DR(short ax, short ay, short bx, short by);

void crossProduct(SVECTOR * v0, SVECTOR * v1, VECTOR * out);
void crossProductS(SVECTOR * v0, SVECTOR * v1, SVECTOR * out);

/* Multiplies the two given fix16_t's and returns the result. */
extern int fix16_mul(int inArg0, int inArg1);

/* Divides the first given fix16_t by the second and returns the result. */
extern int fix16_div(int inArg0, int inArg1);

extern int fix12_div(int a, int b);

/* Returns the arctangent of inY/inX. */
extern int fix16_atan2(int inY, int inX);

extern int fix12_atan2(int inY , int inX);

extern int fpmath_atan2(int inY, int inX);

extern int fpmathTest_atan2(int inY, int inX);

extern unsigned short fxpt_atan2(const short y, const short x);

extern int fix12_atan2s(int inY , int inX);

extern int vec_mag(int x, int y);

extern int fix12_mul(int inArg0, int inArg1);

extern int fix12_smul(int inArg0, int inArg1);

extern int fix12_lerp(int a, int b, int t);

extern int fix12_gtelerp(int a, int b, int t);

extern void fix12_gtesvlerp(SVECTOR * dest, SVECTOR * a, SVECTOR * b, int t);

// Quaternion Math
void fix12_toQuaternion(QUATERNION * quat, SVECTOR * ang);
void fix12_toEuler(SVECTOR * ang, QUATERNION * quat);

void fix12_normalizeQuaternion(QUATERNION * dest, QUATERNION * src);

void fix12_quaternionMul(QUATERNION * dest, QUATERNION * a, QUATERNION * b) ;

void fix12_quaternionFromAxisAngle(QUATERNION * dest, QUATERNION * rot);

//void quaternionSlerp(QUATERNION *src1, QUATERNION *sp2, u_long t, QUATERNION *dquat);

void quaternionGetMatrix(QUATERNION *squat, MATRIX *dmatrix);

void fix12_quaternionSlerp(QUATERNION * dest, QUATERNION * a, QUATERNION * b, u_long t);

// 16.16 FP Square Root
unsigned long fast_sqrt(unsigned long num);

long get_distanceXZ(SVECTOR * dist);

long get_distanceXZsq(SVECTOR * dist);

long get_distanceXYZ(SVECTOR * dist);

// Vector to Angle
short get_angleFromVectorXZ(SVECTOR * pos);

#endif