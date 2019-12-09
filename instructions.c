#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "architecture.h"
#include "functions.h"

/*
 Useful vocabulary:

 - BaseR		Base Register
 - DR			Destination Register
 - imm5			A 5-bit immediate value
 - setcc()		Sets condition codes
 - SEXT(A)		Sign-extend A
 - SR, SR1, SR2		Source Register
 - ZEXT(A)		Zero-extend A
 */

/* Conditional Branch: BR - 0000 */
/*
	if ((n AND N) OR (z AND Z) OR (p AND P))
		PC = PC + SEXT(PCoffset9);
 */
void lc3_br(uint16_t instr)
{
	uint16_t cond = (instr >> 9) & 0x7;
	if(cond & reg[R_COND])
	{
		uint16_t pc_offset_9 = sign_extend(instr & 0x1FF, 9);
		reg[R_PC] += pc_offset_9;
	}
}

/* Addition: ADD - 0001 */
/*
	if (bit[5]==0)
		DR = SR1 + SR2;
	else
		DR = SR1 + SEXT(imm5);
		setcc();
 */
void lc3_add(uint16_t instr)
{
	uint16_t dr = (instr >> 9) & 0x7;
	uint16_t sr1 = (instr >> 6) & 0x7;
	if ( instr & 0x20 ) /* Is this faster than '(instr >> 5) & 0x1' ?*/
	{
		uint16_t imm5 = sign_extend(instr & 0x1F, 5);
		reg[dr] = reg[sr1] + imm5;
	}
	else
	{
		uint16_t sr2 = instr & 0x7;
		reg[dr] = reg[sr1] + reg[sr2];
	}
	set_condition_codes(dr);
}

/* Load: LD - 0010 */
/*
	DR = mem[PC + SEXT(PCoffset9)];
	setcc();
 */
void lc3_ld(uint16_t instr)
{
	uint16_t dr = (instr >> 9) & 0x7;
	uint16_t pc_offset_9 = sign_extend(instr & 0x1FF, 9);
	reg[dr] = mem_read(reg[R_PC] + pc_offset_9);
	set_condition_codes(dr);
}

/* Store: ST - 0011 */
/*
	mem[PC + SEXT(PCoffset9)] = SR;
 */
void lc3_st(uint16_t instr)
{
	uint16_t sr = (instr >> 9) & 0x7;
	uint16_t pc_offset_9 = sign_extend(instr & 0x1FF, 9);
	mem_write(reg[R_PC] + pc_offset_9, reg[sr]);
}

/* Jump to Subroutine: JSR - 0100 */
/*
	R7 = PC;
	if (bit[11] == 0)
		PC = BaseR;
	else
		PC = PC + SEXT(PCoffset11);
 */
void lc3_jsr(uint16_t instr)
{
	reg[R_R7] = reg[R_PC];
	if ( (instr >> 11) & 0x1 )
	{
		uint16_t pc_offset_11 = sign_extend(instr & 0x7FF, 11);
		reg[R_PC] += pc_offset_11;
	}
	else
	{
		uint16_t base_r = (instr >> 6) & 0x7;
		reg[R_PC] = reg[base_r];
	}
}

/* Bit-wise Logical AND: AND - 0101 */
/*
	if (bit[5] == 0)
		DR = SR1 AND SR2;
	else
		DR = SR1 AND SEXT(imm5);
	setcc();
 */
void lc3_and(uint16_t instr)
{
	uint16_t dr = (instr >> 9) & 0x7;
	uint16_t sr1 = (instr >> 6) & 0x7;
	if ( instr & 0x20 )
	{
		uint16_t imm5 = sign_extend(instr & 0x1F, 5);
		reg[dr] = reg[sr1] & imm5;
	}
	else
	{
		uint16_t sr2 = instr & 0x7;
		reg[dr] = reg[sr1] & reg[sr2];
	}
	set_condition_codes(dr);
}

/* Load Base + offset: LDR - 0110 */
/*
	DR = mem[BaseR + SEXT(offset6)];
	setcc();
 */
void lc3_ldr(uint16_t instr)
{
	uint16_t dr = (instr >> 9) & 0x7;
	uint16_t base_r = (instr >> 6) & 0x7;
	uint16_t offset_6 = sign_extend(instr & 0x3F, 6);
	reg[dr] = mem_read(reg[base_r] + offset_6);
	set_condition_codes(dr);
}

/* Store Base + offset: STR - 0111 */
/*
	mem[BaseR + SEXT(offset6)] = SR;
 */
