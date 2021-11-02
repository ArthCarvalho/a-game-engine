;
; The 2D subdivider code (C) 1996 Derek Leigh-Gilchrist / Core Design.
; Email del@nlights.demon.co.uk
;
; Added support for colored gouraud shading -- Morten Ofstad / SCEE.
; Greets to: Del, Dan, Bob, Facet, Spaceman, S'Nao & The rest of Lemon! Keep passing it!
;
; tab spaces = 8 for this source...
;
; doesn`t use lo or hi regs... (IE this does 0 mults/divs)
; now actually takes cluts,polycode,tpage ids from original poly...
; double the speed of the C version
; 700 bytes shorter than C version
; inline addprims
; DIVMAX equate for this version is 4, so pass 1 or 2 as ndiv
;
; - Colored version is as fast as the old one, due to less instructions in polygon write
; - phase (which is executed 4 times more than the subdivision itself)
;
; Fixed silly minky bug in 'DEL_subdivide_gt3_asm' (VERY SILLY RE-ARRANGE PROBLEM!)
; coded 'DEL_subdivide_ft4_asm' with equates for shit... wow!!
; coded 'DEL_subdivide_ft3_asm' with equates for shit... wow!!
; updated gouraud versions to use equates / structures... (WOW!)
; updated flat poly stuff to rip RGB values from original poly (at the same time as the code)
;
; uses scratch-pad ram from $1f80000-$1f801b0 (honest, thats all it uses!)
;

		section poly_subdiv_code

		opt L.,C+			;local labels = .

		global DEL_subdivide_gt4_asm	;gouraud textured quad subdiv
		global DEL_subdivide_gt3_asm	;gouraud textured tri subdiv
		global DEL_subdivide_ft4_asm	;flat textured quad subdiv
		global DEL_subdivide_ft3_asm	;flat textured tri subdiv

scratchpad_base		equ $1f80
regs_offset 	   	equ 400

DIVMAX	   		equ 4			;subdivide amount...


;
; eat my SPOL structure...
;

      		rsreset
SPOL_x		rh 1
SPOL_y		rh 1
SPOL_u		rh 1
SPOL_v		rh 1
SPOL_rgb	rw 1
SPOL_pad	rw 1
SPOL_size 	rb 0		;This HAS to be 16 bytes long, as shifts are being used

; taste the prim length equates

POLYFT4_PRIMLEN		equ 9  			;#define setPolyFT4(p)	setlen(p, 9), 	setcode(p, 0x2c)
POLYFT3_PRIMLEN		equ 7  	       		;#define setPolyFT3(p)	setlen(p, 7), 	setcode(p, 0x24)
POLYGT4_PRIMLEN		equ 12			;#define setPolyGT4(p)	setlen(p, 12), 	setcode(p, 0x3c)
POLYGT3_PRIMLEN		equ 9			;#define setPolyGT3(p)	setlen(p, 9), 	setcode(p, 0x34)

;
; eat my POLYFT3 structure... (nice)
;

		rsreset
POLYFT3_tag	rw 1
POLYFT3_r0	rb 1
POLYFT3_g0	rb 1
POLYFT3_b0	rb 1
POLYFT3_code	rb 1
POLYFT3_x0	rh 1
POLYFT3_y0	rh 1
POLYFT3_u0	rb 1
POLYFT3_v0	rb 1
POLYFT3_clut	rh 1
POLYFT3_x1	rh 1
POLYFT3_y1	rh 1
POLYFT3_u1	rb 1
POLYFT3_v1	rb 1
POLYFT3_tpage	rh 1
POLYFT3_x2	rh 1
POLYFT3_y2	rh 1
POLYFT3_u2	rb 1
POLYFT3_v2	rb 1
POLYFT3_pad1	rh 1		;pad
POLYFT3_size	rb 0		;8 longs

;
; eat my POLYFT4 structure... (nice)
;
		rsreset
POLYFT4_tag	rw 1
POLYFT4_r0	rb 1
POLYFT4_g0	rb 1
POLYFT4_b0	rb 1
POLYFT4_code	rb 1
POLYFT4_x0	rh 1
POLYFT4_y0	rh 1
POLYFT4_u0	rb 1
POLYFT4_v0	rb 1
POLYFT4_clut	rh 1
POLYFT4_x1	rh 1
POLYFT4_y1	rh 1
POLYFT4_u1	rb 1
POLYFT4_v1	rb 1
POLYFT4_tpage	rh 1
POLYFT4_x2	rh 1
POLYFT4_y2	rh 1
POLYFT4_u2	rb 1
POLYFT4_v2	rb 1
POLYFT4_pad1	rh 1		;pad
POLYFT4_x3	rh 1
POLYFT4_y3	rh 1
POLYFT4_u3	rb 1
POLYFT4_v3	rb 1
POLYFT4_pad2	rh 1		;pad
POLYFT4_size	rb 0		;10 longs

;
; eat my POLYGT3 structure... (nice)
;

		rsreset
POLYGT3_tag	rw 1
POLYGT3_r0	rb 1
POLYGT3_g0	rb 1
POLYGT3_b0	rb 1
POLYGT3_code	rb 1
POLYGT3_x0	rh 1
POLYGT3_y0	rh 1
POLYGT3_u0	rb 1
POLYGT3_v0	rb 1
POLYGT3_clut	rh 1
POLYGT3_r1	rb 1
POLYGT3_g1	rb 1
POLYGT3_b1	rb 1
POLYGT3_p1	rb 1		;pad
POLYGT3_x1	rh 1
POLYGT3_y1	rh 1
POLYGT3_u1	rb 1
POLYGT3_v1	rb 1
POLYGT3_tpage	rh 1
POLYGT3_r2	rb 1
POLYGT3_g2	rb 1
POLYGT3_b2	rb 1
POLYGT3_p2	rb 1		;pad
POLYGT3_x2	rh 1
POLYGT3_y2	rh 1
POLYGT3_u2	rb 1
POLYGT3_v2	rb 1
POLYGT3_pad2	rh 1		;pad
POLYGT3_size	rb 0		;size = 10 longs (40 bytes)

;
; eat my POLYGT4 structure... (nice)
;

		rsreset
POLYGT4_tag	rw 1
POLYGT4_r0	rb 1
POLYGT4_g0	rb 1
POLYGT4_b0	rb 1
POLYGT4_code	rb 1
POLYGT4_x0	rh 1
POLYGT4_y0	rh 1
POLYGT4_u0	rb 1
POLYGT4_v0	rb 1
POLYGT4_clut	rh 1
POLYGT4_r1	rb 1
POLYGT4_g1	rb 1
POLYGT4_b1	rb 1
POLYGT4_p1	rb 1		;pad
POLYGT4_x1	rh 1
POLYGT4_y1	rh 1
POLYGT4_u1	rb 1
POLYGT4_v1	rb 1
POLYGT4_tpage	rh 1
POLYGT4_r2	rb 1
POLYGT4_g2	rb 1
POLYGT4_b2	rb 1
POLYGT4_p2	rb 1		;pad
POLYGT4_x2	rh 1
POLYGT4_y2	rh 1
POLYGT4_u2	rb 1
POLYGT4_v2	rb 1
POLYGT4_pad2	rh 1		;pad
POLYGT4_r3	rb 1
POLYGT4_g3	rb 1
POLYGT4_b3      rb 1
POLYGT4_p3	rb 1		;pad
POLYGT4_x3	rh 1
POLYGT4_y3	rh 1
POLYGT4_u3	rb 1
POLYGT4_v3	rb 1
POLYGT4_pad3	rh 1
POLYGT4_size	rb 0		;size = 13 longs (52 bytes)









; enter with:-
;
; r4 = POLY_GT4 *cur_primlist (ie the poly you just made)
; r5 = u_long *OT (address of order table entry for this poly)
; r6 = ndiv (1,2)
;
;
; returns:-
;
; r2 = number of longs to add to your primlist...
;
; SWEETLY optimized! (no stacking, uses scratchpad)
;

DEL_subdivide_gt4_asm

	lui	r11,scratchpad_base 		;r11 = spol (the subdivide buffer in scratchpad)
	sw	r16,regs_offset(r11)
	sw	r17,regs_offset+4(r11)
	sw	r18,regs_offset+8(r11)
	sw	r19,regs_offset+12(r11)

	move	r16,r4				;r16 = original_poly
	move	r19,r5				;r19 = ot_me

; setup top left co-ords

	lw	r2,POLYGT4_x0(r16)		;r2 = original_poly->x0	and original_poly->y0
	lbu	r3,POLYGT4_u0(r16)		;r2 = original_poly->u0 (byte)
	lbu	r4,POLYGT4_v0(r16)		;r2 = original_poly->v0 (byte)
	lw	r5,POLYGT4_r0(r16)		;r2 = original_poly->r0 (byte)

	move	r18,r6				;r18 = ndiv

	sw	r2,SPOL_x(r11)			;spol->xy[0] = original_poly->x0 and spol->xy[1] = original_poly->y0;
	sh	r3,SPOL_u(r11)			;spol->uv[0] = original_poly->u0;
	sh	r4,SPOL_v(r11)			;spol->uv[1] = original_poly->v0;
	sw	r5,SPOL_rgb(r11)		;spol->rgb[0] = original_poly->r0;

;setup top right co-ords

	lw	r2,POLYGT4_x1(r16)		;r2 = original_poly->x1 and original_poly->y1
	lbu	r3,POLYGT4_u1(r16) 		;r2 = original_poly->u1
	lbu	r4,POLYGT4_v1(r16)		;r2 = original_poly->v1
	lw	r5,POLYGT4_r1(r16)		;r2 = original_poly->r1

	sw	r2,((DIVMAX)*SPOL_size)+SPOL_x(r11)	;(spol+DIVMAX)->xy[0] = original_poly->x1 and (spol+DIVMAX)->xy[1] = original_poly->y1;
	sh	r3,((DIVMAX)*SPOL_size)+SPOL_u(r11)	;(spol+DIVMAX)->uv[0] = original_poly->u1;
	sh	r4,((DIVMAX)*SPOL_size)+SPOL_v(r11)	;(spol+DIVMAX)->uv[1] = original_poly->v1;
	sw	r5,((DIVMAX)*SPOL_size)+SPOL_rgb(r11)	;(spol+DIVMAX)->rgb[0] = original_poly->r1;

;setup bottom left co-ords

	lw	r2,POLYGT4_x2(r16) 		;r2 = original_poly->x2 and original_poly->y2
	lbu	r3,POLYGT4_u2(r16) 		;r2 = original_poly->u2
	lbu	r4,POLYGT4_v2(r16) 		;r2 = original_poly->v2
	lw	r5,POLYGT4_r2(r16) 		;r2 = original_poly->r2

	sw	r2,((DIVMAX*(DIVMAX+1))*SPOL_size)+SPOL_x(r11)	;(spol+(DIVMAX*(DIVMAX+1)))->xy[0] = original_poly->x2 and (spol+(DIVMAX*(DIVMAX+1)))->xy[1] = original_poly->y2
	sh	r3,((DIVMAX*(DIVMAX+1))*SPOL_size)+SPOL_u(r11)	;(spol+(DIVMAX*(DIVMAX+1)))->uv[0] = original_poly->u2;
	sh	r4,((DIVMAX*(DIVMAX+1))*SPOL_size)+SPOL_v(r11) 	;(spol+(DIVMAX*(DIVMAX+1)))->uv[1] = original_poly->v2;
	sw	r5,((DIVMAX*(DIVMAX+1))*SPOL_size)+SPOL_rgb(r11) 	;(spol+(DIVMAX*(DIVMAX+1)))->rgb[0] = original_poly->r2;

;setup bottom right co-ords

	lw	r2,POLYGT4_x3(r16)		;r2 = original_poly->x3 and original_poly->y3
	lbu	r3,POLYGT4_u3(r16)		;r2 = original_poly->u3
	lbu	r4,POLYGT4_v3(r16)		;r2 = original_poly->v3
	lw	r5,POLYGT4_r3(r16)		;r2 = original_poly->r3

	move	r17,r0				;r17 = ndiv_counter

	sw	r2,(((DIVMAX*(DIVMAX+1))+DIVMAX)*SPOL_size)+SPOL_x(r11)	;(spol+((DIVMAX*(DIVMAX+1)))+DIVMAX)->xy[0] = original_poly->x3 and (spol+((DIVMAX*(DIVMAX+1)))+DIVMAX)->xy[1] = original_poly->y3;
	sh	r3,(((DIVMAX*(DIVMAX+1))+DIVMAX)*SPOL_size)+SPOL_u(r11)	;(spol+((DIVMAX*(DIVMAX+1)))+DIVMAX)->uv[0] = original_poly->u3;
	sh	r4,(((DIVMAX*(DIVMAX+1))+DIVMAX)*SPOL_size)+SPOL_v(r11)	;(spol+((DIVMAX*(DIVMAX+1)))+DIVMAX)->uv[1] = original_poly->v3;
	sw	r5,(((DIVMAX*(DIVMAX+1))+DIVMAX)*SPOL_size)+SPOL_rgb(r11)	;(spol+((DIVMAX*(DIVMAX+1)))+DIVMAX)->rgb[0] = original_poly->r3;

	blez	r18,.make_the_polys		;ndiv <= 0 ??
	li	r2,DIVMAX			;r2 = DIVMAX (4)

.main_ndiv_loop
	sra	r12,r2,r17			;r12 = temp1 (DIVMAX>>ndiv_counter)
	sra	r24,r12,1			;r24 = temp2 (temp1>>1)
	move	r14,r0				;r14 = myst1
	move	r13,r0				;r13 = myst2
	move	r10,r0				;r10 = myst3
	sll	r2,r12,2			;r2 = temp1*4
	addu	r25,r2,r12			;r25 = (DIVMAX+1)*temp1
.loop2
	move	r15,r13				;myst4=myst2
.loop1
	addu	r3,r15,r10			;r3 = (myst4+myst3)
	sll	r2,r3,4				;r2 = (myst4+myst3)<<4
	addu	r9,r11,r2			;source_pol1 = spol+((myst4+myst3)<<4)
	addu	r2,r3,r12			;r2 = (myst4+myst3)+temp1
	sll	r2,r2,4				;r2 = ((myst4+myst3)+temp1)<<4
	addu	r8,r11,r2			;source_pol2 = spol+(((myst4+myst3)+temp1)<<4)
	addu	r3,r3,r24			;r3 = (myst4+myst3)+temp2
	sll	r3,r3,4				;r3 = ((myst4+myst3)+temp2)<<4

;r9 = source_pol1
;r8 = source_pol2

	lh	r2,SPOL_x(r9)			;r2 = source_pol1->xy[0]
	lh	r4,SPOL_x(r8)			;r4 = source_pol2->xy[0]
	addu	r7,r11,r3			;dest_pol = spol+(((myst4+myst3)+temp2)<<4)
	lh	r3,SPOL_y(r9)			;r3 = source_pol1->xy[1]
;r7 = dest_pol
	addu	r2,r2,r4			;r2 = source_pol1->xy[0]+source_pol2->xy[0]
	lh	r4,SPOL_y(r8)			;r4 = source_pol2->xy[1]
	sra	r2,r2,1				;r2 = (source_pol1->xy[0]+source_pol2->xy[0])>>1
	addu	r3,r3,r4			;r3 = source_pol1->xy[1]+source_pol2->xy[1]
	sra	r3,r3,1				;r3 = (source_pol1->xy[1]+source_pol2->xy[1])>>1
	sh	r2,SPOL_x(r7)			;dest_pol->xy[0] = (source_pol1->xy[0]+source_pol2->xy[0])>>1
	sh	r3,SPOL_y(r7)			;dest_pol->xy[1] = ((source_pol1->xy[1]+source_pol2->xy[1])>>1)

	lh	r2,SPOL_u(r9)			;r2 = source_pol1->uv[0]
	lh	r4,SPOL_u(r8)			;r3 = source_pol2->uv[0]
	lh	r3,SPOL_v(r9)			;r3 = source_pol1->uv[1]
	addu	r2,r2,r4			;r2 = source_pol1->uv[0]+source_pol2->uv[0]
	lh	r4,SPOL_v(r8)			;r4 = source_pol2->uv[1]
	sra	r2,r2,1				;r2 = (source_pol1->uv[0]+source_pol2->uv[0])>>1
	addu	r3,r3,r4			;r3 = source_pol1->uv[1]+source_pol2->uv[1]
	sra	r3,r3,1				;r3 = (source_pol1->uv[1]+source_pol2->uv[1])>>1
	sh	r2,SPOL_u(r7)			;dest_pol->uv[0] = ((source_pol1->uv[0]+source_pol2->uv[0])>>1)
	sh	r3,SPOL_v(r7)			;dest_pol->uv[1] = ((source_pol1->uv[1]+source_pol2->uv[1])>>1)

	lw	r2,SPOL_rgb(r9)			;r2 = source_pol1->rgb[0]
	lw	r3,SPOL_rgb(r8)			;r3 = source_pol2->rgb[0]
	addu	r10,r10,r12			;myst3 = (myst3+temp1)

;	addu	r2,r2,r3			;r2 = source_pol1->rgb[0]+source_pol2->rgb[0]
;	sra	r2,r2,1				;r2 = (source_pol1->rgb[0]+source_pol2->rgb[0])>>1

	srl	r2,r2,1
	srl	r3,r3,1
	xor	r4,r2,r3
	addu	r2,r2,r3
	lui	r3,$0080
	ori	r3,r3,$8080
	and	r4,r4,r3
	subu	r2,r2,r4

	opt at-
	slt	at,r10,DIVMAX			;myst3<DIVMAX ??
	bne	at,r0,.loop1			;myst3<DIVMAX ??
	sw	r2,SPOL_rgb(r7)			;dest_pol->rgb[0] = ((source_pol1->rgb[0]+source_pol2->rgb[0])>>1)
	opt at+

	addu	r13,r13,r25			;myst2 = myst2+((DIVMAX+1)*temp1)
	addu	r14,r14,r12			;myst1 = myst1+temp1
	slt	r2,r14,(DIVMAX+1)		;myst1 < (DIVMAX+1) ??
	bne	r2,r0,.loop2			;myst1 < (DIVMAX+1) ??
	move	r10,r0				;myst3 = 0 (check if this is needed (see the c) )



; second pair of loops...

	move	r14,r0				;myst1 = 0
	move	r13,r0				;myst2 = 0
	sll	r2,r12,2			;r2 = temp1<<2
	addu	r25,r2,r12			;r25 = temp1*(DIVMAX+1)
	sll	r2,r24,2			;r2 = temp2<<2
	addu	r5,r2,r24			;r5 = temp2*(DIVMAX+1)
.loop4
	move	r15,r13				;myst4 = myst2
.loop3
	addu	r3,r15,r10			;r3 = myst4+myst3
	sll	r2,r3,4				;r2 = (myst4+myst3)<<4
	addu	r9,r11,r2			;r9 = spol+((myst4+myst3)<<4)
;r9 = source_pol1
	addu	r2,r25,r3			;r2 = (temp1*(DIVMAX+1))+(myst4+myst3)
	sll	r2,r2,4				;r2 = ((temp1*(DIVMAX+1))+(myst4+myst3)) <<4
	addu	r8,r11,r2			;r8 = spol+(((temp1*(DIVMAX+1))+(myst4+myst3))<<4)
;r8 = source_pol2
	addu	r3,r5,r3			;r3 = (temp2*(DIVMAX+1))+(myst4+myst3)
	sll	r3,r3,4				;r3 = ((temp2*(DIVMAX+1))+(myst4+myst3)) <<4
	lh	r2,SPOL_x(r9)			;r2 = source_pol1->xy[0]
	lh	r4,SPOL_x(r8)			;r4 = source_pol2->xy[0]
	addu	r7,r11,r3			;r7 = spol+(((temp2*(DIVMAX+1))+(myst4+myst3))<<4)
;r7 = dest_pol
	lh	r3,SPOL_y(r9)			;r3 = source_pol1->xy[1]
	addu	r2,r2,r4			;r2 = source_pol1->xy[0]+source_pol2->xy[0]
	lh	r4,SPOL_y(r8)			;r4 = source_pol2->xy[1]
	sra	r2,r2,1				;r2 = (source_pol1->xy[0]+source_pol2->xy[0])>>1
	addu	r3,r3,r4			;r3 = source_pol1->xy[1]+source_pol2->xy[1]
	sra	r3,r3,1				;r3 = (source_pol1->xy[1]+source_pol2->xy[1])>>1
	sh	r2,SPOL_x(r7)			;dest_pol->xy[0] = ((source_pol1->->xy[0]+source_pol2->->xy[0])>>1)
	sh	r3,SPOL_y(r7)			;dest_pol->xy[0] = ((source_pol1->xy[1]+source_pol2->xy[1])>>1)

	lh	r2,SPOL_u(r9)			;r2 = source_pol1->uv[0]
	lh	r4,SPOL_u(r8)			;r4 = source_pol2->uv[0]
	lh	r3,SPOL_v(r9)			;r3 = source_pol1->uv[1]
	addu	r2,r2,r4			;r2 = source_pol1->uv[0]+source_pol2->uv[0]
	lh	r4,SPOL_v(r8)			;r4 = source_pol2->uv[1]
	sra	r2,r2,1				;r2 = (source_pol1->uv[0]+source_pol2->uv[0])>>1
	addu	r3,r3,r4			;r3 = source_pol1->uv[1]+source_pol2->uv[1]
	sra	r3,r3,1				;r3 = (source_pol1->uv[1]+source_pol2->uv[1])>>1
	sh	r2,SPOL_u(r7)			;dest_pol->uv[0] = ((source_pol1->uv[0]+source_pol2->uv[0])>>1)
	sh	r3,SPOL_v(r7)			;dest_pol->uv[1] = ((source_pol1->uv[1]+source_pol2->uv[1])>>1)

	lw	r2,SPOL_rgb(r9)			;r2 = source_pol1->rgb[0]
	lw	r3,SPOL_rgb(r8)			;r3 = source_pol2->rgb[0]
	addu	r10,r10,r12			;myst3=(myst3+temp1)

;	addu	r2,r2,r3			;r2 = source_pol1->rgb[0]+source_pol2->rgb[0]
;	sra	r2,r2,1				;r2 = (source_pol1->rgb[0]+source_pol2->rgb[0])>>1

	srl	r2,r2,1
	srl	r3,r3,1
	xor	r4,r2,r3
	addu	r2,r2,r3
	lui	r3,$0080
	ori	r3,r3,$8080
	and	r4,r4,r3
	subu	r2,r2,r4

	opt at-
	slt	at,r10,(DIVMAX+1)		;myst3<DIVMAX+1 ??
	bne	at,r0,.loop3			;myst3<DIVMAX+1 ??
	sw	r2,SPOL_rgb(r7)			;dest_pol->rgb[0] = ((source_pol1->rgb[0]+source_pol2->rgb[0])>>1)
	opt at+

	addu	r13,r13,r25			;myst2=myst2+(temp1*(DIVMAX+1))
	addu	r14,r14,r12			;myst1=myst1+temp1
	slt	r2,r14,DIVMAX			;myst1<DIVMAX ??
	bne	r2,r0,.loop4
	move	r10,r0				;myst3 = 0


;final pair of loops coming up!
	move	r14,r0				;myst1 = 0
	move	r13,r0				;myst2 = 0
	sll	r2,r12,2			;r2 = temp1<<2
	addu	r25,r2,r12			;r25 = temp1*(DIVMAX+1)
	sll	r2,r24,2			;r2 = temp2<<2
	addu	r2,r2,r24			;r2 = temp2*(DIVMAX+1)
	sll	r6,r2,4				;r6 = (temp2*(DIVMAX+1))<<4
.loop6
	move	r10,r0				;myst3 = 0
	move	r15,r13				;myst4=myst2
	addu	r5,r11,r6			;r5 = spol+((temp2*(DIVMAX+1))<<4)
.loop5
	addu	r4,r15,r10			;r4 = myst4+myst3
	sll	r3,r4,4				;r3 = (myst4+myst3)<<4
	addu	r3,r11,r3			;r3 = spol+((myst4+myst3)<<4)
	sll	r2,r24,4			;r2 = temp2<<4
	addu	r9,r3,r2			;r9 = spol+((myst4+myst3)<<4)+(temp2<<4)

;r9 = source_pol1
	addu	r2,r25,r4			;r2 = (temp1*(DIVMAX+1))+(myst4+myst3)
	addu	r2,r2,r24			;r2 = ((temp1*(DIVMAX+1))+(myst4+myst3))+temp2
	sll	r2,r2,4				;r2 = (((temp1*(DIVMAX+1))+(myst4+myst3))+temp2)<<4
	addu	r8,r11,r2			;r8 = spol+((((temp1*(DIVMAX+1))+(myst4+myst3))+temp2)<<4)
	addu	r4,r4,r24			;r4 = (myst4+myst3)+temp2
	sll	r4,r4,4				;r4 = ((myst4+myst3)+temp2)<<4
	lh	r2,SPOL_x(r9)			;r2 = source_pol1->xy[0]
	lh	r3,SPOL_x(r8)			;r3 = source_pol2->xy[0]
	addu	r7,r5,r4			;r7 = spol+((temp2*(DIVMAX+1))<<4)+(((myst4+myst3)+temp2)<<4)
;r7 = dest_pol
	addu	r2,r2,r3			;r2 = source_pol1->xy[0]+source_pol2->xy[0]
	lh	r3,SPOL_y(r9)			;r3 = source_pol1->xy[1]
	lh	r4,SPOL_y(r8)			;r4 = source_pol2->xy[1]
	sra	r2,r2,1				;r2 = (source_pol1->xy[0]+source_pol2->xy[0])>>1
	addu	r3,r3,r4			;r3 = source_pol1->xy[1]+source_pol2->xy[1]
	sra	r3,r3,1				;r3 = (source_pol1->xy[1]+source_pol2->xy[1])>>1
	sh	r2,SPOL_x(r7)			;dest_pol->xy[0] = (source_pol1->xy[0]+source_pol2->xy[0])>>1
	sh	r3,SPOL_y(r7)			;dest_pol->xy[1] = (source_pol1->xy[1]+source_pol2->xy[1])>>1

	lh	r2,SPOL_u(r9)			;r2 = source_pol1->uv[0]
	lh	r4,SPOL_u(r8)			;r4 = source_pol2->uv[0]
	lh	r3,SPOL_v(r9)			;r3 = source_pol1->uv[1]
	addu	r2,r2,r4			;r2 = source_pol1->uv[0]+source_pol2->uv[0]
	lh	r4,SPOL_v(r8)			;r4 = source_pol2->uv[1]
	sra	r2,r2,1				;r2 = (source_pol1->uv[0]+source_pol2->uv[0])>>1
	addu	r3,r3,r4			;r3 = source_pol1->uv[1]+source_pol2->uv[1]
	sra	r3,r3,1				;r3 = (source_pol1->uv[1]+source_pol2->uv[1])>>1
	sh	r2,SPOL_u(r7)			;dest_pol->uv[0] = (source_pol1->uv[0]+source_pol2->uv[0])>>1
	sh	r3,SPOL_v(r7)			;dest_pol->uv[1] = ((source_pol1->uv[1]+source_pol2->uv[1])>>1)

	lw	r2,SPOL_rgb(r9)			;r2 = source_pol1->rgb[0]
	lw	r3,SPOL_rgb(r8)			;r3 = source_pol2->rgb[0]
	addu	r10,r10,r12			;myst3=myst3+temp1
;	addu	r2,r2,r3			;r2 = source_pol1->rgb[0]+source_pol2->rgb[0]
;	sra	r2,r2,1				;r2 = (source_pol1->rgb[0]+source_pol2->rgb[0])>>1

	srl	r2,r2,1
	srl	r3,r3,1
	xor	r4,r2,r3
	addu	r2,r2,r3
	lui	r3,$0080
	ori	r3,r3,$8080
	and	r4,r4,r3
	subu	r2,r2,r4


	opt at-
	slt	at,r10,DIVMAX			;myst3 < DIVMAX ??
	bne	at,r0,.loop5			;myst3 < DIVMAX ??
	sw	r2,SPOL_rgb(r7)			;dest_pol->rgb[0] = ((source_pol1->rgb[0]+source_pol2->rgb[0])>>1)
	opt at+

	addu	r14,r14,r12			;myst1=myst1+temp1
	slt	r2,r14,DIVMAX			;myst1<DIVMAX ??

	bne	r2,r0,.loop6			;myst1<DIVMAX ??
	addu	r13,r13,r25			;myst2 = myst2+(temp1*(DIVMAX+1))

	addu	r17,r17,1			;ndiv_counter++
	slt	r2,r17,r18			;ndiv_counter < ndiv ??

	bne	r2,r0,.main_ndiv_loop		;ndiv_counter < ndiv ??
	li	r2,DIVMAX			;r2 = DIVMAX (4)




;r11 = spol
;r16 = original_poly
;r18 = ndiv

.make_the_polys
	move	r14,r11				;r14 = spol
	li	r3,DIVMAX			;r3 = DIVMAX
	sra	r3,r3,r18			;step = DIVMAX>>ndiv
	sll	r7,r3,4				;r7 = step<<4
	addu	r25,r14,r7			;r25 = spol+step<<4
	sll	r4,r3,2				;r4 = step<<2
	addu	r4,r4,r3			;r4 = step*(DIVMAX+1)
	sll	r2,r4,4				;r2 = step*(DIVMAX+1)<<4
	addu	r24,r14,r2			;r14 = spol+(step*(DIVMAX+1)<<4)
	addu	r2,r4,r3			;r2 = (step*(DIVMAX+1))+DIVMAX
	sll	r2,r2,4				;r2 = ((step*(DIVMAX+1))+DIVMAX)<<4
	addu	r15,r14,r2			;r15 = spol+(((step*(DIVMAX+1))+DIVMAX)<<4)

	move	r13,r16				;r13 = original_poly
	move	r5,r0				;j=0
	move	r16,r0				;primskip = 0
	sll	r3,r3,r18			;r3 = step<<ndiv
	li	r2,1				;r2 = 1
	sll	r6,r2,r18			;slag = (1<<ndiv)

;
;r6  = slag
;r13 = original_poly
;r14 = spol1
;r25 = spol2
;r24 = spol3
;r15 = spol4
;
	blez	r6,.exitme			;slag<=0 ??
	subu	r18,r4,r3			;r18 = (step*(DIVMAX+1))-(step<<ndiv)

	lbu	r11,POLYGT4_code(r13)		;r11 = original_poly->code
	lhu	r10,POLYGT4_tpage(r13) 		;r10 = original_poly->tpage
	lhu	r9,POLYGT4_clut(r13)   		;r9 = original_poly->clut

	li	r17,$00ff0000
	ori	r17,r17,$ffff			;r17 = $00ffffff
	move	r12,r7				;r12 = step<<4
.jloop
	blez	r6,.next_poly			;slag<= 0 ??
	move	r4,r0				;k=0

.kloop

;top left corner

	lw	r7,SPOL_rgb(r14)		;r7 = spol1->rgb[0]
	lw	r8,SPOL_x(r14)			;r8 = spol1->xy[0] and spol1->xy[1]
;	sll	r3,r7,16			;shift r
;	sll	r2,r7,8				;shift r
;	or	r7,r7,r3			;make rgb long
	lw	r3,SPOL_u(r14)			;r2 = spol1->uv[0] and spol1->uv[1]
;	or	r7,r7,r2			;make rgb long
	sw	r8,POLYGT4_x0(r13)		;stash x0 and y0
	sb	r3,POLYGT4_u0(r13)		;stash u0
	srl	r2,r3,16			;shift uv
	sb	r2,POLYGT4_v0(r13)		;stash v0
	sw	r7,POLYGT4_r0(r13)		;stash rgb0

; top right corner

	lw	r7,SPOL_rgb(r25)		;r7 = spol1->rgb[0]
	lw	r8,SPOL_x(r25)			;r8 = spol1->xy[0] and spol1->xy[1]
;	sll	r3,r7,16			;shift r
;	sll	r2,r7,8				;shift r
;	or	r7,r7,r3			;make rgb long
	lw	r3,SPOL_u(r25)			;r2 = spol1->uv[0] and spol1->uv[1]
;	or	r7,r7,r2			;make rgb long
	sw	r8,POLYGT4_x1(r13)		;stash x1 and y1
	sb	r3,POLYGT4_u1(r13) 		;stash u1
	srl	r2,r3,16			;shift uv
	sb	r2,POLYGT4_v1(r13) 		;stash v1
	sw	r7,POLYGT4_r1(r13)  		;stash rgb1

;bottom left corner

	lw	r7,SPOL_rgb(r24)		;r7 = spol1->rgb[0]
	lw	r8,SPOL_x(r24)			;r8 = spol1->xy[0] and spol1->xy[1]
;	sll	r3,r7,16			;shift r
;	sll	r2,r7,8				;shift r
;	or	r7,r7,r3			;make rgb long
	lw	r3,SPOL_u(r24)			;r2 = spol1->uv[0] and spol1->uv[1]
;	or	r7,r7,r2			;make rgb long
	sw	r8,POLYGT4_x2(r13) 		;stash x2 and y2
	sb	r3,POLYGT4_u2(r13)		;stash u2
	srl	r2,r3,16			;shift uv
	sb	r2,POLYGT4_v2(r13)  		;stash v2
	sw	r7,POLYGT4_r2(r13) 		;stash rgb2

;bottom right corner

	lw	r7,SPOL_rgb(r15)		;r7 = spol1->rgb[0]
	lw	r8,SPOL_x(r15)			;r8 = spol1->xy[0] and spol1->xy[1]
;	sll	r3,r7,16			;shift r
;	sll	r2,r7,8				;shift r
;	or	r7,r7,r3			;make rgb long
	lw	r3,SPOL_u(r15)			;r2 = spol1->uv[0] and spol1->uv[1]
;	or	r7,r7,r2			;make rgb long
	sw	r8,POLYGT4_x3(r13)  		;stash x3 and y3
	sb	r3,POLYGT4_u3(r13) 		;stash u3
	srl	r2,r3,16			;shift uv
	sb	r2,POLYGT4_v3(r13)		;stash v3
	sw	r7,POLYGT4_r3(r13) 		;stash rgb3

	addu	r14,r14,r12			;spol1=spol1+(step<<4)
	addu	r25,r25,r12			;spol2=spol2+(step<<4)
	addu	r24,r24,r12			;spol3=spol3+(step<<4)
	addu	r15,r15,r12			;spol4=spol4+(step<<4)

	sb	r11,POLYGT4_code(r13)		;stash poly code
	sh	r9,POLYGT4_clut(r13)		;stash clut (CHANGE)
	sh	r10,POLYGT4_tpage(r13) 		;stash tpage

	addu	r4,r4,1				;k++
	addu	r16,r16,(POLYGT4_size)/4	;primskip+=13

;r13 = poly
;r19 = ot
;this is the inline addprim

	lw	r3,0(r19)			;r3 = 0(ot)
	lui	r2,(POLYGT4_PRIMLEN<<8)		;r2 = TAG (length = 12)
	and	r3,r3,r17			;r3 = 0(ot)&$00ffffff
	or	r2,r2,r3			;r2 = (TAG&$ff000000)|0(ot)&$00ffffff
	and	r3,r13,r17			;r3 = poly address&$00ffffff
	sw	r2,POLYGT4_tag(r13)		;stash (TAG&$ff000000)|0(ot)&$00ffffff
	sw	r3,0(r19)			;stash poly address&$00ffffff

	slt	r2,r4,r6			;k<slag??
	bne	r2,r0,.kloop			;k<slag??
	addu	r13,r13,POLYGT4_size		;original_poly++

.next_poly
	sll	r2,r18,4
	addu	r14,r14,r2
	addu	r25,r25,r2
	addu	r24,r24,r2
	addu	r5,r5,1				;j++
	opt at-
	slt	at,r5,r6			;j<slag??
	bne	at,r0,.jloop
	addu	r15,r15,r2
	opt at+

.exitme
	move	r2,r16				;r2 = primskip (the value I return)

	lui	r11,scratchpad_base		;r11 = scratchpad
	lw	r16,regs_offset(r11)	 	;restore r16
	lw	r17,regs_offset+4(r11)	 	;restore r17
	lw	r18,regs_offset+8(r11)	 	;restore r18
	j	r31
	lw	r19,regs_offset+12(r11)	 	;restore r19





; enter with:-
;
; r4 = POLY_GT3 *cur_primlist (ie the poly you just made)
; r5 = u_long *OT (address of order table entry for this poly)
; r6 = ndiv (1,2)
;
; returns:-
;
; r2 = number of longs to add to your primlist...
;
; SWEETLY optimized! (no stacking, uses scratchpad)
;

DEL_subdivide_gt3_asm

	lui	r14,scratchpad_base
	sw	r23,regs_offset(r14)	     	;stash r23
	sw	r22,regs_offset+4(r14)	     	;stash r22
	sw	r21,regs_offset+8(r14)	     	;stash r21
	sw	r20,regs_offset+12(r14)	     	;stash r20
	sw	r19,regs_offset+16(r14)	     	;stash r19
	sw	r18,regs_offset+20(r14)	     	;stash r18
	sw	r17,regs_offset+24(r14)	     	;stash r17
	sw	r16,regs_offset+28(r14)	     	;stash r16

; setup top-left co-ords

	move	r16,r4		     	;r16 = original_poly

	lw	r2,POLYGT3_x0(r16)	;r2 = original_poly->x0 and original_poly->y0
	lbu	r3,POLYGT3_u0(r16)	;r2 = original_poly->u0

	move	r21,r5			;r21 = ot_me

	lbu	r4,POLYGT3_v0(r16)	;r2 = original_poly->v0
	lw	r5,POLYGT3_r0(r16)  	;original_poly->r0

	sw	r2,SPOL_x(r14)		;spol->xy[0] = original_poly->x0 & spol->xy[1] = original_poly->y0
	sh	r3,SPOL_u(r14)		;spol->uv[0] = original_poly->u0
	sh	r4,SPOL_v(r14)		;spol->uv[1]= original_poly->v0
	sw	r5,SPOL_rgb(r14)	;spol->rgb[0] = original_poly->r0

; setup top-right co-ords

	lw	r2,POLYGT3_x1(r16)	;r2 = original_poly->x1 and original_poly->y1
	lbu	r3,POLYGT3_u1(r16)  	;r2 = original_poly->u1
	lbu	r4,POLYGT3_v1(r16)   	;r2 = original_poly->v1
	lw	r5,POLYGT3_r1(r16)   	;r2 = original_poly->r1

	li	r20,DIVMAX		;DIVMAX

	sw	r2,((DIVMAX)*SPOL_size)+SPOL_x(r14)	;(spol+DIVMAX2)->xy[0] = original_poly->x1
	sh	r3,((DIVMAX)*SPOL_size)+SPOL_u(r14)	;(spol+DIVMAX2)->uv[0] = original_poly->u1
	sh	r4,((DIVMAX)*SPOL_size)+SPOL_v(r14)	;(spol+DIVMAX2)->uv[1] = original_poly->v1
	sw	r5,((DIVMAX)*SPOL_size)+SPOL_rgb(r14)	;(spol+DIVMAX2)->rgb[0] = original_poly->r1

; setup bottom-left co-ords

	lw	r2,POLYGT3_x2(r16)	;r2 = original_poly->x2 and original_poly->y2
	lbu	r3,POLYGT3_u2(r16)  	;r2 = original_poly->u2
	lbu	r4,POLYGT3_v2(r16) 	;r2 = original_poly->v2
	lw	r5,POLYGT3_r2(r16)	;r2 = original_poly->r2

	move	r22,r6			;r22 = ndiv

	sw	r2,((DIVMAX*(DIVMAX+1))*SPOL_size)+SPOL_x(r14) 	;(spol+(DIVMAX2*(DIVMAX2+1)))->xy[0] = original_poly->x2
	sh	r3,((DIVMAX*(DIVMAX+1))*SPOL_size)+SPOL_u(r14) 	;(spol+(DIVMAX2*(DIVMAX2+1)))->uv[0] = original_poly->u2
	sh	r4,((DIVMAX*(DIVMAX+1))*SPOL_size)+SPOL_v(r14) 	;(spol+(DIVMAX2*(DIVMAX2+1)))->uv[1] = original_poly->v2
	sw	r5,((DIVMAX*(DIVMAX+1))*SPOL_size)+SPOL_rgb(r14) 	;(spol+(DIVMAX2*(DIVMAX2+1)))->rgb[0] = original_poly->r2

	blez	r22,.make_the_polys
	move	r19,r0			;ndiv_counter = 0

;
;r20 = DIVMAX
;r22 = ndiv
;r21 = ot_me
;r19 = ndiv_counter
;r16 = original_poly
;r14 = spol
;
;

.ndiv_loop
	sra	r13,r20,r19		;temp1 = DIVMAX>>ndiv_counter
	sra	r25,r13,1		;temp2 = temp1>>1
	move	r11,r0			;myst1 = 0
	move	r12,r0			;myst2 = 0
	sll	r2,r13,2		;r2 = temp1<<2
	addu	r17,r2,r13		;r17 = temp1*(DIVMAX+1)
.loop2
	subu	r2,r20,r11		;r2 = DIVMAX-myst1
	blez	r2,.skip_loop1		;(DIVMAX-myst1)<=0 ??

;	nop	;don`t need this really... :)

	move	r10,r0			;myst4 = 0
	move	r24,r12			;myst3 = myst2
	move	r15,r2			;delme1 = DIVMAX-myst1
	addu	r3,r24,r10		;r3 = myst3+myst4
