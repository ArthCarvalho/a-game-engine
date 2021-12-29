/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#include "math/math.h"

/*
 * fxpt_atan2.c
 *
 * Copyright (C) 2012, Xo Wang
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */
/**
 * Convert floating point to Q15 (1.0.15 fixed point) format.
 *
 * @param d floating-point value within range -1 to (1 - (2**-15)), inclusive
 * @return Q15 value representing d; same range
 */
 
 #define M_1_PI 0.31830988618379067154 
 // FULL PI
 #define FPI 3.1415926535897932384626433832795
 // HALF PI
 #define HPI 1.5707963267948966192313216916398
 // PI FIXED
#define FIX12_PI 12868
#define FIX12_HPI 6434
#define FIX12_EPSILON 4096

 #define STATIC0 28103
 #define STATIC1 53839
 
 #define FIXMATH_NO_ROUNDING
#define FIXMATH_NO_OVERFLOW

unsigned short acostable[] = {1024, 
1023, 1022, 1022, 1021, 1020, 1020, 1019, 1019, 1018, 1017, 1017, 1016, 1015, 1015, 1014, 1013, 
1013, 1012, 1012, 1011, 1010, 1010, 1009, 1008, 1008, 1007, 1006, 1006, 1005, 1005, 1004, 1003, 
1003, 1002, 1001, 1001, 1000, 999, 999, 998, 998, 997, 996, 996, 995, 994, 994, 993, 
992, 992, 991, 991, 990, 989, 989, 988, 987, 987, 986, 985, 985, 984, 984, 983, 
982, 982, 981, 980, 980, 979, 978, 978, 977, 976, 976, 975, 975, 974, 973, 973, 
972, 971, 971, 970, 969, 969, 968, 968, 967, 966, 966, 965, 964, 964, 963, 962, 
962, 961, 960, 960, 959, 959, 958, 957, 957, 956, 955, 955, 954, 953, 953, 952, 
952, 951, 950, 950, 949, 948, 948, 947, 946, 946, 945, 944, 944, 943, 943, 942, 
941, 941, 940, 939, 939, 938, 937, 937, 936, 935, 935, 934, 934, 933, 932, 932, 
931, 930, 930, 929, 928, 928, 927, 926, 926, 925, 925, 924, 923, 923, 922, 921, 
921, 920, 919, 919, 918, 917, 917, 916, 916, 915, 914, 914, 913, 912, 912, 911, 
910, 910, 909, 908, 908, 907, 906, 906, 905, 905, 904, 903, 903, 902, 901, 901, 
900, 899, 899, 898, 897, 897, 896, 895, 895, 894, 893, 893, 892, 892, 891, 890, 
890, 889, 888, 888, 887, 886, 886, 885, 884, 884, 883, 882, 882, 881, 880, 880, 
879, 879, 878, 877, 877, 876, 875, 875, 874, 873, 873, 872, 871, 871, 870, 869, 
869, 868, 867, 867, 866, 865, 865, 864, 863, 863, 862, 861, 861, 860, 859, 859, 
858, 858, 857, 856, 856, 855, 854, 854, 853, 852, 852, 851, 850, 850, 849, 848, 
848, 847, 846, 846, 845, 844, 844, 843, 842, 842, 841, 840, 840, 839, 838, 838, 
837, 836, 836, 835, 834, 834, 833, 832, 832, 831, 830, 830, 829, 828, 828, 827, 
826, 826, 825, 824, 824, 823, 822, 822, 821, 820, 820, 819, 818, 818, 817, 816, 
816, 815, 814, 814, 813, 812, 812, 811, 810, 810, 809, 808, 808, 807, 806, 806, 
805, 804, 804, 803, 802, 802, 801, 800, 800, 799, 798, 797, 797, 796, 795, 795, 
794, 793, 793, 792, 791, 791, 790, 789, 789, 788, 787, 787, 786, 785, 785, 784, 
783, 783, 782, 781, 780, 780, 779, 778, 778, 777, 776, 776, 775, 774, 774, 773, 
772, 772, 771, 770, 769, 769, 768, 767, 767, 766, 765, 765, 764, 763, 763, 762, 
761, 760, 760, 759, 758, 758, 757, 756, 756, 755, 754, 754, 753, 752, 751, 751, 
750, 749, 749, 748, 747, 747, 746, 745, 744, 744, 743, 742, 742, 741, 740, 740, 
739, 738, 737, 737, 736, 735, 735, 734, 733, 733, 732, 731, 730, 730, 729, 728, 
728, 727, 726, 725, 725, 724, 723, 723, 722, 721, 720, 720, 719, 718, 718, 717, 
716, 715, 715, 714, 713, 713, 712, 711, 710, 710, 709, 708, 708, 707, 706, 705, 
705, 704, 703, 703, 702, 701, 700, 700, 699, 698, 697, 697, 696, 695, 695, 694, 
693, 692, 692, 691, 690, 689, 689, 688, 687, 687, 686, 685, 684, 684, 683, 682, 
681, 681, 680, 679, 678, 678, 677, 676, 675, 675, 674, 673, 672, 672, 671, 670, 
670, 669, 668, 667, 667, 666, 665, 664, 664, 663, 662, 661, 661, 660, 659, 658, 
658, 657, 656, 655, 655, 654, 653, 652, 652, 651, 650, 649, 648, 648, 647, 646, 
645, 645, 644, 643, 642, 642, 641, 640, 639, 639, 638, 637, 636, 635, 635, 634, 
633, 632, 632, 631, 630, 629, 629, 628, 627, 626, 625, 625, 624, 623, 622, 622, 
621, 620, 619, 618, 618, 617, 616, 615, 614, 614, 613, 612, 611, 611, 610, 609, 
608, 607, 607, 606, 605, 604, 603, 603, 602, 601, 600, 599, 599, 598, 597, 596, 
595, 595, 594, 593, 592, 591, 591, 590, 589, 588, 587, 586, 586, 585, 584, 583, 
582, 582, 581, 580, 579, 578, 578, 577, 576, 575, 574, 573, 573, 572, 571, 570, 
569, 568, 568, 567, 566, 565, 564, 563, 563, 562, 561, 560, 559, 558, 558, 557, 
556, 555, 554, 553, 552, 552, 551, 550, 549, 548, 547, 546, 546, 545, 544, 543, 
542, 541, 540, 540, 539, 538, 537, 536, 535, 534, 534, 533, 532, 531, 530, 529, 
528, 527, 527, 526, 525, 524, 523, 522, 521, 520, 519, 519, 518, 517, 516, 515, 
514, 513, 512, 511, 510, 510, 509, 508, 507, 506, 505, 504, 503, 502, 501, 500, 
500, 499, 498, 497, 496, 495, 494, 493, 492, 491, 490, 489, 488, 487, 487, 486, 
485, 484, 483, 482, 481, 480, 479, 478, 477, 476, 475, 474, 473, 472, 471, 470, 
469, 468, 467, 467, 466, 465, 464, 463, 462, 461, 460, 459, 458, 457, 456, 455, 
454, 453, 452, 451, 450, 449, 448, 447, 446, 445, 444, 443, 442, 441, 440, 439, 
438, 437, 436, 435, 434, 433, 431, 430, 429, 428, 427, 426, 425, 424, 423, 422, 
421, 420, 419, 418, 417, 416, 415, 414, 412, 411, 410, 409, 408, 407, 406, 405, 
404, 403, 402, 400, 399, 398, 397, 396, 395, 394, 393, 392, 390, 389, 388, 387, 
386, 385, 384, 382, 381, 380, 379, 378, 377, 376, 374, 373, 372, 371, 370, 368, 
367, 366, 365, 364, 362, 361, 360, 359, 358, 356, 355, 354, 353, 351, 350, 349, 
348, 346, 345, 344, 343, 341, 340, 339, 338, 336, 335, 334, 332, 331, 330, 328, 
327, 326, 324, 323, 322, 320, 319, 318, 316, 315, 314, 312, 311, 310, 308, 307, 
305, 304, 302, 301, 300, 298, 297, 295, 294, 292, 291, 289, 288, 286, 285, 283, 
282, 280, 279, 277, 276, 274, 273, 271, 270, 268, 266, 265, 263, 262, 260, 258, 
257, 255, 253, 252, 250, 248, 246, 245, 243, 241, 239, 238, 236, 234, 232, 230, 
229, 227, 225, 223, 221, 219, 217, 215, 213, 211, 209, 207, 205, 203, 201, 199, 
197, 195, 193, 190, 188, 186, 184, 181, 179, 177, 174, 172, 169, 167, 164, 162, 
159, 157, 154, 151, 148, 145, 143, 140, 137, 134, 130, 127, 124, 120, 117, 113, 
110, 106, 102, 98, 93, 89, 84, 79, 73, 67, 61, 54, 45, 35, 21, 0};