void lc3_str(uint16_t instr)
{
	uint16_t sr = (instr >> 9) & 0x7;
	uint16_t base_r = (instr >> 6) & 0x7;
	uint16_t offset_6 = sign_extend(instr & 0x3F, 6);
	mem_write(reg[base_r] + offset_6, reg[sr]);
}

/* Bit-wise Complement: NOT - 1001 */
/*
	DR = NOT(SR);
	setcc();
 */
void lc3_not(uint16_t instr)
{
	uint16_t dr = (instr >> 9) & 0x7;
	uint16_t sr = (instr >> 6 ) & 0x7;
	reg[dr] = ~reg[sr];
	set_condition_codes(dr);
}

/* Load Indirect: LDI - 1010 */
/*
	DR = mem[mem[PC + SEXT(PCoffset9)]];
	setcc();
 */
void lc3_ldi(uint16_t instr)
{
	uint16_t dr = (instr >> 9) & 0x7;
	uint16_t pc_offset_9 = sign_extend(instr & 0x1FF, 9);
	reg[dr] = mem_read(mem_read(reg[R_PC] + pc_offset_9));
	set_condition_codes(dr);
}

/* Store Indirect: STI - 1011 */
/*
	mem[mem[PC + SEXT(PCoffset9)]] = SR
 */
void lc3_sti(uint16_t instr)
{
	uint16_t sr = (instr >> 9) & 0x7;
	uint16_t pc_offset_9 = sign_extend(instr & 0x1FF, 9);
	mem_write(mem_read(reg[R_PC] + pc_offset_9), reg[sr]);
}

/* Jump: JMP - 1100 */
/*
	PC = BaseR;
 */
void lc3_jmp(uint16_t instr)
{
	uint16_t base_r = (instr >> 6) & 0x7;
	reg[R_PC] = reg[base_r];
}

/* Load Effective Address: LEA - 1110 */
/*
	DR = PC + SEXT(PCoffset9);
	setcc();
 */
void lc3_lea(uint16_t instr)
{
	uint16_t dr = (instr >> 9) & 0x7;
	uint16_t pc_offset_9 = sign_extend(instr & 0x1FF, 9);
	reg[dr] = reg[R_PC] + pc_offset_9;
	set_condition_codes(dr);
}

/* System Call: TRAP - 1111 */
/*
	R7 = PC;
	PC = mem[ZEXT(trapvect8)];
 */
int lc3_trap(uint16_t instr)
{
	reg[R_R7] = reg[R_PC];
	switch (instr & 0xFF)
	{
		case TRAP_GETC: {
			
			uint16_t c = (uint16_t)fgetc(stdin);
			reg[R_R0] = c & 0x00FF; /* Making sure the high eight bits of R0 are cleared */
			break;
		}
		case TRAP_OUT:
			fputc((char)reg[R_R0], stdout);
			fflush(stdout);
			break;

		case TRAP_PUTS: {
			/*
			 * A variable declared as an array of some type acts as a pointer to that type.
			 * When used by itself, it points to the first element of the array.
			 */
			/*
			 * R0 does not specify the char, but the memory address to the start of the string.
			 * We cannot use, however, the relative address given by R0. That is why we add R0 to the memory address of 'memory'.
			 */
			uint16_t *c = memory + reg[R_R0];
			while ( *c ) /* Writing terminates with the occurrence of x0000 in a memory location */
			{
				fputc((char)*c, stdout);
				++c;
			}
			fflush(stdout);
			break;
		}
		case TRAP_IN: {
			fputs("Enter a character: ", stdout);
			char c = fgetc(stdin); /* 'char' is stored in 1 byte */
			fputc(c, stdout);
			fflush(stdout);
			reg[R_R0] = (uint16_t)c; /* Assuming casting clears the high eight bits of R0 */
			break;
		}
		case TRAP_PUTSP: {
			uint16_t *c = memory + reg[R_R0];
			while ( *c )
			{
				char char1 = (*c) & 0xFF;
				fputc(char1, stdout); /* 'The ASCII code contained in bits [7:0] of a memory location is written to the console first' */
				char char2 = (*c >> 8) & 0xFF;
				if ( char2 ) /* Check if high eight bits are x00. If they aren't, print the char stored in there. */
				{
					fputc(char2, stdout);
				}
				++c;
			}
			fflush(stdout);
			break;
		}
		case TRAP_HALT:
			fputs("HALT\n", stdout);
			fflush(stdout);
			return 0;
	}
	return 1;
}
