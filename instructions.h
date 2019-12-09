#include <stdint.h>

#ifndef LC3_INSTRUCTIONS_H
#define LC3_INSTRUCTIONS_H

void lc3_br(uint16_t instr);
void lc3_add(uint16_t instr);
void lc3_ld(uint16_t instr);
void lc3_st(uint16_t instr);
void lc3_jsr(uint16_t instr);
void lc3_and(uint16_t instr);
void lc3_ldr(uint16_t instr);
void lc3_str(uint16_t instr);
void lc3_not(uint16_t instr);
void lc3_ldi(uint16_t instr);
void lc3_sti(uint16_t instr);
void lc3_jmp(uint16_t instr);
void lc3_lea(uint16_t instr);
int lc3_trap(uint16_t instr);

#endif