.loop1
	sll	r2,r3,4			;r3 = (myst3+myst4)<<4
	addu	r7,r14,r2		;r7 = spol+((myst3+myst4)<<4)
;r7 = source_pol1

	addu	r2,r3,r13		;r2 = (myst3+myst4)+temp1
	sll	r2,r2,4			;r2 = ((myst3+myst4)+temp1)<<4
	addu	r8,r14,r2		;r8 = spol+(((myst3+myst4)+temp1)<<4)
;r8 = source_pol2

	addu	r3,r3,r25		;r3 = (myst3+myst4)+temp2
	sll	r3,r3,4			;r3 = ((myst3+myst4)+temp2)<<4

	lh	r2,SPOL_x(r7)		;r2 = source_pol1->xy[0]
	lh	r4,SPOL_x(r8)		;r4 = source_pol2->xy[0]
	addu	r9,r14,r3		;r9 = spol+(((myst3+myst4)+temp2)<<4)
;r9 = destpol

	addu	r2,r2,r4		;r2 = source_pol1->xy[0]+source_pol2->xy[0]

	lh	r3,SPOL_y(r7)		;r3 = source_pol1->xy[1]
	lh	r4,SPOL_y(r8)		;r4 = source_pol2->xy[1]
	sra	r2,r2,1			;r2 = (source_pol1->xy[0]+source_pol2->xy[0])>>1
	addu	r3,r3,r4		;r3 = source_pol1->xy[1]+source_pol2->xy[1]
	sra	r3,r3,1			;r3 = (source_pol1->xy[1]+source_pol2->xy[1])>>1
	sh	r2,SPOL_x(r9)		;dest_pol->xy[0] = ((source_pol1->xy[0]+source_pol2->xy[0])>>1)
	sh	r3,SPOL_y(r9)		;dest_pol->xy[1] = (source_pol1->xy[1]+source_pol2->xy[1])>>1

	lh	r2,SPOL_u(r7)		;r2 = source_pol1->uv[0]
	lh	r4,SPOL_u(r8)		;r3 = source_pol2->uv[0]
	lh	r3,SPOL_v(r7)		;r3 = source_pol1->uv[1]
	addu	r2,r2,r4		;r2 = source_pol1->uv[0]+source_pol2->uv[0]
	lh	r4,SPOL_v(r8)		;r4 = source_pol2->uv[1]
	sra	r2,r2,1			;r2 = (source_pol1->uv[0]+source_pol2->uv[0])>>1
	addu	r3,r3,r4		;r3 = source_pol1->uv[1]+source_pol2->uv[1]
	sra	r3,r3,1			;r3 = (source_pol1->uv[1]+source_pol2->uv[1])>>1
	sh	r2,SPOL_u(r9)		;dest_pol->uv[0]
	sh	r3,SPOL_v(r9)		;dest_pol->uv[1] = ((source_pol1->uv[1]+source_pol2->uv[1])>>1)

	lw	r2,SPOL_rgb(r7)		;r2 = source_pol1->rgb[0]
	lw	r3,SPOL_rgb(r8)		;r3 = source_pol2->rgb[0]
	addu	r10,r10,r13		;myst4=myst4+temp1
