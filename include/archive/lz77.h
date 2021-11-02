// PSX 'Bare Metal' LZ77 Decode Algorithm by krom (Peter Lemon)
// C Header Version by Arthur Carvalho de Souza Lima
// Source: https://github.com/PeterLemon/PSX/tree/master/Compress/LZ77/LZ77Decode
#ifndef _LZ77_H_
#define _LZ77_H_

unsigned long * asm_lzdecompress(unsigned long * src, unsigned long * dest);

#endif