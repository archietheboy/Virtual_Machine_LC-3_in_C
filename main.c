#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

#include "architecture.h"
#include "functions.h"
#include "instructions.h"

int main(int argc, const char *argv[])
{
	/* LOAD ARGUMENTS */
	if (argc < 2)
	{
		/* Print usage string */
		puts("lc3 [image-file1] ...");
		exit(2);
	}
	
	for (int j = 1; j < argc; ++j)
	{
		if ( read_image(argv[j]) )
		{
			printf("failed to load image: %s\n", argv[j]);
			exit(1);
		}
	}

	/* SETUP */ 
	lc3_setup();
	
	/* 0x3000 is the start of the 'Available for user programs' block in 'memory' */
	reg[R_PC] = 0x3000;

	int running = 1;
	while ( running )
	{
		/* Fetch instruction */
		uint16_t instr = mem_read(reg[R_PC]++); /* PC will be incremented after extracting the instr */
		switch ( instr >> 12 ) /* Bits [15:12] indicate the opcode */
		{
			case OP_BR:
				lc3_br(instr);
				break;
			case OP_ADD:
				lc3_add(instr);
				break;
			case OP_LD:
				lc3_ld(instr);
				break;
			case OP_ST:
				lc3_st(instr);
				break;
			case OP_JSR:
				lc3_jsr(instr);
				break;
			case OP_AND:
				lc3_and(instr);
				break;
			case OP_LDR:
				lc3_ldr(instr);
				break;
			case OP_STR:
				lc3_str(instr);
				break;
			case OP_RTI:
			case OP_NOT:
				lc3_not(instr);
				break;
			case OP_LDI:
				lc3_ldi(instr);
				break;
			case OP_STI:
				lc3_sti(instr);
				break;
			case OP_JMP:
			       lc3_jmp(instr);
				break;
		 	case OP_RES:
			case OP_LEA:
				lc3_lea(instr);
				break;
			case OP_TRAP:
				running = lc3_trap(instr);
				break;
			default:
				/* Bad opcode */
				abort();
				break;
		}
	}
	restore_input_buffering();
	return 0;
}