;	addu	r2,r2,r3		;r2 = source_pol1->rgb[0]+source_pol2->rgb[0]
;	sra	r2,r2,1			;r2 = (source_pol1->rgb[0]+source_pol2->rgb[0])>>1

	srl	r2,r2,1
	srl	r3,r3,1
	xor	r4,r2,r3
	addu	r2,r2,r3
	lui	r3,$0080
	ori	r3,r3,$8080
	and	r4,r4,r3
	subu	r2,r2,r4

	sw	r2,SPOL_rgb(r9)		;dest_pol->rgb[0] = ((source_pol1->rgb[0]+source_pol2->rgb[0])>>1)
	slt	r2,r10,r15		;myst4<delme1 ??
	bne	r2,r0,.loop1		;myst4<delme1 ??
	addu	r3,r24,r10		;r3 = (myst3+myst4)


.skip_loop1
	addu	r11,r11,r13		;myst1=myst1+temp1
	slt	r2,r11,(DIVMAX+1)	;myst1<(DIVMAX+1) ??
	bne	r2,r0,.loop2		;myst1<(DIVMAX+1) ??
	addu	r12,r12,r17		;myst2=myst2+(temp1*(DIVMAX+1))


; second pair of loops coming your way...

	opt at-

	move	r10,r0			;myst4 =0
	sll	r2,r13,2		;r2 = temp1<<2
	addu	r4,r2,r13		;r4 = temp1*(DIVMAX+1)
	sll	r2,r25,2		;r2 = temp2<<2
	addu	r2,r2,r25		;r2 = temp2*(DIVMAX+1)
	sll	r24,r2,4		;r24 = (temp2*(DIVMAX+1))<<4
	subu	r2,r20,r10		;r2 = DIVMAX-myst4