short shortAngleDist(short a0, short a1){
  short da = (a1 - a0) % 4096;
  return (da<<1) % 4096 - da;
}

short dotProductXYZ(SVECTOR * a, SVECTOR * b) {
  long temp = (a->vx * b->vx + a->vy * b->vy + a->vz * b->vz)>>12;
  return (short) temp;
}

//extern short dotProductXZ(SVECTOR * a, SVECTOR * b);
short dotProductXZ(VECTOR * a, SVECTOR * b) {
  long temp = (a->vx * b->vx + a->vz * b->vz)>>12;
  return (short) temp;
}

long dotProductXZex(VECTOR * a, SVECTOR * b) {
  long temp = (a->vx * b->vx + a->vz * b->vz);
  return temp;
}

long dotProductXZex2(VECTOR * a, SVECTOR * b) {
  long temp = fix12_mul(a->vx,b->vx) + fix12_mul(a->vz,b->vz);
  return temp;
}

long dotProductXYex2(VECTOR * a, SVECTOR * b) {
  long temp = fix12_mul(a->vx,b->vx) + fix12_mul(a->vy,b->vy);
  return temp;
}

short dotProduct2D(short ax, short ay, short bx, short by) {
  return (short) ((((long)ax) * ((long)ay) + ((long)bx) * ((long)by)) >> 12);
}

