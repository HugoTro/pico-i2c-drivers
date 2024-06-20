#ifndef SH1106_H
#define SH1106_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "hardware/i2c.h"

// MAX SH1106 I2C SPEED: 400kbit/s

// User parameters
#define SH1106_ADDRESS 0x3C
#define SH1106_I2C_STRUCT i2c0

// Function declarations
int sh1106_init();
int sh1106_display_on();
int sh1106_display_off();
int sh1106_clock_freq(uint8_t);
int sh1106_reverse_display(uint8_t);
int sh1106_set_contrast(uint8_t);
int sh1106_set_column_number(uint8_t);
int sh1106_set_page_number(uint8_t);
int sh1106_write_byte(uint8_t);
int sh1106_write_bytes(uint8_t *, uint8_t);
int sh1106_write_pixel(uint8_t, uint8_t, uint8_t);
int sh1106_set_pixel(uint8_t, uint8_t, uint8_t);
int sh1106_draw_rectangle(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
int sh1106_blit();
void sh1106_clear_display();

#endif