.loop4
	blez	r2,.skip_loop3		;DIVMAX-myst4 <= 0 ??
;	nop	;don`t need this really... :)

	move	r11,r0			;myst1=0
	move	r15,r2			;delme1=(DIVMAX2-myst4)
	move	r12,r0			;myst2=0
.loop3
	addu	r2,r12,r10		;r2 = myst2+myst4
	sll	r2,r2,4			;r2 = (myst2+myst4)<<4
	addu	r7,r14,r2		;r7 = spol+((myst2+myst4)<<4)
;r7 = source_pol1

	sll	r2,r4,4			;r2 = (temp1*(DIVMAX+1))<<4
	addu	r8,r7,r2		;r8 = spol+((myst2+myst4)<<4)+((temp1*(DIVMAX+1))<<4)
;r8 = source_pol2

	lh	at,SPOL_x(r7)		;at = source_pol1->xy[0]
	lh	r2,SPOL_x(r8)		;r2 = source_pol2->xy[0]
	lh	r3,SPOL_y(r8)		;r3 = source_pol2->xy[1]
	addu	r9,r7,r24		;r9 = spol+((myst2+myst4)<<4)+((temp2*(DIVMAX+1))<<4)
;r9 = dest_pol
	addu	at,at,r2		;at = source_pol1->xy[0]+source_pol2->xy[0]
	lh	r2,SPOL_y(r7)		;r2 = source_pol1->xy[1]
	sra	at,at,1			;at = (source_pol1->xy[0]+source_pol2->xy[0])>>1
	addu	r2,r2,r3		;r2 = source_pol1->xy[1]+source_pol2->xy[1]
	sra	r2,r2,1			;r2 = (source_pol1->xy[1]+source_pol2->xy[1])>>1
	sh	at,SPOL_x(r9)		;dest_pol->xy[0] = ((source_pol1->xy[0]+source_pol2->xy[0])>>1)
	sh	r2,SPOL_y(r9)		;dest_pol->xy[1] = ((source_pol1->xy[1]+source_pol2->xy[1])>>1)


	lh	at,SPOL_u(r7)		;at = source_pol1->uv[0]
	lh	r2,SPOL_u(r8)		;r3 = source_pol2->uv[0]
	lh	r3,SPOL_v(r8)		;r2 = source_pol2->uv[1]
	addu	at,at,r2		;at = source_pol1->uv[0]+source_pol2->uv[0]
	lh	r2,SPOL_v(r7)		;r2 = source_pol1->uv[1]
	sra	at,at,1			;at = (source_pol1->uv[0]+source_pol2->uv[0])>>1
	addu	r2,r2,r3		;r2 = source_pol1->uv[1]+source_pol2->uv[1]
	sra	r2,r2,1			;r2 = (source_pol1->uv[1]+source_pol2->uv[1])>>1
	sh	at,SPOL_u(r9)		;dest_pol->uv[0] = ((source_pol1->uv[0]+source_pol2->uv[0])>>1)
	sh	r2,SPOL_v(r9)		;dest_pol->uv[1] = ((source_pol1->uv[1]+source_pol2->uv[1])>>1)

	lw	r2,SPOL_rgb(r7)		;r2 = source_pol1->rgb[0]
	lw	r3,SPOL_rgb(r8)		;r3 = source_pol2->rgb[0]
	addu	r11,r11,r13		;myst1=myst1+temp1
;	addu	r2,r2,r3		;r2 = source_pol1->rgb[0]+source_pol2->rgb[0]
;	sra	r2,r2,1			;r2 = (source_pol1->rgb[0]+source_pol2->rgb[0])>>1

	srl	r2,r2,1
	srl	r3,r3,1
	xor	at,r2,r3
	addu	r2,r2,r3
	lui	r3,$0080
	ori	r3,r3,$8080
	and	at,at,r3
	subu	r2,r2,at

	sw	r2,SPOL_rgb(r9)		;dest_pol->((source_pol1->rgb[0]+source_pol2->rgb[0])>>1)

	slt	r2,r11,r15		;myst1<delme1
	bne	r2,r0,.loop3		;myst1<delme1
	addu	r12,r12,r4		;myst2=myst2+(temp1*(DIVMAX+1))

.skip_loop3
	addu	r10,r10,r13		;myst4=(myst4+temp1)
	slt	r2,r10,(DIVMAX+1)	;myst4<(DIVMAX+1) ??
	bne	r2,r0,.loop4		;myst4<(DIVMAX+1) ??
	subu	r2,r20,r10		;r2 = DIVMAX-myst4

	opt at+

; ok, here goes the final 2 loops...

	move	r11,r0			;myst1=0
	move	r12,r0			;myst2=0
	sll	r2,r13,2		;r2 = temp1<<2
	addu	r6,r2,r13		;r6 = temp1*(DIVMAX+1)
	sll	r17,r6,4		;r17 = (temp1*(DIVMAX+1))<<4
	sll	r2,r25,1		;r2 = temp2<<1
	addu	r18,r2,r25		;r18 = temp2*3
.loop6
	subu	r2,r20,r11		;r2 = DIVMAX-myst1
	blez	r2,.skiploop5		;(DIVMAX-myst1) <= 0 ??
;	nop	;don`t need this really... :)

	move	r10,r0			;myst4 = 0
	move	r24,r12			;myst3=myst2
	move	r15,r2			;delme1 = (DIVMAX-myst1)
	sll	r5,r18,5		;r5 = (temp2*(DIVMAX+2))<<4
	addu	r3,r24,r10		;r3 = (myst3+myst4)
.loop5
	sll	r3,r3,4			;r3 = (myst3+myst4)<<4
	addu	r3,r14,r3		;r3 = spol+(myst3+myst4)<<4
	sll	r2,r25,5		;r2 = (temp2<<1)<<4
	addu	r7,r3,r2		;r7 = spol+((myst3+myst4)<<4)+((temp2<<1)<<4)
;r7 = source_pol1

	addu	r8,r3,r17		;r8 = spol+((myst3+myst4)<<4)+((temp1*(DIVMAX+1))<<4)
;r8 = source_pol2

	lh	r2,SPOL_x(r7)		;r2 = source_pol1->xy[0]
	lh	r4,SPOL_x(r8)		;r4 = source_pol2->xy[0]
	addu	r9,r3,r5		;r9 = spol+((myst3+myst4)<<4)+((temp2*(DIVMAX+2))<<4)
	lh	r3,SPOL_y(r7)		;r3 = source_pol1->xy[1]
;r9 = dest_pol
	addu	r2,r2,r4		;r2 = source_pol1->xy[0]+source_pol2->xy[0]
	lh	r4,SPOL_y(r8)		;r4 = source_pol2->xy[1]
	sra	r2,r2,1			;r2 = (source_pol1->xy[0]+source_pol2->xy[0])>>1
	addu	r3,r3,r4		;r3 = source_pol1->xy[1]+source_pol2->xy[1]
	sra	r3,r3,1			;r3 = (source_pol1->xy[1]+source_pol2->xy[1])>>1
	sh	r2,SPOL_x(r9)		;dest_pol->xy[0] = ((source_pol1->xy[0]+source_pol2->xy[0])>>1)
	sh	r3,SPOL_y(r9)		;dest_pol->xy[1] = ((source_pol1->xy[1]+source_pol2->xy[1])>>1)

	lh	r2,SPOL_u(r7)		;r2 = source_pol1->uv[0]
	lh	r4,SPOL_u(r8)		;r4 = source_pol2->uv[0]
	lh	r3,SPOL_v(r7)		;r3 = source_pol1->uv[1]
	addu	r2,r2,r4		;r2 = source_pol1->uv[0]+source_pol2->uv[0]
	lh	r4,SPOL_v(r8)		;r4 = source_pol2->uv[1]
	sra	r2,r2,1			;r2 = (source_pol1->uv[0]+source_pol2->uv[0])>>1
	addu	r3,r3,r4		;r3 = source_pol1->uv[1]+source_pol2->uv[1]
	sra	r3,r3,1			;r2 = (source_pol1->uv[1]+source_pol2->uv[1])>>1
	sh	r2,SPOL_u(r9)		;dest_pol->uv[0] = ((source_pol1->uv[0]+source_pol2->uv[0])>>1)
	sh	r3,SPOL_v(r9)		;dest_pol->uv[1] = ((source_pol1->uv[1]+source_pol2->uv[1])>>1)

	lw	r2,SPOL_rgb(r7)		;r2 = source_pol1->rgb[0]
	lw	r3,SPOL_rgb(r8)		;r3 = source_pol2->rgb[1]
	addu	r10,r10,r13		;myst4=myst4+temp1
;	addu	r2,r2,r3		;r2 = source_pol1->rgb[0]+source_pol2->rgb[0]
;	sra	r2,r2,1			;r2 = (source_pol1->rgb[0]+source_pol2->rgb[0])>>1

	srl	r2,r2,1
	srl	r3,r3,1
	xor	r4,r2,r3
	addu	r2,r2,r3
	lui	r3,$0080
	ori	r3,r3,$8080
	and	r4,r4,r3
	subu	r2,r2,r4

	sw	r2,SPOL_rgb(r9)		;dest_pol1->rgb[0] = ((source_pol1->rgb[0]+source_pol2->rgb[0])>>1)

	slt	r2,r10,r15		;myst4<delme1 ??
	bne	r2,r0,.loop5		;myst4<delme1 ??
	addu	r3,r24,r10		;r3 = (myst3+myst4)

.skiploop5
	addu	r11,r11,r13		;myst1=(myst1+temp1)
	slt	r2,r11,DIVMAX		;myst1<DIVMAX ??
	bne	r2,r0,.loop6		;myst1<DIVMAX ??
	addu	r12,r12,r6		;myst2=myst2+(temp1*(DIVMAX+1))

	addu	r19,r19,1		;ndiv_counter++
	slt	r2,r19,r22		;ndiv_counter<ndiv ??
	bne	r2,r0,.ndiv_loop	;ndiv_counter<ndiv ??
	nop