extern long dotProduct2DR(short ax, short ay, short bx, short by) {
  return ((((long)ax) * ((long)ay) + ((long)bx) * ((long)by)));
}

// From PSn00bSDK
void crossProduct(SVECTOR * v0, SVECTOR * v1, VECTOR * out) {
  out->vx = ((v0->vy*v1->vz)-(v0->vz*v1->vy))>>12;
  out->vy = ((v0->vz*v1->vx)-(v0->vx*v1->vz))>>12;
  out->vz = ((v0->vx*v1->vy)-(v0->vy*v1->vx))>>12;
}
void crossProductS(SVECTOR * v0, SVECTOR * v1, SVECTOR * out) {
    out->vx = ((v0->vy*v1->vz)-(v0->vz*v1->vy))>>12;
    out->vy = ((v0->vz*v1->vx)-(v0->vx*v1->vz))>>12;
    out->vz = ((v0->vx*v1->vy)-(v0->vy*v1->vx))>>12;
}

int fix16_mul(int inArg0, int inArg1)
{
	int64 product = (int64)inArg0 * inArg1;
	
	#ifndef FIXMATH_NO_OVERFLOW
	// The upper 17 bits should all be the same (the sign).
	u_long upper = (product >> 47);
	#endif
	
	if (product < 0)
	{
		#ifndef FIXMATH_NO_OVERFLOW
		if (~upper)
				return fix16_overflow;
		#endif
		
		#ifndef FIXMATH_NO_ROUNDING
		// This adjustment is required in order to round -1/2 correctly
		product--;
		#endif
	}
	else
	{
		#ifndef FIXMATH_NO_OVERFLOW
		if (upper)
				return fix16_overflow;
		#endif
	}
	
	#ifdef FIXMATH_NO_ROUNDING
	return product >> 16;
	#else
	int result = product >> 16;
	result += (product & 0x8000) >> 15;
	
	return result;
	#endif
}

static u_char clz(u_int x)
{
  u_char result = 0;
  if (x == 0) return 32;
  while (!(x & 0xF0000000)) { result += 4; x <<= 4; }
  while (!(x & 0x80000000)) { result += 1; x <<= 1; }
  return result;
}

int fix16_div(int a, int b)
{
  int result;
  u_long remainder;
  u_long divider;
  u_long quotient = 0;
  int bit_pos = 17;
  // This uses a hardware 32/32 bit division multiple times, until we have
  // computed all the bits in (a<<17)/b. Usually this takes 1-3 iterations.
  
  if (b == 0)
      return fix16_min;
  
  remainder = (a >= 0) ? a : (-a);
  divider = (b >= 0) ? b : (-b);
  
  // Kick-start the division a bit.
  // This improves speed in the worst-case scenarios where N and D are large
  // It gets a lower estimate for the result by N/(D >> 17 + 1).
  if (divider & 0xFFF00000)
  {
    u_long shifted_div = ((divider >> 17) + 1);
    quotient = remainder / shifted_div;
    remainder -= ((u_int64)quotient * divider) >> 17;
  }
  
  // If the divider is divisible by 2^n, take advantage of it.
  while (!(divider & 0xF) && bit_pos >= 4)
  {
    divider >>= 4;
    bit_pos -= 4;
  }
  
  while (remainder && bit_pos >= 0)
  {
	u_long div;
    // Shift remainder as much as we can without overflowing
	int shift = clz(remainder);
    /*int shift = clz(remainder);*/
    if (shift > bit_pos) shift = bit_pos;
    remainder <<= shift;
    bit_pos -= shift;
    
    div = remainder / divider;
    remainder = remainder % divider;
    quotient += div << bit_pos;
    
    remainder <<= 1;
    bit_pos--;
  }
  
  result = quotient >> 1;
  
  // Figure out the sign of the result
  if ((a ^ b) & 0x80000000)
  {    
    result = -result;
  }
  
  return result;
}


