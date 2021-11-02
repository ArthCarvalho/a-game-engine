/* $PSLibId: Run-time Library Release 4.4$ */
/*
 * "spadstk.h" - scratch pad stack sample macro
 * 
 * Version 1.00 July 18, 1997
 * 
 * Copyright (C) 1993-97 Sony Computer Entertainment Inc.
 * All rights Reserved 
 */

/* scratch pad address 0x1f800000 - 0x1f800400 */
#define SPAD_STACK_ADDR 0x1f8003fc

#define SetSpadStack(addr) { \
    __asm__ volatile ("move $8,%0"     ::"r"(addr):"$8","memory"); \
    __asm__ volatile ("sw $29,0($8)"   ::         :"$8","memory"); \
    __asm__ volatile ("addiu $8,$8,-24"::         :"$8","memory"); \
    __asm__ volatile ("move $29,$8"    ::         :"$8","memory"); \
}

#define ResetSpadStack() { \
    __asm__ volatile ("addiu $29,$29,24":::"$29","memory"); \
    __asm__ volatile ("lw $29,0($29)"   :::"$29","memory"); \
}

#define GetStackAddr(addr) { \
    __asm__ volatile ("move $8,%0"  ::"r"(addr):"$8","memory"); \
    __asm__ volatile ("sw $29,0($8)"::         :"$8","memory"); \
}

/*
#define SetSpadStack(addr) { \
    __asm__ volatile ("move $8,%0"     ::"r"(addr):"$8","memory"); \
    __asm__ volatile ("sw $29,0($8)"   ::         :"$8","memory"); \
    __asm__ volatile ("addiu $8,$8,-24"::         :"$8","memory"); \
    __asm__ volatile ("move $29,$8"    ::         :"$8","memory"); \
}

#define ResetSpadStack() { \
    __asm__ volatile ("addiu $29,$29,24":::"$29","memory"); \
    __asm__ volatile ("lw $29,0($29)"   :::"$29","memory"); \
}

#define GetStackAddr(addr) { \
    __asm__ volatile ("move $8,%0"  ::"r"(addr):"$8","memory"); \
    __asm__ volatile ("sw $29,0($8)"::         :"$8","memory"); \
}
*/