;r22 = ndiv
;
;
;
;
; killed R7 shitterness, R4 is now free...
; sorted loop, rgb longs, x&y longs,tpage/clut id`s from original poly...



.make_the_polys
	li	r2,1 			;r2 = 1
	sll	r24,r2,r22		;num_vertical_lines = (1<<ndiv)
	sll	r2,r24,1		;r2 = num_vertical_lines<<1
	addu	r20,r2,-1		;first_xcount = (num_vertical_lines<<1)-1
	move	r17,r14			;r17 = sspol1
;r17 = sspol1
	li	r2,DIVMAX		;r2 = DIVMAX
	sra	r6,r2,r22		;step = DIVMAX>>ndiv
	sll	r2,r6,4			;r2 = (DIVMAX>>ndiv)<<4

	addu	r19,r17,r2		;sspol2 = spol+step
;r19 = sspol2
	sll	r3,r6,2			;r3 = step<<2
	addu	r3,r3,r6		;cunt = (step*(DIVMAX+1))
	sll	r2,r3,4			;r2 = cunt<<4
	addu	r18,r17,r2		;sspol3 = spol+(cunt<<4)
;r18 = sspol3
	move	r7,r16			;r7 = polyme
	move	r5,r0			;ycount=0
	move	r14,r0			;primskip = 0
	blez	r24,.exit		;num_vertical_lines<=0??
	move	r23,r3			;r23 = cunt

	lbu	r22,POLYGT3_code(r7)  	;r22 = poly code (from original poly)
	lhu	r25,POLYGT3_tpage(r7) 	;r25 = tpage id (from original poly)
	lhu	r16,POLYGT3_clut(r7)  	;r16 = clut id (from original poly)

	li	r15,$00ff0000
	ori	r15,r15,$ffff
.yloop
	move	r12,r17			;spol1=sspol1
	move	r11,r19			;spol2=sspol2
	addu	r13,r20,-1		;xcount=(first_xcount-1)
	bltz	r13,.skip_xloop		;xcount<0 ?
	move	r10,r18			;spol3=sspol3

.xloop


	opt at-
	lw	at,SPOL_rgb(r12)	;r2 = spol1->rgb[0]
	lw	r2,SPOL_x(r12)		;r2 = spol1->xy[0] and spol1->xy[1]
;	sll	r3,at,16		;shift r
;	sll	r4,at,8			;shift r
;	or	at,at,r3		;create rgb long
;	or	at,at,r4		;create rgb long
	lbu	r3,SPOL_u(r12)		;r3 = spol1->uv[0]
	lbu	r4,SPOL_v(r12)		;r2 = spol1->uv[1]

	addu	r14,r14,(POLYGT3_size/4) ;primskip+=10

	sw	at,POLYGT3_r0(r7)	;stash rgb0 long...
	sw	r2,POLYGT3_x0(r7) 	;x0&y0
	sb	r3,POLYGT3_u0(r7)   	;u0
	sb	r4,POLYGT3_v0(r7) 	;v0
	opt at+

;

	opt at-
	lw	at,SPOL_rgb(r11)	;r2 = spol2->rgb[0]
	lw	r2,SPOL_x(r11)		;r2 = spol2->xy[0] and spol2->xy[1]
;	sll	r3,at,16		;shift r
;	sll	r4,at,8			;shift r
;	or	at,at,r3		;create rgb long
;	or	at,at,r4		;create rgb long
	lbu	r3,SPOL_u(r11)		;r3 = spol2->uv[0]
	lbu	r4,SPOL_v(r11)		;r4 = spol2->uv[1]

	move	r8,r11			;spolfill1 = spol2

	sw	at,POLYGT3_r1(r7)  	;stash rgb1 long...
	sw	r2,POLYGT3_x1(r7)  	;x1&y1
	sb	r3,POLYGT3_u1(r7)  	;u1
	sb	r4,POLYGT3_v1(r7)  	;v1
	opt at+

;
	opt at-
	lw	at,SPOL_rgb(r10)	;r2 = spol3->rgb[0]
	lw	r2,SPOL_x(r10)		;r2 = spol3->xy[0] and spol3->xy[1]
;	sll	r3,at,16		;shift r
;	sll	r4,at,8			;shift r
;	or	at,at,r3		;create rgb long
;	or	at,at,r4		;create rgb long
	lbu	r3,SPOL_u(r10)		;r3 = spol3->uv[0]
	lbu	r4,SPOL_v(r10)		;r4 = spol3->uv[1]

	move	r9,r10			;spolfill2 = spol3

	sw	at,POLYGT3_r2(r7)	;stash rgb2 long...
	sw	r2,POLYGT3_x2(r7)	;x2&y2
	sb	r3,POLYGT3_u2(r7)	;u2
	sb	r4,POLYGT3_v2(r7)	;v2
	opt at+

	sll	r3,r6,4			;r3 = step<<4

	addu	r12,r12,r3		;spol1=spol1+step

	sb	r22,POLYGT3_code(r7) 	;stash code (52)!!
	addu	r11,r11,r3		;spol2=spol2+step
	sh	r25,POLYGT3_tpage(r7)	;stash tpage
	addu	r10,r10,r3		;spol3=spol3+step
	sh	r16,POLYGT3_clut(r7)	;stash clut


;inline addprim

	lw	r3,0(r21)
	lui	r2,(POLYGT3_PRIMLEN<<8)	;r2 = TAG (length = 9)
	and	r3,r3,r15
	or	r2,r2,r3
	and	r3,r7,r15
	sw	r2,POLYGT3_tag(r7)
	addu	r7,r7,POLYGT3_size	;polyme++
	beq	r13,r0,.skip_filler	;xcount == 0??
	sw	r3,0(r21)

; if not the last poly on a line, do a filler...

	opt at-
	lw	at,SPOL_rgb(r8)		;r2 = spolfill1->rgb[0]
	lw	r2,SPOL_x(r8)		;r2 = spolfill1->xy[0] and spolfill1->xy[1]
;	sll	r3,at,16		;shift r
;	sll	r4,at,8			;shift r
;	or	at,at,r3		;create rgb long
;	or	at,at,r4		;create rgb long
	lbu	r3,SPOL_u(r8)		;r3 = spolfill1->uv[0]
	lbu	r4,SPOL_v(r8)		;r4 = spolfill1->uv[1]

	addu	r14,r14,(POLYGT3_size/4) ;primskip+=10

	sw	at,POLYGT3_r0(r7)  	;stash rgb0 long...
	sw	r2,POLYGT3_x0(r7)  	;x0&y0
	sb	r3,POLYGT3_u0(r7)  	;u0
	sb	r4,POLYGT3_v0(r7)  	;v0
	opt at+
;
	opt at-
	lw	at,SPOL_rgb(r9)		;r2 = spollfill2->rgb[0]
	lw	r2,SPOL_x(r9)		;r2 = spollfill2->xy[0] and spolfill2->xy[1]
;	sll	r3,at,16		;shift r
;	sll	r4,at,8			;shift r
;	or	at,at,r3		;create rgb long
;	or	at,at,r4		;create rgb long
	lbu	r3,SPOL_u(r9)		;r2 = spollfill2->uv[0]
	lbu	r4,SPOL_v(r9)		;r2 = spollfill2->uv[1]

	sw	at,POLYGT3_r1(r7)   	;stash rgb1 long...
	sw	r2,POLYGT3_x1(r7)   	;x1&y1
	sb	r3,POLYGT3_u1(r7)   	;u1
	sb	r4,POLYGT3_v1(r7)   	;v1
	opt at+

;
	opt at-
	lw	at,SPOL_rgb(r10)	;r2 = spol3->rgb[0]
	lw	r2,SPOL_x(r10)		;r2 = spol3->xy[0] and spol3->xy[1]
;	sll	r3,at,16		;shift r
;	sll	r4,at,8			;shift r
;	or	at,at,r3		;create rgb long
;	or	at,at,r4		;create rgb long
	lbu	r3,SPOL_u(r10)		;r2 = spol3->uv[0]
	lbu	r4,SPOL_v(r10)		;r2 = spol3->uv[0]

	sw	at,POLYGT3_r2(r7)  	;stash rgb2 long...
	sw	r2,POLYGT3_x2(r7)  	;x2&y2
	sb	r3,POLYGT3_u2(r7)  	;u2
	sb	r4,POLYGT3_v2(r7)  	;v2
	opt at+

	sb	r22,POLYGT3_code(r7)   	;stash prim code
	sh	r25,POLYGT3_tpage(r7)  	;stash TPAGE ID
	addu	r13,r13,-1		;xcount--
	sh	r16,POLYGT3_clut(r7)	;stash CLUT ID

;inline addprim
	lw	r3,0(r21)
	lui	r2,(POLYGT3_PRIMLEN<<8)	;r2 = TAG (length = 9)
	and	r3,r3,r15
	or	r2,r2,r3
	and	r3,r7,r15
	sw	r2,POLYGT3_tag(r7)
	addu	r7,r7,POLYGT3_size	;polyme++
	sw	r3,0(r21)

.skip_filler
	addu	r13,r13,-1		;xcount--
	bgez	r13,.xloop		;count>=0??
	nop

.skip_xloop
	sll	r2,r23,4		;r2 = cunt<<4
	addu	r17,r17,r2		;sspol1+=cunt
	addu	r19,r19,r2		;sspol2+=cunt
	addu	r18,r18,r2		;sspol3+=cunt
	addu	r5,r5,1			;ycount++
	slt	r2,r5,r24		;ycount<num_vertical_lines ??
	bne	r2,r0,.yloop		;ycount<num_vertical_lines ??
	addu	r20,r20,-2		;first_xcount-=2

.exit
	move	r2,r14			;r2 = primskip (the value I return)

	lui	r14,scratchpad_base

	lw	r23,regs_offset(r14)	     	;stash r23
	lw	r22,regs_offset+4(r14)	     	;stash r22
	lw	r21,regs_offset+8(r14)	     	;stash r21
	lw	r20,regs_offset+12(r14)	     	;stash r20
	lw	r19,regs_offset+16(r14)	     	;stash r19
	lw	r18,regs_offset+20(r14)	     	;stash r18
	lw	r17,regs_offset+24(r14)	     	;stash r17
	j	r31
	lw	r16,regs_offset+28(r14)	     	;stash r16


; enter with:-
;
; r4 = POLY_GT4 *cur_primlist (ie the poly you just made)
; r5 = u_long *OT (address of order table entry for this poly)
; r6 = ndiv (1,2)
;
;
; returns:-
;
; r2 = number of longs to add to your primlist...
;
; SWEETLY optimized! (no stacking, uses scratchpad)
;

DEL_subdivide_ft4_asm

	lui	r11,scratchpad_base 		;r11 = spol (the subdivide buffer in scratchpad)
	sw	r16,regs_offset(r11)
	sw	r17,regs_offset+4(r11)
	sw	r18,regs_offset+8(r11)
	sw	r19,regs_offset+12(r11)

	move	r16,r4				;r16 = original_poly
	move	r19,r5				;r19 = ot_me

; setup top left co-ords

	lw	r2,POLYFT4_x0(r16)		;r2 = original_poly->x0	and original_poly->y0
	lbu	r3,POLYFT4_u0(r16)		;r2 = original_poly->u0 (byte)
	lbu	r4,POLYFT4_v0(r16)		;r2 = original_poly->v0 (byte)

	move	r18,r6				;r18 = ndiv

	sw	r2,SPOL_x(r11)			;spol->xy[0] = original_poly->x0 and spol->xy[1] = original_poly->y0;
	sh	r3,SPOL_u(r11)			;spol->uv[0] = original_poly->u0;
	sh	r4,SPOL_v(r11)			;spol->uv[1] = original_poly->v0;

;setup top right co-ords

	lw	r2,POLYFT4_x1(r16)		;r2 = original_poly->x1 and original_poly->y1
	lbu	r3,POLYFT4_u1(r16)		;r2 = original_poly->u1
	lbu	r4,POLYFT4_v1(r16)		;r2 = original_poly->v1

	sw	r2,((DIVMAX)*SPOL_size)+SPOL_x(r11)	;(spol+DIVMAX)->xy[0] = original_poly->x1 and (spol+DIVMAX)->xy[1] = original_poly->y1;
	sh	r3,((DIVMAX)*SPOL_size)+SPOL_u(r11)	;(spol+DIVMAX)->uv[0] = original_poly->u1;
	sh	r4,((DIVMAX)*SPOL_size)+SPOL_v(r11)	;(spol+DIVMAX)->uv[1] = original_poly->v1;

;setup bottom left co-ords

	lw	r2,POLYFT4_x2(r16) 		;r2 = original_poly->x2 and original_poly->y2
	lbu	r3,POLYFT4_u2(r16) 		;r2 = original_poly->u2
	lbu	r4,POLYFT4_v2(r16) 		;r2 = original_poly->v2

	sw	r2,((DIVMAX*(DIVMAX+1))*SPOL_size)+SPOL_x(r11)	;(spol+(DIVMAX*(DIVMAX+1)))->xy[0] = original_poly->x2 and (spol+(DIVMAX*(DIVMAX+1)))->xy[1] = original_poly->y2
	sh	r3,((DIVMAX*(DIVMAX+1))*SPOL_size)+SPOL_u(r11)	;(spol+(DIVMAX*(DIVMAX+1)))->uv[0] = original_poly->u2;
	sh	r4,((DIVMAX*(DIVMAX+1))*SPOL_size)+SPOL_v(r11)	;(spol+(DIVMAX*(DIVMAX+1)))->uv[1] = original_poly->v2;

;setup bottom right co-ords

	lw	r2,POLYFT4_x3(r16)		;r2 = original_poly->x3 and original_poly->y3
	lbu	r3,POLYFT4_u3(r16)		;r2 = original_poly->u3
	lbu	r4,POLYFT4_v3(r16)		;r2 = original_poly->v3

	move	r17,r0				;r17 = ndiv_counter

	sw	r2,(((DIVMAX*(DIVMAX+1))+DIVMAX)*SPOL_size)+SPOL_x(r11)	;(spol+((DIVMAX*(DIVMAX+1)))+DIVMAX)->xy[0] = original_poly->x3 and (spol+((DIVMAX*(DIVMAX+1)))+DIVMAX)->xy[1] = original_poly->y3;
	sh	r3,(((DIVMAX*(DIVMAX+1))+DIVMAX)*SPOL_size)+SPOL_u(r11)	;(spol+((DIVMAX*(DIVMAX+1)))+DIVMAX)->uv[0] = original_poly->u3;
	sh	r4,(((DIVMAX*(DIVMAX+1))+DIVMAX)*SPOL_size)+SPOL_v(r11)	;(spol+((DIVMAX*(DIVMAX+1)))+DIVMAX)->uv[1] = original_poly->v3;

	blez	r18,.make_the_polys		;ndiv <= 0 ??
	li	r2,DIVMAX			;r2 = DIVMAX (4)

.main_ndiv_loop
	sra	r12,r2,r17			;r12 = temp1 (DIVMAX>>ndiv_counter)
	sra	r24,r12,1			;r24 = temp2 (temp1>>1)
	move	r14,r0				;r14 = myst1
	move	r13,r0				;r13 = myst2
	move	r10,r0				;r10 = myst3
	sll	r2,r12,2			;r2 = temp1*4
	addu	r25,r2,r12			;r25 = (DIVMAX+1)*temp1
.loop2
	move	r15,r13				;myst4=myst2
.loop1
	addu	r3,r15,r10			;r3 = (myst4+myst3)
	sll	r2,r3,4				;r2 = (myst4+myst3)<<4
	addu	r9,r11,r2			;source_pol1 = spol+((myst4+myst3)<<4)
	addu	r2,r3,r12			;r2 = (myst4+myst3)+temp1
	sll	r2,r2,4				;r2 = ((myst4+myst3)+temp1)<<4
	addu	r8,r11,r2			;source_pol2 = spol+(((myst4+myst3)+temp1)<<4)
	addu	r3,r3,r24			;r3 = (myst4+myst3)+temp2
	sll	r3,r3,4				;r3 = ((myst4+myst3)+temp2)<<4

;r9 = source_pol1
;r8 = source_pol2

	lh	r2,SPOL_x(r9)			;r2 = source_pol1->xy[0]
	lh	r4,SPOL_x(r8)			;r4 = source_pol2->xy[0]
	addu	r7,r11,r3			;dest_pol = spol+(((myst4+myst3)+temp2)<<4)
	lh	r3,SPOL_y(r9)			;r3 = source_pol1->xy[1]
;r7 = dest_pol
	addu	r2,r2,r4			;r2 = source_pol1->xy[0]+source_pol2->xy[0]
	lh	r4,SPOL_y(r8)			;r4 = source_pol2->xy[1]
	sra	r2,r2,1				;r2 = (source_pol1->xy[0]+source_pol2->xy[0])>>1
	addu	r3,r3,r4			;r3 = source_pol1->xy[1]+source_pol2->xy[1]
	sra	r3,r3,1				;r3 = (source_pol1->xy[1]+source_pol2->xy[1])>>1
	sh	r2,SPOL_x(r7)			;dest_pol->xy[0] = (source_pol1->xy[0]+source_pol2->xy[0])>>1
	sh	r3,SPOL_y(r7)			;dest_pol->xy[1] = ((source_pol1->xy[1]+source_pol2->xy[1])>>1)

	lh	r2,SPOL_u(r9)			;r2 = source_pol1->uv[0]
	lh	r4,SPOL_u(r8)			;r3 = source_pol2->uv[0]
	lh	r3,SPOL_v(r9)			;r3 = source_pol1->uv[1]
	addu	r2,r2,r4			;r2 = source_pol1->uv[0]+source_pol2->uv[0]
	lh	r4,SPOL_v(r8)			;r4 = source_pol2->uv[1]
	sra	r2,r2,1				;r2 = (source_pol1->uv[0]+source_pol2->uv[0])>>1
	addu	r3,r3,r4			;r3 = source_pol1->uv[1]+source_pol2->uv[1]
	sra	r3,r3,1				;r3 = (source_pol1->uv[1]+source_pol2->uv[1])>>1
	sh	r2,SPOL_u(r7)			;dest_pol->uv[0] = ((source_pol1->uv[0]+source_pol2->uv[0])>>1)
	sh	r3,SPOL_v(r7)			;dest_pol->uv[1] = ((source_pol1->uv[1]+source_pol2->uv[1])>>1)

	addu	r10,r10,r12			;myst3 = (myst3+temp1)

	opt at-
	slt	at,r10,DIVMAX			;myst3<DIVMAX ??
	bne	at,r0,.loop1			;myst3<DIVMAX ??
	nop
	opt at+

	addu	r13,r13,r25			;myst2 = myst2+((DIVMAX+1)*temp1)
	addu	r14,r14,r12			;myst1 = myst1+temp1
	slt	r2,r14,(DIVMAX+1)		;myst1 < (DIVMAX+1) ??
	bne	r2,r0,.loop2			;myst1 < (DIVMAX+1) ??
	move	r10,r0				;myst3 = 0 (check if this is needed (see the c) )



; second pair of loops...

	move	r14,r0				;myst1 = 0
	move	r13,r0				;myst2 = 0
	sll	r2,r12,2			;r2 = temp1<<2
	addu	r25,r2,r12			;r25 = temp1*(DIVMAX+1)
	sll	r2,r24,2			;r2 = temp2<<2
	addu	r5,r2,r24			;r5 = temp2*(DIVMAX+1)
.loop4
	move	r15,r13				;myst4 = myst2
.loop3
	addu	r3,r15,r10			;r3 = myst4+myst3
	sll	r2,r3,4				;r2 = (myst4+myst3)<<4
	addu	r9,r11,r2			;r9 = spol+((myst4+myst3)<<4)
;r9 = source_pol1
	addu	r2,r25,r3			;r2 = (temp1*(DIVMAX+1))+(myst4+myst3)
	sll	r2,r2,4				;r2 = ((temp1*(DIVMAX+1))+(myst4+myst3)) <<4
	addu	r8,r11,r2			;r8 = spol+(((temp1*(DIVMAX+1))+(myst4+myst3))<<4)
;r8 = source_pol2
	addu	r3,r5,r3			;r3 = (temp2*(DIVMAX+1))+(myst4+myst3)
	sll	r3,r3,4				;r3 = ((temp2*(DIVMAX+1))+(myst4+myst3)) <<4
	lh	r2,SPOL_x(r9)			;r2 = source_pol1->xy[0]
	lh	r4,SPOL_x(r8)			;r4 = source_pol2->xy[0]
	addu	r7,r11,r3			;r7 = spol+(((temp2*(DIVMAX+1))+(myst4+myst3))<<4)
;r7 = dest_pol
	lh	r3,SPOL_y(r9)			;r3 = source_pol1->xy[1]
	addu	r2,r2,r4			;r2 = source_pol1->xy[0]+source_pol2->xy[0]
	lh	r4,SPOL_y(r8)			;r4 = source_pol2->xy[1]
	sra	r2,r2,1				;r2 = (source_pol1->xy[0]+source_pol2->xy[0])>>1
	addu	r3,r3,r4			;r3 = source_pol1->xy[1]+source_pol2->xy[1]
	sra	r3,r3,1				;r3 = (source_pol1->xy[1]+source_pol2->xy[1])>>1
	sh	r2,SPOL_x(r7)			;dest_pol->xy[0] = ((source_pol1->->xy[0]+source_pol2->->xy[0])>>1)
	sh	r3,SPOL_y(r7)			;dest_pol->xy[1] = ((source_pol1->xy[1]+source_pol2->xy[1])>>1)

	lh	r2,SPOL_u(r9)			;r2 = source_pol1->uv[0]
	lh	r4,SPOL_u(r8)			;r4 = source_pol2->uv[0]
	lh	r3,SPOL_v(r9)			;r3 = source_pol1->uv[1]
	addu	r2,r2,r4			;r2 = source_pol1->uv[0]+source_pol2->uv[0]
	lh	r4,SPOL_v(r8)			;r4 = source_pol2->uv[1]
	sra	r2,r2,1				;r2 = (source_pol1->uv[0]+source_pol2->uv[0])>>1
	addu	r3,r3,r4			;r3 = source_pol1->uv[1]+source_pol2->uv[1]
	sra	r3,r3,1				;r3 = (source_pol1->uv[1]+source_pol2->uv[1])>>1
	sh	r2,SPOL_u(r7)			;dest_pol->uv[0] = ((source_pol1->uv[0]+source_pol2->uv[0])>>1)
	sh	r3,SPOL_v(r7)			;dest_pol->uv[1] = ((source_pol1->uv[1]+source_pol2->uv[1])>>1)

	addu	r10,r10,r12			;myst3=(myst3+temp1)
	opt at-
	slt	at,r10,(DIVMAX+1)		;myst3<DIVMAX+1 ??
	bne	at,r0,.loop3			;myst3<DIVMAX+1 ??
	nop
	opt at+

	addu	r13,r13,r25			;myst2=myst2+(temp1*(DIVMAX+1))
	addu	r14,r14,r12			;myst1=myst1+temp1
	slt	r2,r14,DIVMAX			;myst1<DIVMAX ??
	bne	r2,r0,.loop4
	move	r10,r0				;myst3 = 0


;final pair of loops coming up!
	move	r14,r0				;myst1 = 0
	move	r13,r0				;myst2 = 0
	sll	r2,r12,2			;r2 = temp1<<2
	addu	r25,r2,r12			;r25 = temp1*(DIVMAX+1)
	sll	r2,r24,2			;r2 = temp2<<2
	addu	r2,r2,r24			;r2 = temp2*(DIVMAX+1)
	sll	r6,r2,4				;r6 = (temp2*(DIVMAX+1))<<4
.loop6
	move	r10,r0				;myst3 = 0
	move	r15,r13				;myst4=myst2
	addu	r5,r11,r6			;r5 = spol+((temp2*(DIVMAX+1))<<4)
.loop5
	addu	r4,r15,r10			;r4 = myst4+myst3
	sll	r3,r4,4				;r3 = (myst4+myst3)<<4
	addu	r3,r11,r3			;r3 = spol+((myst4+myst3)<<4)
	sll	r2,r24,4			;r2 = temp2<<4
	addu	r9,r3,r2			;r9 = spol+((myst4+myst3)<<4)+(temp2<<4)

;r9 = source_pol1
	addu	r2,r25,r4			;r2 = (temp1*(DIVMAX+1))+(myst4+myst3)
	addu	r2,r2,r24			;r2 = ((temp1*(DIVMAX+1))+(myst4+myst3))+temp2
	sll	r2,r2,4				;r2 = (((temp1*(DIVMAX+1))+(myst4+myst3))+temp2)<<4
	addu	r8,r11,r2			;r8 = spol+((((temp1*(DIVMAX+1))+(myst4+myst3))+temp2)<<4)
	addu	r4,r4,r24			;r4 = (myst4+myst3)+temp2
	sll	r4,r4,4				;r4 = ((myst4+myst3)+temp2)<<4
	lh	r2,SPOL_x(r9)			;r2 = source_pol1->xy[0]
	lh	r3,SPOL_x(r8)			;r3 = source_pol2->xy[0]
	addu	r7,r5,r4			;r7 = spol+((temp2*(DIVMAX+1))<<4)+(((myst4+myst3)+temp2)<<4)
;r7 = dest_pol
	addu	r2,r2,r3			;r2 = source_pol1->xy[0]+source_pol2->xy[0]
	lh	r3,SPOL_y(r9)			;r3 = source_pol1->xy[1]
	lh	r4,SPOL_y(r8)			;r4 = source_pol2->xy[1]
	sra	r2,r2,1				;r2 = (source_pol1->xy[0]+source_pol2->xy[0])>>1
	addu	r3,r3,r4			;r3 = source_pol1->xy[1]+source_pol2->xy[1]
	sra	r3,r3,1				;r3 = (source_pol1->xy[1]+source_pol2->xy[1])>>1
	sh	r2,SPOL_x(r7)			;dest_pol->xy[0] = (source_pol1->xy[0]+source_pol2->xy[0])>>1
	sh	r3,SPOL_y(r7)			;dest_pol->xy[1] = (source_pol1->xy[1]+source_pol2->xy[1])>>1

	lh	r2,SPOL_u(r9)			;r2 = source_pol1->uv[0]
	lh	r4,SPOL_u(r8)			;r4 = source_pol2->uv[0]
	lh	r3,SPOL_v(r9)			;r3 = source_pol1->uv[1]
	addu	r2,r2,r4			;r2 = source_pol1->uv[0]+source_pol2->uv[0]
	lh	r4,SPOL_v(r8)			;r4 = source_pol2->uv[1]
	sra	r2,r2,1				;r2 = (source_pol1->uv[0]+source_pol2->uv[0])>>1
	addu	r3,r3,r4			;r3 = source_pol1->uv[1]+source_pol2->uv[1]
	sra	r3,r3,1				;r3 = (source_pol1->uv[1]+source_pol2->uv[1])>>1
	sh	r2,SPOL_u(r7)			;dest_pol->uv[0] = (source_pol1->uv[0]+source_pol2->uv[0])>>1
	sh	r3,SPOL_v(r7)			;dest_pol->uv[1] = ((source_pol1->uv[1]+source_pol2->uv[1])>>1)

	addu	r10,r10,r12			;myst3=myst3+temp1

	opt at-
	slt	at,r10,DIVMAX			;myst3 < DIVMAX ??
	bne	at,r0,.loop5			;myst3 < DIVMAX ??
	nop
	opt at+

	addu	r14,r14,r12			;myst1=myst1+temp1
	slt	r2,r14,DIVMAX			;myst1<DIVMAX ??

	bne	r2,r0,.loop6			;myst1<DIVMAX ??
	addu	r13,r13,r25			;myst2 = myst2+(temp1*(DIVMAX+1))

	addu	r17,r17,1			;ndiv_counter++
	slt	r2,r17,r18			;ndiv_counter < ndiv ??

	bne	r2,r0,.main_ndiv_loop		;ndiv_counter < ndiv ??
	li	r2,DIVMAX			;r2 = DIVMAX (4)




;r11 = spol
;r16 = original_poly
;r18 = ndiv

.make_the_polys
	move	r14,r11				;r14 = spol
	li	r3,DIVMAX			;r3 = DIVMAX
	sra	r3,r3,r18			;step = DIVMAX>>ndiv
	sll	r7,r3,4				;r7 = step<<4
	addu	r25,r14,r7			;r25 = spol+step<<4
	sll	r4,r3,2				;r4 = step<<2
	addu	r4,r4,r3			;r4 = step*(DIVMAX+1)
	sll	r2,r4,4				;r2 = step*(DIVMAX+1)<<4
	addu	r24,r14,r2			;r14 = spol+(step*(DIVMAX+1)<<4)
	addu	r2,r4,r3			;r2 = (step*(DIVMAX+1))+DIVMAX
	sll	r2,r2,4				;r2 = ((step*(DIVMAX+1))+DIVMAX)<<4
	addu	r15,r14,r2			;r15 = spol+(((step*(DIVMAX+1))+DIVMAX)<<4)

	move	r13,r16				;r13 = original_poly
	move	r5,r0				;j=0
	move	r16,r0				;primskip = 0
	sll	r3,r3,r18			;r3 = step<<ndiv
	li	r2,1				;r2 = 1
	sll	r6,r2,r18			;slag = (1<<ndiv)

;
;r6  = slag
;r13 = original_poly
;r14 = spol1
;r25 = spol2
;r24 = spol3
;r15 = spol4
;
	blez	r6,.exitme			;slag<=0 ??
	subu	r18,r4,r3			;r18 = (step*(DIVMAX+1))-(step<<ndiv)

;	lbu	r11,POLYFT4_code(r13) 		;r11 = original_poly->code
	lw	r11,POLYFT4_r0(r13)		;r11 = original_poly->r0,g0,b0,code
	lhu	r10,POLYFT4_tpage(r13)		;r10 = original_poly->tpage
	lhu	r9,POLYFT4_clut(r13) 		;r9 = original_poly->clut

	li	r17,$00ff0000
	ori	r17,r17,$ffff			;r17 = $00ffffff
	move	r12,r7				;r12 = step<<4
.jloop
	blez	r6,.next_poly			;slag<= 0 ??
	move	r4,r0				;k=0

.kloop

;top left corner

	lw	r8,SPOL_x(r14)			;r8 = spol1->xy[0] and spol1->xy[1]
	lw	r3,SPOL_u(r14)			;r2 = spol1->uv[0] and spol1->uv[1]

	sw	r8,POLYFT4_x0(r13)		;stash x0 and y0
	sb	r3,POLYFT4_u0(r13)		;stash u0
	srl	r2,r3,16			;shift uv
	sb	r2,POLYFT4_v0(r13)   		;stash v0

; top right corner

	lw	r8,SPOL_x(r25)			;r8 = spol1->xy[0] and spol1->xy[1]
	lw	r3,SPOL_u(r25)			;r2 = spol1->uv[0] and spol1->uv[1]
	sw	r8,POLYFT4_x1(r13)		;stash x1 and y1
	sb	r3,POLYFT4_u1(r13)  		;stash u1
	srl	r2,r3,16			;shift uv
	sb	r2,POLYFT4_v1(r13)		;stash v1

;bottom left corner

	lw	r8,SPOL_x(r24)			;r8 = spol1->xy[0] and spol1->xy[1]
	lw	r3,SPOL_u(r24)			;r2 = spol1->uv[0] and spol1->uv[1]
	sw	r8,POLYFT4_x2(r13)	    	;stash x2 and y2
	sb	r3,POLYFT4_u2(r13)		;stash u2
	srl	r2,r3,16			;shift uv
	sb	r2,POLYFT4_v2(r13)		;stash v2

;bottom right corner

	lw	r8,SPOL_x(r15)			;r8 = spol1->xy[0] and spol1->xy[1]
	lw	r3,SPOL_u(r15)			;r2 = spol1->uv[0] and spol1->uv[1]
	sw	r8,POLYFT4_x3(r13)  		;stash x3 and y3
	sb	r3,POLYFT4_u3(r13)  		;stash u3
	srl	r2,r3,16			;shift uv
	sb	r2,POLYFT4_v3(r13)  		;stash v3

	addu	r14,r14,r12			;spol1=spol1+(step<<4)
	addu	r25,r25,r12			;spol2=spol2+(step<<4)
	addu	r24,r24,r12			;spol3=spol3+(step<<4)
	addu	r15,r15,r12			;spol4=spol4+(step<<4)

;	sb	r11,POLYFT4_code(r13)  		;stash poly code
	sw	r11,POLYFT4_r0(r13)  		;stash poly r0,g0,b0,code
	sh	r9,POLYFT4_clut(r13)   		;stash clut (CHANGE)
	sh	r10,POLYFT4_tpage(r13) 		;stash tpage

	addu	r4,r4,1				;k++
	addu	r16,r16,(POLYFT4_size)/4	;primskip+=((POLYFT4_size)/4)

;r13 = poly
;r19 = ot
;this is the inline addprim

	lw	r3,0(r19)			;r3 = 0(ot)
	lui	r2,(POLYFT4_PRIMLEN<<8)		;r2 = TAG (primlen<<8)
	and	r3,r3,r17			;r3 = 0(ot)&$00ffffff
	or	r2,r2,r3			;r2 = (TAG&$ff000000)|0(ot)&$00ffffff
	and	r3,r13,r17			;r3 = poly address&$00ffffff
	sw	r2,POLYFT4_tag(r13)		;stash (TAG&$ff000000)|0(ot)&$00ffffff
	sw	r3,0(r19)			;stash poly address&$00ffffff

	slt	r2,r4,r6			;k<slag??
	bne	r2,r0,.kloop			;k<slag??
	addu	r13,r13,POLYFT4_size		;original_poly++

.next_poly
	sll	r2,r18,4
	addu	r14,r14,r2
	addu	r25,r25,r2
	addu	r24,r24,r2
	addu	r5,r5,1				;j++
	opt at-
	slt	at,r5,r6			;j<slag??
	bne	at,r0,.jloop
	addu	r15,r15,r2
	opt at+

.exitme
	move	r2,r16				;r2 = primskip (the value I return)

	lui	r11,scratchpad_base		;r11 = scratchpad
	lw	r16,regs_offset(r11)	 	;restore r16
	lw	r17,regs_offset+4(r11)	 	;restore r17
	lw	r18,regs_offset+8(r11)	 	;restore r18
	j	r31
	lw	r19,regs_offset+12(r11)	 	;restore r19





; enter with:-
;
; r4 = POLY_FT3 *cur_primlist (ie the poly you just made)
; r5 = u_long *OT (address of order table entry for this poly)
; r6 = ndiv (1,2)
;
; returns:-
;
; r2 = number of longs to add to your primlist...
;
; SWEETLY optimized! (no stacking, uses scratchpad)
;

DEL_subdivide_ft3_asm

	lui	r14,scratchpad_base
	sw	r23,regs_offset(r14)	     	;stash r23
	sw	r22,regs_offset+4(r14)	     	;stash r22
	sw	r21,regs_offset+8(r14)	     	;stash r21
	sw	r20,regs_offset+12(r14)	     	;stash r20
	sw	r19,regs_offset+16(r14)	     	;stash r19
	sw	r18,regs_offset+20(r14)	     	;stash r18
	sw	r17,regs_offset+24(r14)	     	;stash r17
	sw	r16,regs_offset+28(r14)	     	;stash r16

; setup top-left co-ords

	move	r16,r4		     	;r16 = original_poly

	lw	r2,POLYFT3_x0(r16)	;r2 = original_poly->x0 and original_poly->y0
	lbu	r3,POLYFT3_u0(r16) 	;r2 = original_poly->u0
	lbu	r4,POLYFT3_v0(r16)	;r2 = original_poly->v0

	move	r21,r5			;r21 = ot_me

	sw	r2,SPOL_x(r14)		;spol->xy[0] = original_poly->x0 & spol->xy[1] = original_poly->y0
	sh	r3,SPOL_u(r14)		;spol->uv[0] = original_poly->u0
	sh	r4,SPOL_v(r14)		;spol->uv[1]= original_poly->v0

; setup top-right co-ords

	lw	r2,POLYFT3_x1(r16)    	;r2 = original_poly->x1 and original_poly->y1
	lbu	r3,POLYFT3_u1(r16)    	;r2 = original_poly->u1
	lbu	r4,POLYFT3_v1(r16)    	;r2 = original_poly->v1

	li	r20,DIVMAX		;DIVMAX

	sw	r2,((DIVMAX)*SPOL_size)+SPOL_x(r14)		;(spol+DIVMAX2)->xy[0] = original_poly->x1
	sh	r3,((DIVMAX)*SPOL_size)+SPOL_u(r14)		;(spol+DIVMAX2)->uv[0] = original_poly->u1
	sh	r4,((DIVMAX)*SPOL_size)+SPOL_v(r14)		;(spol+DIVMAX2)->uv[1] = original_poly->v1

; setup bottom-left co-ords

	lw	r2,POLYFT3_x2(r16) 	;r2 = original_poly->x2 and original_poly->y2
	lbu	r3,POLYFT3_u2(r16) 	;r2 = original_poly->u2
	lbu	r4,POLYFT3_v2(r16) 	;r2 = original_poly->v2

	move	r22,r6			;r22 = ndiv

	sw	r2,((DIVMAX*(DIVMAX+1))*SPOL_size)+SPOL_x(r14)		;(spol+(DIVMAX2*(DIVMAX2+1)))->xy[0] = original_poly->x2
	sh	r3,((DIVMAX*(DIVMAX+1))*SPOL_size)+SPOL_u(r14)		;(spol+(DIVMAX2*(DIVMAX2+1)))->uv[0] = original_poly->u2
	sh	r4,((DIVMAX*(DIVMAX+1))*SPOL_size)+SPOL_v(r14)		;(spol+(DIVMAX2*(DIVMAX2+1)))->uv[1] = original_poly->v2

	blez	r22,.make_the_polys
	move	r19,r0			;ndiv_counter = 0

;
;r20 = DIVMAX
;r22 = ndiv
;r21 = ot_me
;r19 = ndiv_counter
;r16 = original_poly
;r14 = spol
;

.ndiv_loop
	sra	r13,r20,r19		;temp1 = DIVMAX>>ndiv_counter
	sra	r25,r13,1		;temp2 = temp1>>1
	move	r11,r0			;myst1 = 0
	move	r12,r0			;myst2 = 0
	sll	r2,r13,2		;r2 = temp1<<2
	addu	r17,r2,r13		;r17 = temp1*(DIVMAX+1)
.loop2
	subu	r2,r20,r11		;r2 = DIVMAX-myst1
	blez	r2,.skip_loop1		;(DIVMAX-myst1)<=0 ??

;	nop	;don`t need this really... :)

	move	r10,r0			;myst4 = 0
	move	r24,r12			;myst3 = myst2
	move	r15,r2			;delme1 = DIVMAX-myst1
	addu	r3,r24,r10		;r3 = myst3+myst4