int fix12_div(int a, int b)
{
  int result;
  u_long remainder;
  u_long divider;
  u_long quotient = 0;
  int bit_pos = 13;
  // This uses a hardware 32/32 bit division multiple times, until we have
  // computed all the bits in (a<<17)/b. Usually this takes 1-3 iterations.
  
  if (b == 0)
      return fix16_min;
  
  remainder = (a >= 0) ? a : (-a);
  divider = (b >= 0) ? b : (-b);
  
  // Kick-start the division a bit.
  // This improves speed in the worst-case scenarios where N and D are large
  // It gets a lower estimate for the result by N/(D >> 17 + 1).
  if (divider & 0xFFFF0000)
  {
    u_long shifted_div = ((divider >> 13) + 1);
    quotient = remainder / shifted_div;
    remainder -= ((u_int64)quotient * divider) >> 13;
  }
  
  // If the divider is divisible by 2^n, take advantage of it.
  while (!(divider & 0xF) && bit_pos >= 4)
  {
    divider >>= 4;
    bit_pos -= 4;
  }
  
  while (remainder && bit_pos >= 0)
  {
	u_long div;
    // Shift remainder as much as we can without overflowing
	int shift = clz(remainder);
    /*int shift = clz(remainder);*/
    if (shift > bit_pos) shift = bit_pos;
    remainder <<= shift;
    bit_pos -= shift;
    
    div = remainder / divider;
    remainder = remainder % divider;
    quotient += div << bit_pos;
    
    remainder <<= 1;
    bit_pos--;
  }
  
  result = quotient >> 1;
  
  // Figure out the sign of the result
  if ((a ^ b) & 0x80000000)
  {    
    result = -result;
  }
  
  return result;
}
 
int fix16_atan2(int inY , int inX) {
	int abs_inY, mask, angle, r, r_3;

	/* Absolute inY */
	mask = (inY >> (sizeof(int)*CHAR_BIT-1));
	abs_inY = (inY + mask) ^ mask;

	if (inX >= 0)
	{
		r = fix16_div( (inX - abs_inY), (inX + abs_inY));
		r_3 = fix16_mul(fix16_mul(r, r),r);
		angle = fix16_mul(0x00003240 , r_3) - fix16_mul(0x0000FB50,r) + PI_DIV_4;
	} else {
		r = fix16_div( (inX + abs_inY), (abs_inY - inX));
		r_3 = fix16_mul(fix16_mul(r, r),r);
		angle = fix16_mul(0x00003240 , r_3) - fix16_mul(0x0000FB50,r) + THREE_PI_DIV_4;
	}
	if (inY < 0)
	{
		angle = -angle;
	}

	return angle;
}

int fix12_atan2(int inY , int inX) {
	int abs_inY, mask, angle, r, r_3;

	/* Absolute inY */
	mask = (inY >> (sizeof(int)*CHAR_BIT-1));
	abs_inY = (inY + mask) ^ mask;

	if (inX >= 0)
	{
		r = fix12_div( (inX - abs_inY), (inX + abs_inY));
		r_3 = fix12_mul(fix12_mul(r, r),r);
		angle = fix12_mul(0x00000324 , r_3) - fix12_mul(0x00000FB5,r) + PI_DIV_4_F12;
	} else {
		r = fix12_div( (inX + abs_inY), (abs_inY - inX));
		r_3 = fix12_mul(fix12_mul(r, r),r);
		angle = fix12_mul(0x00000324 , r_3) - fix12_mul(0x00000FB5,r) + THREE_PI_DIV_4_F12;
	}
	if (inY < 0)
	{
		angle = -angle;
	}

	return angle;
}

int fix12_atan2s(int inY , int inX){
  return (fix12_atan2(inY, inX) << 7) / 0x0324;
}


