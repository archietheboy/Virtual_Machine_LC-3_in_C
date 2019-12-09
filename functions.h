#include <stdint.h>

#ifndef LC3_FUNCTIONS_H
#define LC3_FUNCTIONS_H

uint16_t sign_extend(uint16_t x, int bit_count);
uint16_t swap16_BE_to_LE(uint16_t x);
void set_condition_codes(uint16_t r);
int read_image(const char *file_path);
void mem_write(uint16_t address, uint16_t val);
uint16_t mem_read(uint16_t address);
void restore_input_buffering(void);
void lc3_setup(void);

#endif