.loop1
	sll	r2,r3,4			;r3 = (myst3+myst4)<<4
	addu	r7,r14,r2		;r7 = spol+((myst3+myst4)<<4)
;r7 = source_pol1

	addu	r2,r3,r13		;r2 = (myst3+myst4)+temp1
	sll	r2,r2,4			;r2 = ((myst3+myst4)+temp1)<<4
	addu	r8,r14,r2		;r8 = spol+(((myst3+myst4)+temp1)<<4)
;r8 = source_pol2

	addu	r3,r3,r25		;r3 = (myst3+myst4)+temp2
	sll	r3,r3,4			;r3 = ((myst3+myst4)+temp2)<<4

	lh	r2,SPOL_x(r7)		;r2 = source_pol1->xy[0]
	lh	r4,SPOL_x(r8)		;r4 = source_pol2->xy[0]
	addu	r9,r14,r3		;r9 = spol+(((myst3+myst4)+temp2)<<4)
;r9 = destpol

	addu	r2,r2,r4		;r2 = source_pol1->xy[0]+source_pol2->xy[0]

	lh	r3,SPOL_y(r7)		;r3 = source_pol1->xy[1]
	lh	r4,SPOL_y(r8)		;r4 = source_pol2->xy[1]
	sra	r2,r2,1			;r2 = (source_pol1->xy[0]+source_pol2->xy[0])>>1
	addu	r3,r3,r4		;r3 = source_pol1->xy[1]+source_pol2->xy[1]
	sra	r3,r3,1			;r3 = (source_pol1->xy[1]+source_pol2->xy[1])>>1
	sh	r2,SPOL_x(r9)		;dest_pol->xy[0] = ((source_pol1->xy[0]+source_pol2->xy[0])>>1)
	sh	r3,SPOL_y(r9)		;dest_pol->xy[1] = (source_pol1->xy[1]+source_pol2->xy[1])>>1

	lh	r2,SPOL_u(r7)		;r2 = source_pol1->uv[0]
	lh	r4,SPOL_u(r8)		;r3 = source_pol2->uv[0]
	lh	r3,SPOL_v(r7)		;r3 = source_pol1->uv[1]
	addu	r2,r2,r4		;r2 = source_pol1->uv[0]+source_pol2->uv[0]
	lh	r4,SPOL_v(r8)		;r4 = source_pol2->uv[1]
	sra	r2,r2,1			;r2 = (source_pol1->uv[0]+source_pol2->uv[0])>>1
	addu	r3,r3,r4		;r3 = source_pol1->uv[1]+source_pol2->uv[1]
	sra	r3,r3,1			;r3 = (source_pol1->uv[1]+source_pol2->uv[1])>>1
	sh	r2,SPOL_u(r9)		;dest_pol->uv[0]
	sh	r3,SPOL_v(r9)		;dest_pol->uv[1] = ((source_pol1->uv[1]+source_pol2->uv[1])>>1)

	addu	r10,r10,r13		;myst4=myst4+temp1
	slt	r2,r10,r15		;myst4<delme1 ??
	bne	r2,r0,.loop1		;myst4<delme1 ??
	addu	r3,r24,r10		;r3 = (myst3+myst4)