int fpmath_atan2(int inY , int inX) {
	int abs_inY, mask, angle, r, r_3;

	/* Absolute inY */
	mask = (inY >> (sizeof(int)*CHAR_BIT-1));
	abs_inY = (inY + mask) ^ mask;

	if (inX >= 0)
	{
		r = fix16_div( (inX - abs_inY), (inX + abs_inY));
		r_3 = asm_fp16mul64(asm_fp16mul64(r, r),r);
		angle = asm_fp16mul64(0x00003240 , r_3) - asm_fp16mul64(0x0000FB50,r) + PI_DIV_4;
	} else {
		r = fix16_div( (inX + abs_inY), (abs_inY - inX));
		r_3 = asm_fp16mul64(asm_fp16mul64(r, r),r);
		angle = asm_fp16mul64(0x00003240 , r_3) - asm_fp16mul64(0x0000FB50,r) + THREE_PI_DIV_4;
	}
	if (inY < 0)
	{
		angle = -angle;
	}

	return angle;
}
 
//static short q15_from_double(const double d) {
//    return lrint(d * 32768);
//}

/**
 * Negative absolute value. Used to avoid undefined behavior for most negative
 * integer (see C99 standard 7.20.6.1.2 and footnote 265 for the description of
 * abs/labs/llabs behavior).
 *
 * @param i 16-bit signed integer
 * @return negative absolute value of i; defined for all values of i
 */
static short s16_nabs(const short j) {
#if (((short)-1) >> 1) == ((short)-1)
    // signed right shift sign-extends (arithmetic)
    const short negSign = ~(j >> 15); // splat sign bit into all 16 and complement
    // if j is positive (negSign is -1), xor will invert j and sub will add 1
    // otherwise j is unchanged
    return (j ^ negSign) - negSign;
#else
    return (j < 0 ? j : -j);
#endif
}

/**
 * Q15 (1.0.15 fixed point) multiplication. Various common rounding modes are in
 * the function definition for reference (and preference).
 *
 * @param j 16-bit signed integer representing -1 to (1 - (2**-15)), inclusive
 * @param k same format as j
 * @return product of j and k, in same format
 */
static short q15_mul(const short j, const short k) {
    const long intermediate = j * (long)k;
#if 0 // don't round
    return intermediate >> 15;
#elif 0 // biased rounding
    return (intermediate + 0x4000) >> 15;
#else // unbiased rounding
    return (intermediate + ((intermediate & 0x7FFF) == 0x4000 ? 0 : 0x4000)) >> 15;
#endif
}

/**
 * Q15 (1.0.15 fixed point) division (non-saturating). Be careful when using
 * this function, as it does not behave well when the result is out-of-range.
 *
 * Value is not defined if numerator is greater than or equal to denominator.
 *
 * @param numer 16-bit signed integer representing -1 to (1 - (2**-15))
 * @param denom same format as numer; must be greater than numerator
 * @return numer / denom in same format as numer and denom
 */
static short q15_div(const short numer, const short denom) {
    return ((long)numer << 15) / denom;
}

/**
 * 16-bit fixed point four-quadrant arctangent. Given some Cartesian vector
 * (x, y), find the angle subtended by the vector and the positive x-axis.
 *
 * The value returned is in units of 1/65536ths of one turn. This allows the use
 * of the full 16-bit unsigned range to represent a turn. e.g. 0x0000 is 0
 * radians, 0x8000 is pi radians, and 0xFFFF is (65535 / 32768) * pi radians.
 *
 * Because the magnitude of the input vector does not change the angle it
 * represents, the inputs can be in any signed 16-bit fixed-point format.
 *
 * @param y y-coordinate in signed 16-bit
 * @param x x-coordinate in signed 16-bit
 * @return angle in (val / 32768) * pi radian increments from 0x0000 to 0xFFFF
 */
unsigned short fxpt_atan2(const short y, const short x) {
    short nabs_y, nabs_x;
    if (x == y) { // x/y or y/x would return -1 since 1 isn't representable
        if (y > 0) { // 1/8
            return 8192;
        } else if (y < 0) { // 5/8
            return 40960;
        } else { // x = y = 0
            return 0;
        }
    }
    nabs_y = s16_nabs(y);
    nabs_x = s16_nabs(x);
    if (nabs_x < nabs_y) { // octants 1, 4, 5, 8
        const short y_over_x = q15_div(y, x);
        const short correction = q15_mul(STATIC0, s16_nabs(y_over_x));
        const short unrotated = q15_mul(STATIC1 + correction, y_over_x);
        if (x > 0) { // octants 1, 8
            return unrotated;
        } else { // octants 4, 5
            return 32768 + unrotated;
        }
    } else { // octants 2, 3, 6, 7
        const short x_over_y = q15_div(x, y);
        const short correction = q15_mul(STATIC0, s16_nabs(x_over_y));
        const short unrotated = q15_mul(STATIC1 + correction, x_over_y);
        if (y > 0) { // octants 2, 3
            return 16384 - unrotated;
        } else { // octants 6, 7
            return 49152 - unrotated;
        }
    }
}

