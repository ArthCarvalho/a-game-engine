	/* PSX 'Bare Metal' LZ77 Decode Algorithm by krom (Peter Lemon)
   C Header Version by Arthur Carvalho de Souza Lima
   Source: https://github.com/PeterLemon/PSX/tree/master/Compress/LZ77/LZ77Decode */
  .section .text
  
  .global	asm_lzdecompress

#include "mipsregs.h"

/* unsigned long * lzDecompress(unsigned long * src, unsigned long * dest) */
asm_lzdecompress:
  lw t0,0(a0) /* T0 = Data Length & Header Info */
  addiu a0,4  /* Add 4 To LZ Offset */
  srl t0,8    /* T0 = Data Length */
  addu t0,a1  /* T0 = Destination End Offset (RAM End Offset) */

  LZLoop:
    lbu t1,0(a0)        /* T1 = Flag Data For Next 8 Blocks (0 = Uncompressed Byte, 1 = Compressed Bytes) */
    addiu a0,1          /* Add 1 To LZ Offset */
    ori t2,r0,0b10000000 /* T2 = Flag Data Block Type Shifter */
    LZBlockLoop:
      beq a1,t0,LZEnd  /* IF (Destination Address == Destination End Offset) LZEnd */
      and t4,t1,t2     /* Test Block Type (Delay Slot) */
      beqz t2,LZLoop   /* IF (Flag Data Block Type Shifter == 0) LZLoop */
      srl t2,1         /* Shift T2 To Next Flag Data Block Type (Delay Slot) */
      lbu t3,0(a0)     /* T3 = Copy Uncompressed Byte / Number Of Bytes To Copy & Disp MSB's */
      bnez t4,LZDecode /* IF (BlockType != 0) LZDecode Bytes */
      addiu a0,1       /* Add 1 To LZ Offset (Delay Slot) */
      sb t3,0(a1)      /* Store Uncompressed Byte To Destination */
      j LZBlockLoop
      addiu a1,1       /* Add 1 To RAM Offset (Delay Slot) */

      LZDecode:
        lbu t4,0(a0)  /* T4 = Disp LSB's */
        addiu a0,1    /* Add 1 To LZ Offset */
        sll t5,t3,8   /* T5 = Disp MSB's */
        or t4,t5      /* T4 = Disp 16-Bit */
        andi t4,0xFFF /* T4 &= $FFF (Disp 12-Bit) */
        nor t4,r0     /* T4 = -Disp - 1 */
        addu t4,a1    /* T4 = Destination - Disp - 1 */
        srl t3,4      /* T3 = Number Of Bytes To Copy (Minus 3) */
        addiu t3,3    /* T3 = Number Of Bytes To Copy */
        LZCopy:
          lbu t5,0(t4)   /* T5 = Byte To Copy */
          addiu t4,1     /* Add 1 To T4 Offset */
          sb t5,0(a1)    /* Store Byte To RAM */
          addiu t3,-1     /* Number Of Bytes To Copy -= 1 */
          bnez t3,LZCopy /* IF (Number Of Bytes To Copy != 0) LZCopy Bytes */
          addiu a1,1     /* Add 1 To RAM Offset (Delay Slot) */
          j LZBlockLoop
          nop /* Delay Slot */
    LZEnd:
  j ra
  or v0,zero,a1  /* Return Data End Address */