.skip_loop1
	addu	r11,r11,r13		;myst1=myst1+temp1
	slt	r2,r11,(DIVMAX+1)   	;myst1<(DIVMAX+1) ??
	bne	r2,r0,.loop2		;myst1<(DIVMAX+1) ??
	addu	r12,r12,r17		;myst2=myst2+(temp1*(DIVMAX+1))


; second pair of loops coming your way...

	opt at-

	move	r10,r0			;myst4 =0
	sll	r2,r13,2		;r2 = temp1<<2
	addu	r4,r2,r13		;r4 = temp1*(DIVMAX+1)
	sll	r2,r25,2		;r2 = temp2<<2
	addu	r2,r2,r25		;r2 = temp2*(DIVMAX+1)
	sll	r24,r2,4		;r24 = (temp2*(DIVMAX+1))<<4
	subu	r2,r20,r10		;r2 = DIVMAX-myst4
.loop4
	blez	r2,.skip_loop3		;DIVMAX-myst4 <= 0 ??
;	nop	;don`t need this really... :)

	move	r11,r0			;myst1=0
	move	r15,r2			;delme1=(DIVMAX2-myst4)
	move	r12,r0			;myst2=0
.loop3
	addu	r2,r12,r10		;r2 = myst2+myst4
	sll	r2,r2,4			;r2 = (myst2+myst4)<<4
	addu	r7,r14,r2		;r7 = spol+((myst2+myst4)<<4)
;r7 = source_pol1

	sll	r2,r4,4			;r2 = (temp1*(DIVMAX+1))<<4
	addu	r8,r7,r2		;r8 = spol+((myst2+myst4)<<4)+((temp1*(DIVMAX+1))<<4)
;r8 = source_pol2

	lh	at,SPOL_x(r7)		;at = source_pol1->xy[0]
	lh	r2,SPOL_x(r8)		;r2 = source_pol2->xy[0]
	lh	r3,SPOL_y(r8)		;r3 = source_pol2->xy[1]
	addu	r9,r7,r24		;r9 = spol+((myst2+myst4)<<4)+((temp2*(DIVMAX+1))<<4)
;r9 = dest_pol
	addu	at,at,r2		;at = source_pol1->xy[0]+source_pol2->xy[0]
	lh	r2,SPOL_y(r7)		;r2 = source_pol1->xy[1]
	sra	at,at,1			;at = (source_pol1->xy[0]+source_pol2->xy[0])>>1
	addu	r2,r2,r3		;r2 = source_pol1->xy[1]+source_pol2->xy[1]
	sra	r2,r2,1			;r2 = (source_pol1->xy[1]+source_pol2->xy[1])>>1
	sh	at,SPOL_x(r9)		;dest_pol->xy[0] = ((source_pol1->xy[0]+source_pol2->xy[0])>>1)
	sh	r2,SPOL_y(r9)		;dest_pol->xy[1] = ((source_pol1->xy[1]+source_pol2->xy[1])>>1)


	lh	at,SPOL_u(r7)		;at = source_pol1->uv[0]
	lh	r2,SPOL_u(r8)		;r3 = source_pol2->uv[0]
	lh	r3,SPOL_v(r8)		;r2 = source_pol2->uv[1]
	addu	at,at,r2		;at = source_pol1->uv[0]+source_pol2->uv[0]
	lh	r2,SPOL_v(r7)		;r2 = source_pol1->uv[1]
	sra	at,at,1			;at = (source_pol1->uv[0]+source_pol2->uv[0])>>1
	addu	r2,r2,r3		;r2 = source_pol1->uv[1]+source_pol2->uv[1]
	sra	r2,r2,1			;r2 = (source_pol1->uv[1]+source_pol2->uv[1])>>1
	sh	at,SPOL_u(r9)		;dest_pol->uv[0] = ((source_pol1->uv[0]+source_pol2->uv[0])>>1)
	sh	r2,SPOL_v(r9)		;dest_pol->uv[1] = ((source_pol1->uv[1]+source_pol2->uv[1])>>1)

	addu	r11,r11,r13		;myst1=myst1+temp1
	slt	r2,r11,r15		;myst1<delme1
	bne	r2,r0,.loop3		;myst1<delme1
	addu	r12,r12,r4		;myst2=myst2+(temp1*(DIVMAX+1))

.skip_loop3
	addu	r10,r10,r13		;myst4=(myst4+temp1)
	slt	r2,r10,(DIVMAX+1)	;myst4<(DIVMAX+1) ??
	bne	r2,r0,.loop4		;myst4<(DIVMAX+1) ??
	subu	r2,r20,r10		;r2 = DIVMAX-myst4

	opt at+

; ok, here goes the final 2 loops...

	move	r11,r0			;myst1=0
	move	r12,r0			;myst2=0
	sll	r2,r13,2		;r2 = temp1<<2
	addu	r6,r2,r13		;r6 = temp1*(DIVMAX+1)
	sll	r17,r6,4		;r17 = (temp1*(DIVMAX+1))<<4
	sll	r2,r25,1		;r2 = temp2<<1
	addu	r18,r2,r25		;r18 = temp2*3
.loop6
	subu	r2,r20,r11		;r2 = DIVMAX-myst1
	blez	r2,.skiploop5		;(DIVMAX-myst1) <= 0 ??
;	nop	;don`t need this really... :)

	move	r10,r0			;myst4 = 0
	move	r24,r12			;myst3=myst2
	move	r15,r2			;delme1 = (DIVMAX-myst1)
	sll	r5,r18,5		;r5 = (temp2*(DIVMAX+2))<<4
	addu	r3,r24,r10		;r3 = (myst3+myst4)