int vec_mag(int x, int y) {
  int mag = x * x + y * y;
  //return mag;
  return SquareRoot0(mag);
}

int hisin(int x) {
    int c, x2, y;

    c= x<<(30-qN);              // Semi-circle info into carry.
    x -= 1<<qN;                 // sine -> cosine calc

    x= x<<(31-qN);              // Mask with PI
    x= x>>(31-qN);              // Note: SIGNED shift! (to qN)
    x= x*x>>(2*qN-14);          // x=x^2 To Q14

    y= B - (x*C>>14);           // B - x^2*C
    y= (1<<qA)-(x*y>>16);       // A - x^2*(B-x^2*C)

    return c>=0 ? y : -y;
}

int hicos(int x) {
    return hisin( x+32768 );
}

int isin(int x) {
    int c, x2, y;

    c= x<<(30-qN);              // Semi-circle info into carry.
    x -= 1<<qN;                 // sine -> cosine calc

    x= x<<(31-qN);              // Mask with PI
    x= x>>(31-qN);              // Note: SIGNED shift! (to qN)

    x= x*x>>(2*qN-14);          // x=x^2 To Q14

    y= B - (x*C>>14);           // B - x^2*C
    y= (1<<qA)-(x*y>>16);       // A - x^2*(B-x^2*C)

    return c>=0 ? y : -y;
}

int icos(int x) {
    return isin( x+1024 );
}

int fix12_mul(int inArg0, int inArg1) {
  int64 result = (int64)inArg0 * inArg1;
  return result >> 12;
}
// Avoids a few extra instructions
extern int fix12_smul(int inArg0, int inArg1){
  return (inArg0 * inArg1) >> 12;
}

int fix12_lerp(int a, int b, int t){
  return a + fix12_mul(t, (b - a));
}

long fix12_arcsin(long s)
{
	long fsin = (1024 - s) >> 2;
	
	if(fsin < 0)
		return 2048 - acostable[fsin + 1024];
	else
		return acostable[fsin];
}

long fix12_arccos(long c)
{
	long fcos = c >> 2;
	if (fcos < 0)
		return 2048 - acostable[fcos + 1024];
	else
		return acostable[fcos];
}


// From http://www.euclideanspace.com/maths/geometry/rotations/conversions/eulerToQuaternion/index.htm
void fix12_toQuaternion(QUATERNION * quat, SVECTOR * ang){
  long c1 = icos(ang->vz >> 1); // Z / Yaw / Heading
  long s1 = isin(ang->vz >> 1);
  long c2 = icos(ang->vy >> 1); // Y / Pitch / Attitude
  long s2 = isin(ang->vy >> 1);
  long c3 = icos(ang->vx >> 1); // X / Roll / Bank
  long s3 = isin(ang->vx >> 1);
  long c1c2 = fix12_smul(c1, c2);
  long s1s2 = fix12_smul(s1, s2);

  quat->vx = fix12_smul(s1s2, c3) - fix12_smul(c1c2, s3);
  // sign hack, thanks to sickle.
  quat->vy = -(fix12_smul(fix12_smul(c1, s2), c3) + fix12_smul(fix12_smul(s1, c2), s3));
  quat->vz = -(fix12_smul(fix12_smul(s1, c2), c3) - fix12_smul(fix12_smul(c1, s2), s3));

  quat->vw = fix12_smul(c1c2, c3) + fix12_smul(s1s2, s3);
}

