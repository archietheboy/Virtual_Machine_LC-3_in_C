#include <stdio.h>
#include <stdlib.h> /* size_t */
#include <stdint.h> /* uint16_t */
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/termios.h>
#include <sys/mman.h>

#include "architecture.h"

/* SEXT(A): Sign-extend A
 	- The most significant bit of A is replicated as many times as necessary to extend A to 16 bits.
	- For example, if A=110000, then SEXT(A)=1111 11111111 0000.
 */
uint16_t sign_extend(uint16_t x, int bit_count)
{
	if ( x >> (bit_count - 1) & 1 ) /* To make sure we get 0x0 or 0x1 */
	{
		x |= (0xFFFF << bit_count);
	}
	/* Normally, you would have to also pad left with zeroes, but the casting to 'uint16_t' already does this */
	return x;
}

/* 'LC-3 programs are big-endian, but most of the modern computers we use are little endian.' */
uint16_t swap16_BE_to_LE(uint16_t x)
{
	return (x << 8) | (x >> 8);
}

/* setcc() */
void set_condition_codes(uint16_t r)
{
	if ( reg[r] == 0 )
	{
		reg[R_COND] = FL_ZRO;
	}
	else if ( reg[r] >> 15 ) /* A 2's complement integer is negative if its most left bit is 1 */
	{
		reg[R_COND] = FL_NEG;
	}
	else
	{
		reg[R_COND] = FL_POS;
	}
}

void read_image_file(FILE *file)
{
	/* The first 16 bits of the program file specify the address in memory where the program should start */
	uint16_t origin;
	fread(&origin, sizeof(origin), 1, file); /* The 3rd argument indicates how many objects of the specified size to read */
	origin = swap16_BE_to_LE(origin);
	
	/* As we know the maximum file size we can use 'fread' once by using its third argument */
	uint16_t max_read = UINT16_MAX - origin; /* How many memory locations left after starting in origin */
	uint16_t *p = memory + origin;
	/* 'fread' returns  the  number  of  elements  successfully  read, which  may  be less than the 3rd argument if a read error or end-of-file is encountered */
	size_t read = fread(p, sizeof(uint16_t), max_read, file);

	/* Swap all memory read from BE to LE */
	while ( read-- > 0 )
	{
		*p = swap16_BE_to_LE(*p);
		++p;
	}
}

int read_image(const char *image_path)
{
	FILE *file = fopen(image_path, "rb"); /* "rb": open binary file for reading */
	if ( !file ) { return 1; }
	read_image_file(file);
	fclose(file);
	return 0;
}

uint16_t check_key(void)
{
	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(STDIN_FILENO, &readfds);

	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
	return select(1, &readfds, NULL, NULL, &timeout) != 0;
}

void mem_write(uint16_t address, uint16_t val)
{
	memory[address] = val;
}


/* Memory mapped registers: special addresses that we interact directly, by writing or reading directly of their memory location */
enum
{
	/* Keyboard status register: the ready bit (bit [15]) indicates if the keyboard has received a new character */
	MR_KBSR = 0xFE00,
	/* Keyboard data register: bits [7:0] contain the last character typed on the keyboard */
	MR_KBDR = 0xFE02
};

uint16_t mem_read(uint16_t address)
{
	if ( address == MR_KBSR )
	{
		if ( check_key() )
		{
			memory[MR_KBSR] = (1 << 15); /* 0x8000 */
			memory[MR_KBDR] = fgetc(stdin);
		}
		else
		{
			memory[MR_KBSR] = 0;
		}

	}
	return memory[address];
}

struct termios original_tio;

void disable_input_buffering(void)
{
	tcgetattr(STDIN_FILENO, &original_tio);
	struct termios new_tio = original_tio;
	new_tio.c_lflag &= ~ICANON & ~ECHO;
	tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
}

void restore_input_buffering(void)
{
	tcsetattr(STDIN_FILENO, TCSANOW, &original_tio);
}

void handle_interrupt(int signal)
{
	restore_input_buffering();
	printf("\n");
	exit(-2);
}

void lc3_setup(void)
{
	signal(SIGINT, handle_interrupt);
	disable_input_buffering();
}