.loop5
	sll	r3,r3,4			;r3 = (myst3+myst4)<<4
	addu	r3,r14,r3		;r3 = spol+(myst3+myst4)<<4
	sll	r2,r25,5		;r2 = (temp2<<1)<<4
	addu	r7,r3,r2		;r7 = spol+((myst3+myst4)<<4)+((temp2<<1)<<4)
;r7 = source_pol1

	addu	r8,r3,r17		;r8 = spol+((myst3+myst4)<<4)+((temp1*(DIVMAX+1))<<4)
;r8 = source_pol2

	lh	r2,SPOL_x(r7)		;r2 = source_pol1->xy[0]
	lh	r4,SPOL_x(r8)		;r4 = source_pol2->xy[0]
	addu	r9,r3,r5		;r9 = spol+((myst3+myst4)<<4)+((temp2*(DIVMAX+2))<<4)
	lh	r3,SPOL_y(r7)		;r3 = source_pol1->xy[1]
;r9 = dest_pol
	addu	r2,r2,r4		;r2 = source_pol1->xy[0]+source_pol2->xy[0]
	lh	r4,SPOL_y(r8)		;r4 = source_pol2->xy[1]
	sra	r2,r2,1			;r2 = (source_pol1->xy[0]+source_pol2->xy[0])>>1
	addu	r3,r3,r4		;r3 = source_pol1->xy[1]+source_pol2->xy[1]
	sra	r3,r3,1			;r3 = (source_pol1->xy[1]+source_pol2->xy[1])>>1
	sh	r2,SPOL_x(r9)		;dest_pol->xy[0] = ((source_pol1->xy[0]+source_pol2->xy[0])>>1)
	sh	r3,SPOL_y(r9)		;dest_pol->xy[1] = ((source_pol1->xy[1]+source_pol2->xy[1])>>1)

	lh	r2,SPOL_u(r7)		;r2 = source_pol1->uv[0]
	lh	r4,SPOL_u(r8)		;r4 = source_pol2->uv[0]
	lh	r3,SPOL_v(r7)		;r3 = source_pol1->uv[1]
	addu	r2,r2,r4		;r2 = source_pol1->uv[0]+source_pol2->uv[0]
	lh	r4,SPOL_v(r8)		;r4 = source_pol2->uv[1]
	sra	r2,r2,1			;r2 = (source_pol1->uv[0]+source_pol2->uv[0])>>1
	addu	r3,r3,r4		;r3 = source_pol1->uv[1]+source_pol2->uv[1]
	sra	r3,r3,1			;r2 = (source_pol1->uv[1]+source_pol2->uv[1])>>1
	sh	r2,SPOL_u(r9)		;dest_pol->uv[0] = ((source_pol1->uv[0]+source_pol2->uv[0])>>1)
	sh	r3,SPOL_v(r9)		;dest_pol->uv[1] = ((source_pol1->uv[1]+source_pol2->uv[1])>>1)

	addu	r10,r10,r13		;myst4=myst4+temp1
	slt	r2,r10,r15		;myst4<delme1 ??
	bne	r2,r0,.loop5		;myst4<delme1 ??
	addu	r3,r24,r10		;r3 = (myst3+myst4)

.skiploop5
	addu	r11,r11,r13		;myst1=(myst1+temp1)
	slt	r2,r11,DIVMAX		;myst1<DIVMAX ??
	bne	r2,r0,.loop6		;myst1<DIVMAX ??
	addu	r12,r12,r6		;myst2=myst2+(temp1*(DIVMAX+1))

	addu	r19,r19,1		;ndiv_counter++
	slt	r2,r19,r22		;ndiv_counter<ndiv ??
	bne	r2,r0,.ndiv_loop	;ndiv_counter<ndiv ??
	nop


;r22 = ndiv
;
;
;
;
; killed R7 shitterness, R4 is now free...
; sorted loop, rgb longs, x&y longs,tpage/clut id`s from original poly...



.make_the_polys
	li	r2,1 			;r2 = 1
	sll	r24,r2,r22		;num_vertical_lines = (1<<ndiv)
	sll	r2,r24,1		;r2 = num_vertical_lines<<1
	addu	r20,r2,-1		;first_xcount = (num_vertical_lines<<1)-1
	move	r17,r14			;r17 = sspol1
;r17 = sspol1
	li	r2,DIVMAX		;r2 = DIVMAX
	sra	r6,r2,r22		;step = DIVMAX>>ndiv
	sll	r2,r6,4			;r2 = (DIVMAX>>ndiv)<<4

	addu	r19,r17,r2		;sspol2 = spol+step
;r19 = sspol2
	sll	r3,r6,2			;r3 = step<<2
	addu	r3,r3,r6		;cunt = (step*(DIVMAX+1))
	sll	r2,r3,4			;r2 = cunt<<4
	addu	r18,r17,r2		;sspol3 = spol+(cunt<<4)
;r18 = sspol3
	move	r7,r16			;r7 = polyme
	move	r5,r0			;ycount=0
	move	r14,r0			;primskip = 0
	blez	r24,.exit		;num_vertical_lines<=0??
	move	r23,r3			;r23 = cunt

;	lbu	r22,POLYFT3_code(r7)	;r22 = poly code (from original poly)
	lw	r22,POLYFT3_r0(r7)	;r22 = poly r0,g0,b0,code (from original poly)
	lhu	r25,POLYFT3_tpage(r7)	;r25 = tpage id (from original poly)
	lhu	r16,POLYFT3_clut(r7)	;r16 = clut id (from original poly)

	li	r15,$00ff0000
	ori	r15,r15,$ffff
.yloop
	move	r12,r17			;spol1=sspol1
	move	r11,r19			;spol2=sspol2
	addu	r13,r20,-1		;xcount=(first_xcount-1)
	bltz	r13,.skip_xloop		;xcount<0 ?
	move	r10,r18			;spol3=sspol3

.xloop


	lw	r2,SPOL_x(r12)		;r2 = spol1->xy[0] and spol1->xy[1]
	lbu	r3,SPOL_u(r12)		;r3 = spol1->uv[0]
	lbu	r4,SPOL_v(r12)		;r2 = spol1->uv[1]

	addu	r14,r14,(POLYFT3_size)/4 ;primskip+=((POLYFT3_size)/4)

	sw	r2,POLYFT3_x0(r7) 	;stash x0&y0
	sb	r3,POLYFT3_u0(r7) 	;stash u0
	sb	r4,POLYFT3_v0(r7) 	;stash v0

;

	lw	r2,SPOL_x(r11)		;r2 = spol2->xy[0] and spol2->xy[1]
	lbu	r3,SPOL_u(r11)		;r3 = spol2->uv[0]
	lbu	r4,SPOL_v(r11)		;r4 = spol2->uv[1]

	move	r8,r11			;spolfill1 = spol2

	sw	r2,POLYFT3_x1(r7) 	;stash x1&y1
	sb	r3,POLYFT3_u1(r7) 	;stash u1
	sb	r4,POLYFT3_v1(r7)	;stash v1

;
	lw	r2,SPOL_x(r10)		;r2 = spol3->xy[0] and spol3->xy[1]
	lbu	r3,SPOL_u(r10)		;r3 = spol3->uv[0]
	lbu	r4,SPOL_v(r10)		;r4 = spol3->uv[1]

	move	r9,r10			;spolfill2 = spol3

	sw	r2,POLYFT3_x2(r7) 	;stash x2&y2
	sb	r3,POLYFT3_u2(r7) 	;stash u2
	sb	r4,POLYFT3_v2(r7) 	;stash v2
;

	sll	r3,r6,4			;r3 = step<<4

	addu	r12,r12,r3		;spol1=spol1+step

;	sb	r22,POLYFT3_code(r7) 	;stash code (52)!!
	sw	r22,POLYFT3_r0(r7) 	;stash r0,g0,b0,code!
	addu	r11,r11,r3		;spol2=spol2+step
	sh	r25,POLYFT3_tpage(r7)	;stash tpage
	addu	r10,r10,r3		;spol3=spol3+step
	sh	r16,POLYFT3_clut(r7)	;stash clut


;inline addprim

	lw	r3,0(r21)
	lui	r2,(POLYFT3_PRIMLEN<<8)	;r2 = TAG<<8
	and	r3,r3,r15
	or	r2,r2,r3
	and	r3,r7,r15
	sw	r2,POLYFT3_tag(r7)
	addu	r7,r7,POLYFT3_size	;polyme++
	beq	r13,r0,.skip_filler	;xcount == 0??
	sw	r3,0(r21)

; if not the last poly on a line, do a filler...

	lw	r2,SPOL_x(r8)		;r2 = spolfill1->xy[0] and spolfill1->xy[1]
	lbu	r3,SPOL_u(r8)		;r3 = spolfill1->uv[0]
	lbu	r4,SPOL_v(r8)		;r4 = spolfill1->uv[1]

	addu	r14,r14,(POLYFT3_size)/4 ;primskip+=((POLYFT3_size)/4)

	sw	r2,POLYFT3_x0(r7)	;stash x0&y0
	sb	r3,POLYFT3_u0(r7)	;stash u0
	sb	r4,POLYFT3_v0(r7)	;stash v0


;
	lw	r2,SPOL_x(r9)		;r2 = spollfill2->xy[0] and spolfill2->xy[1]
	lbu	r3,SPOL_u(r9)		;r2 = spollfill2->uv[0]
	lbu	r4,SPOL_v(r9)		;r2 = spollfill2->uv[1]

	sw	r2,POLYFT3_x1(r7)	;stash x1&y1
	sb	r3,POLYFT3_u1(r7)	;stash u1
	sb	r4,POLYFT3_v1(r7)	;stash v1

;
	lw	r2,SPOL_x(r10)		;r2 = spol3->xy[0] and spol3->xy[1]
	lbu	r3,SPOL_u(r10)		;r2 = spol3->uv[0]
	lbu	r4,SPOL_v(r10)		;r2 = spol3->uv[0]

	sw	r2,POLYFT3_x2(r7)	;stash x2&y2
	sb	r3,POLYFT3_u2(r7)	;stash u2
	sb	r4,POLYFT3_v2(r7)	;stash v2

;	sb	r22,POLYFT3_code(r7)	;stash prim code
	sw	r22,POLYFT3_r0(r7) 	;stash r0,g0,b0,code!
	sh	r25,POLYFT3_tpage(r7) 	;stash TPAGE ID
	addu	r13,r13,-1		;xcount--
	sh	r16,POLYFT3_clut(r7) 	;stash CLUT ID

;inline addprim
	lw	r3,0(r21)
	lui	r2,(POLYFT3_PRIMLEN<<8)	;r2 = TAG<<8
	and	r3,r3,r15
	or	r2,r2,r3
	and	r3,r7,r15
	sw	r2,POLYFT3_tag(r7)
	addu	r7,r7,POLYFT3_size  	;polyme++
	sw	r3,0(r21)

.skip_filler
	addu	r13,r13,-1		;xcount--
	bgez	r13,.xloop		;count>=0??
	nop

.skip_xloop
	sll	r2,r23,4		;r2 = cunt<<4
	addu	r17,r17,r2		;sspol1+=cunt
	addu	r19,r19,r2		;sspol2+=cunt
	addu	r18,r18,r2		;sspol3+=cunt
	addu	r5,r5,1			;ycount++
	slt	r2,r5,r24		;ycount<num_vertical_lines ??
	bne	r2,r0,.yloop		;ycount<num_vertical_lines ??
	addu	r20,r20,-2		;first_xcount-=2

.exit
	move	r2,r14			;r2 = primskip (the value I return)

	lui	r14,scratchpad_base

	lw	r23,regs_offset(r14)	     	;stash r23
	lw	r22,regs_offset+4(r14)	     	;stash r22
	lw	r21,regs_offset+8(r14)	     	;stash r21
	lw	r20,regs_offset+12(r14)	     	;stash r20
	lw	r19,regs_offset+16(r14)	     	;stash r19
	lw	r18,regs_offset+20(r14)	     	;stash r18
	lw	r17,regs_offset+24(r14)	     	;stash r17
	j	r31				;l8r fuckers...
	lw	r16,regs_offset+28(r14)	     	;stash r16