// From http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/index.htm
void fix12_toEuler(SVECTOR * ang, QUATERNION * quat) {
  long sqx, sqy, sqz;
	long test = fix12_smul(quat->vx,quat->vy) + fix12_mul(quat->vz,quat->vw);
	if (test > 2047) { // singularity at north pole
		ang->vz = fix12_atan2s(quat->vx,quat->vw) << 1;
		ang->vy = FIX12_HPI;
		ang->vx = 0;
		return;
	}
	if (test < -2047) { // singularity at south pole
		ang->vz = (-fix12_atan2s(quat->vx,quat->vw)) << 1;
		ang->vy = - FIX12_HPI;
		ang->vx = 0;
		return;
	}
  sqx = fix12_smul(quat->vx,quat->vx);
  sqy = fix12_smul(quat->vy,quat->vy);
  sqz = fix12_smul(quat->vz,quat->vz);
  ang->vx = fix12_atan2s((fix12_smul(quat->vy,quat->vw) - fix12_smul(quat->vx,quat->vz)) << 1, (4096 - (sqy<<1) - (sqz<<1)));
	ang->vy = fix12_arccos(test << 1);
	ang->vz = fix12_atan2s((fix12_smul(quat->vx,quat->vw) - fix12_smul(quat->vy,quat->vz)) << 1, (4096 - (sqx<<1) - (sqz<<1)));
}

// Slow code ahead, use with caution!
void fix12_normalizeQuaternion(QUATERNION * dest, QUATERNION * src){
  long s, tempcalc, squatw;
  VECTOR sqrin, sqrout;
  sqrin.vx = src->vx;
  sqrin.vy = src->vy;
  sqrin.vz = src->vz;
  gte_ldlvl(&sqrin);
  squatw = src->vw;
  gte_nop();
  gte_sqr0_b();
  squatw = squatw * squatw;
  gte_stlvnl(&sqrout);

  tempcalc = ((sqrout.vx + sqrout.vy + sqrout.vz + squatw) >> 12);
  squatw = src->vw;
	
	s = (tempcalc > 1) ? (33554432 / tempcalc) : (33554432);

  dest->vx = fix12_smul(sqrin.vx, s);
  dest->vy = fix12_smul(sqrin.vy, s);
  dest->vz = fix12_smul(sqrin.vz, s);
  dest->vw = fix12_smul(squatw, s);
}

// Very slow and unoptimized code ahead
void fix12_quaternionMul(QUATERNION * dest, QUATERNION * a, QUATERNION * b) {
  QUATERNION quat;
  // Maybe this can be done using the GTE?
  quat.vx = (( a->vx * b->vw) + (a->vy * b->vz) - (a->vz * b->vy) + (a->vw * b->vx)) >> 12;
  quat.vy = ((-a->vx * b->vz) + (a->vy * b->vw) + (a->vz * b->vx) + (a->vw * b->vy)) >> 12;
  quat.vz = (( a->vx * b->vy) - (a->vy * b->vx) + (a->vz * b->vw) + (a->vw * b->vz)) >> 12;
  quat.vw = ((-a->vx * b->vx) - (a->vy * b->vy) - (a->vz * b->vz) + (a->vw * b->vw)) >> 12;

  *dest = quat;
}

// This is from a copyrighted work almost as-is, still needs to be modified
void quaternionGetMatrix(QUATERNION *squat, MATRIX *dmatrix)
{
	long	s, xs,ys,zs, wx,wy,wz, xx,xy,xz, yy,yz,zz,tempcalc;
	long	squatw;
	VECTOR	sqrin, sqrout;

	sqrin.vx = squat->vx;
	sqrin.vy = squat->vy;
	sqrin.vz = squat->vz;

	gte_ldlvl(&sqrin);
	squatw = squat->vw;
	gte_nop();
	gte_sqr0_b();
	squatw = squatw * squatw;
	gte_stlvnl(&sqrout);

	tempcalc = ((sqrout.vx + sqrout.vy + sqrout.vz + squatw) >> 12);
	squatw = squat->vw;
	
	s = (tempcalc > 1) ? (33554432 / tempcalc) : (33554432);
	
	xs = fix12_smul(sqrin.vx, s);
	ys = fix12_smul(sqrin.vy, s);
	zs = fix12_smul(sqrin.vz, s);

	wx = fix12_smul(squatw, xs);
	wy = fix12_smul(squatw, ys);
	wz = fix12_smul(squatw, zs);

	xx = fix12_smul(sqrin.vx, xs);
	xy = fix12_smul(sqrin.vx, ys);
	xz = fix12_smul(sqrin.vx, zs);

	yy = fix12_smul(sqrin.vy, ys);
	yz = fix12_smul(sqrin.vy, zs);
	zz = fix12_smul(sqrin.vz, zs);

	dmatrix->m[0][0] = 4096 - (yy + zz);
	dmatrix->m[0][1] = xy + wz;
	dmatrix->m[0][2] = xz - wy;

	dmatrix->m[1][0] = xy - wz;
	dmatrix->m[1][1] = 4096 - (xx + zz);
	dmatrix->m[1][2] = yz + wx;

	dmatrix->m[2][0] = xz + wy;
	dmatrix->m[2][1] = yz - wx;
	dmatrix->m[2][2] = 4096 - (xx + yy);
}

