#include <stdint.h>

#ifndef LC3_ARCHITECTURE_H
#define LC3_ARCHITECTURE_H

/*
  "Memory address: space 16 bits, corresponding to 2^16 (65536) locations, each containing one word (16 bits)."
  In this program one word is represented with the type 'uint16_t'.
  Memory map of LC-3:
  - x0000..x00FF: Trap Vector Table
  - x0100..x01FF: Interrupt Vector Table
  - x0200..x2FFF: Operating System and Supervisor Stack
  - x3000..xFDFF: Available fo user programs
  - xFE00..xFFFF: Device register addresses
 */
uint16_t memory[UINT16_MAX];

/*
  According to documentation, LC-3 has 12 registers:
  - Eight 16-bit registers, numbered from 000 to 111.
  - A Program Counter (PC), 16-bit register containing the address of the next instruction to be processed.
  - Condition Codes, three 1 bit registers: N (Negative), Z (Zero) and P (Positive). And are set if the result, taken 16-bit 2's compliment integer, is negative, positive or zero.
 */

enum
  {
   R_R0 = 0,
   R_R1,
   R_R2,
   R_R3,
   R_R4,
   R_R5,
   R_R6,
   R_R7,
   R_PC,
   /*
     Instead of using three 1-bit registers for the Condition Codes, we will use one 16-bit register.
     This makes easier handling the registers as they would have the same type.
    */
   R_COND,
   R_COUNT /* size of the register: 10 */
  };
/* The registers are stored in an array (same as memory) */
uint16_t reg[R_COUNT];

/*
  Each 16-bit instruction consists of an opcode (bits[15:12]) plus 12 additional bits to specify the other information that is needed to carry out the work of that instruction.
  The opcode runs from 0000b to 1111b.
 */

enum
  {
   OP_BR = 0, /* Conditional Branch */
   OP_ADD,    /* Addition */
   OP_LD,     /* Load */
   OP_ST,     /* Store */
   OP_JSR,    /* Jump to Subroutine */
   OP_AND,    /* Bit-wise Logical AND */
   OP_LDR,    /* Load Base + offset */
   OP_STR,    /* Store Base + offset */
   OP_RTI,    /* Return from Interrumpt (unused) */
   OP_NOT,    /* Bit-wise Complement */
   OP_LDI,    /* Load Indirect */
   OP_STI,    /* Store Indirect */
   OP_JMP,    /* Jump */
   OP_RES,    /* reserved (unused) */
   OP_LEA,    /* Load Effective Address */
   OP_TRAP    /* System Call */
  };

/*
  The possible values are: 100 (N = 1; Z,P = 0), 010 (Z = 0; N,P = 0) and 001 (P = 1; N,Z = 0).
  The values of the Condition Codes are set depending on the result of a load (LD, LDI, LDR, and LEA) or operate (ADD, AND, NOT) instruction.
  Said result, taken as a 16-bit 2's complement integer, will set Z=1 if it is 0x0000, N=1 if the left-most bit is 1 and P=1 the rest of cases.
 */
enum
{
   FL_POS = 1 << 0, /* P */
   FL_ZRO = 1 << 1, /* Z */
   FL_NEG = 1 << 2, /* N */
};

/* Trap Vector Table: x0000..x00FF */
enum
{
   /*
     Read a single character from the keyboard.
     The character is not echoed onto the console.
     Its ASCII code is copied into R0.
     The high eight bits of R0 are cleared.
   */
   TRAP_GETC = 0x20,

   /*
     Write a character in R0[7:0] to the console display.
   */
   TRAP_OUT = 0x21,

   /*
     Write a string of ASCII characters to the console display.
     The characters are containedin consecutive memory locations, one character per memory location, starting with the address specified in R0.
     Writing terminates with the occurrence of x0000 in amemory location.
   */
   TRAP_PUTS = 0x22,

   /*
     Print a prompt on the screen and read a single character from the keyboard.
     The character is echoed onto the console monitor, and its ASCII code is copied into R0.
     The high eight bits of R0 are cleared.
    */
   TRAP_IN = 0x23,

   /*
     Write a string of ASCII characters to the console.
     The characters are contained inconsecutive memory locations, two characters per memory location, starting with the address specified in R0.
     The ASCII code contained in bits [7:0] of a memory location is written to the console first.
     Then the ASCII code contained in bits [15:8] of that memory location is written to the console.
     A character string consisting of an odd number of characters to be written will have x00 in bits [15:8] of the memory location containing the last character to be written.
     Writing terminates with the occurrence of x0000 in a memory location.
    */
   TRAP_PUTSP = 0x24,

   /*
     Halt execution and print a message on the console.
    */
   TRAP_HALT = 0x25
};

#endif