void fix12_quaternionSlerp(QUATERNION * dest, QUATERNION * a, QUATERNION * b, u_long t) {
  VECTOR		interp, source, tdest;

  source.vx = a->vx;
  source.vy = a->vy;
  source.vz = a->vz;

  // adjust signs
  if ((source.vx * b->vx + source.vy * b->vy + source.vz * b->vz + a->vw * b->vw) < 0) {
    tdest.vx = -b->vx;
    tdest.vy = -b->vy;
    tdest.vz = -b->vz;
    tdest.pad = -b->vw; // Hey, It's free real estate.
  } else {
    tdest.vx = b->vx;
    tdest.vy = b->vy;
    tdest.vz = b->vz;
    tdest.pad = b->vw;
  }
    
  // use linear interpolation
  gte_ldlvl(&source); // load source
  gte_ldfc(&tdest);   // load destination
  gte_lddp(t);        // load interpolant
  gte_intpl();        // interpolate

  source.vx = a->vw;	  // move source w into source
  tdest.vx = tdest.pad; // move tdest pad into tdest
    
  gte_stlvnl(&interp);  // put interpolated x y & z into interp

  gte_ldlvl(&source); // load source w
  gte_ldfc(&tdest);   // load destination w
  gte_intpl();        // interpolate
  gte_stlvnl(&tdest); // move interpolated w into tdest

  dest->vx = interp.vx;
  dest->vy = interp.vy;
  dest->vz = interp.vz;
  dest->vw = tdest.vx;
}

// Input "Quaternion": vx,vy,vz: Rotation Axis, vw: Rotation angle
// Todo: Change to a more fitting input structure?
// Also this is not working.
void fix12_quaternionFromAxisAngle(QUATERNION * dest, QUATERNION * rot){
  QUATERNION quat;
  long theta_h = rot->vw >> 1;
  long factor = hisin(theta_h);
  
  quat.vx = rot->vx;
  quat.vy = rot->vy;
  quat.vz = rot->vz;

  dest->vx = fix12_smul(quat.vx, factor);
  dest->vy = fix12_smul(quat.vy, factor);
  dest->vz = fix12_smul(quat.vz, factor);

  dest->vw = hicos(theta_h);

  fix12_normalizeQuaternion(dest,dest);

}

// Interpolate short vector, fast but low accuracy.
void fix12_gtesvlerp(SVECTOR * dest, SVECTOR * a, SVECTOR * b, int t) {
  VECTOR result;
  gte_ld_intpol_sv0(a);		// load x, y, z
  gte_ld_intpol_sv1(b);	  // load x, y, z
  gte_lddp(t);			      // load interpolant
  gte_intpl();			      // interpolate (8 cycles)
  gte_stlvnl(&result);    // store result
  dest->vx = result.vx;
  dest->vy = result.vy;
  dest->vz = result.vz;
}
// Interpolates single value (TODO: look for better way to load a single value instead of gte_ldlvl)
int fix12_gtelerp(int a, int b, int t) {
  VECTOR result;
  gte_ldlvl(&a);		// load source
	gte_ldfc(&b);		// load dest
  gte_lddp(&t);
	gte_intpl();			  // interpolate (8 cycles)
	gte_stlvnl(&result);  // put interpolated value into dest
  return result.vx;
}

long get_distanceXZ(SVECTOR * dist) {
  long d = dist->vx * dist->vx + dist->vz * dist->vz;
  return SquareRoot0(d);
};

long get_distanceXZsq(SVECTOR * dist) {
  return dist->vx * dist->vx + dist->vz * dist->vz;
}

long get_distanceXYZ(SVECTOR * dist) {
  VECTOR sqin, sqout;
  sqin.vx = dist->vx;
  sqin.vy = dist->vy;
  sqin.vz = dist->vz;
  gte_ldlvl(&sqin);
	gte_nop();
	gte_sqr0_b();
	gte_stlvnl(&sqout);
  return SquareRoot0(sqout.vx + sqout.vy + sqout.vz);
};

short get_angleFromVectorXZ(SVECTOR * vec) {
  long len = SquareRoot0(vec->vx * vec->vx + vec->vz * vec->vz);
  short angv_x = (vec->vx<<12) / len;
  short angv_z = (vec->vz<<12) / len;
  return fix12_atan2s(angv_x, angv_z);
}