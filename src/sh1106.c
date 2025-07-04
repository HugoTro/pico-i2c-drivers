#include "sh1106.h"

// Array of 2 frames, 8 pages, and 132 columns for buffering
uint8_t frame_buffer[8][132] = {0};

Font *current_font = NULL;

int sh1106_init() {

	current_font = &default_font;
	// DC-DC part
	uint8_t instructions[3] = {
		0b00000000,	// last, only data to follow
		0b10101101,	// DC-DC control mode set
		0b10001011	// Actually enable DC-DC
	};
	i2c_write_blocking(SH1106_I2C_STRUCT, SH1106_ADDRESS, instructions, 3, 0);
	

	// Make sure first orders are sent at least 100ms after init sequence
	sleep_ms(100);
	sh1106_clear_display();
	sh1106_display_on();
	return 0;
}

int sh1106_display_on() {
	const uint8_t commands[2] = {
		0b00000000,
		0b10101111,
	};
	return i2c_write_blocking(SH1106_I2C_STRUCT, SH1106_ADDRESS, commands, 2, 0);
}

int sh1106_display_off() {
	const uint8_t commands[2] = {
		0b00000000,
		0b10101110,
	};
	return i2c_write_blocking(SH1106_I2C_STRUCT, SH1106_ADDRESS, commands, 2, 0);
}

/*!
 *	\brief Function to set the clock divider, as percentage
 *	\param value See sh1106 datasheet to set that correctly
*/
int sh1106_clock_freq(uint8_t value) {
	uint8_t commands[4] = {
		0b10000000, // not last instruction
		0b11010101, // set "Divide Ratio/Oscillator Frequency Mode"
		0b00000000, // last instruction
		value, // whole byte is the argument
	};
	return i2c_write_blocking(SH1106_I2C_STRUCT, SH1106_ADDRESS, commands, 4, 0);
}

int sh1106_reverse_display(uint8_t on) {
	uint8_t commands[2] = {
		0b00000000,
		0b10100110,
	};
	commands[1] |= (0x01 & on);
	return i2c_write_blocking(SH1106_I2C_STRUCT, SH1106_ADDRESS, commands, 2, 0);
}

int sh1106_set_contrast(uint8_t constrast) {
	uint8_t commands[4] = {
		0b10000000,
		0b10000001,
		0b00000000,
		constrast,
	};
	return i2c_write_blocking(SH1106_I2C_STRUCT, SH1106_ADDRESS, commands, 4, 0);
}

int sh1106_set_column_number(uint8_t column_number) {
	if (column_number>=132) {
		return -1;
	}
	uint8_t commands[4] = {
		0b10000000,
		0b00010000,
		0b00000000,
		0b00000000,
	};
	// MSBs in 1st data byte
	commands[1] |= ((column_number & 0xF0) >> 4);
	// LSBs in 2nd data byte
	commands[3] |= (column_number & 0x0F);
	return i2c_write_blocking(SH1106_I2C_STRUCT, SH1106_ADDRESS, commands, 4, 0);
}

/*!
 *	\brief Sets the page number
 *	\param page_number Page number, between 0 and 7 inclusive
 *	\returns The result of the i2c_write_blocking() used
*/
int sh1106_set_page_number(uint8_t page_number) {
	// printf("Page number %hhu set.\n", page_number);
	uint8_t commands[2] = {
		0b00000000,
		0b10110000,
	};
	commands[1] |= (page_number & 0b00000111);
	return i2c_write_blocking(SH1106_I2C_STRUCT, SH1106_ADDRESS, commands, 2, 0);
}

/*!
 *	\brief Write a single pixel to the display, erasing the byte containing it
 *	\param x Position on the x axis [0, 131]
 *	\param y Position on the y axis [0, 63]
 *	\returns The result of the sh1106_write_byte() function used last.
 *	
 *	Note: This function uses multiples communication functions, so the return value does not represent the real success/failure of the function but is a good guess
*/
int sh1106_write_pixel(uint8_t x, uint8_t y, uint8_t value) {
	if (x>=132 || y>=64) {
		return -1;
	}
	value &= 0x01;
	sh1106_set_page_number(y/8);
	sh1106_set_column_number(x);
	return sh1106_write_byte((value << y%8));
}

int sh1106_set_pixel(uint8_t x, uint8_t y, uint8_t value) {
	if (x>=132 || y>=64) {
		return -1;
	}
	value &= 0x01;
	if (value) {
		frame_buffer[y/8][x] |= (1 << y%8);
	} else {
		frame_buffer[y/8][x] &= ~(1 << y%8);
	}
	return 0;
}

int sh1106_set_letter(uint8_t x, uint8_t y, uint8_t letter) {
	if (x>131-8 || y>63-8) {
		return -1;
	}

	// i is for lines
	for (uint8_t i = 0; i < 8; i++) {
		// columns
		for (uint8_t j = 0; j < 8; j++) {
			// We'll have to use a mask and bit shift to get the actual bit value from the font
			uint8_t pixel_value = ((*current_font)[letter-SH1106_CHAR_MAPS_START][i] >> 7-j) & 0x01;
			sh1106_set_pixel(x+j, y+i, pixel_value);
		}
	}
	return 0;
}

/*!
 *	\brief Write a byte to the display
 *	\param byte The byte to write
 *	\returns The result of the i2c_write_blocking() function
*/
int sh1106_write_byte(uint8_t byte) {
	uint8_t commands[2] = {
		0b01000000,
		byte,
	};
	return i2c_write_blocking(SH1106_I2C_STRUCT, SH1106_ADDRESS, commands, 2, 0);
}

/*!
 *	\brief Write multiple bytes to display.
 *	\param buffer Pointer to the buffer of bytes to send
 *	\param len_bytes Length of the byte array
 */
int sh1106_write_bytes(uint8_t *buffer, uint8_t len_bytes) {
	#define COMMAND_OFFSET 1
	uint8_t byte_buffer[132+COMMAND_OFFSET];
	
	// Low cost (like in shitty, not in O(1) complexity) memcpy function
	for (uint8_t i = len_bytes+COMMAND_OFFSET-1; i>=COMMAND_OFFSET; i--) {
		*(byte_buffer+i) = *(buffer+i-COMMAND_OFFSET);	
	}
	// Add correct control byte
	*byte_buffer = 0b01000000;

	// Send data buffer
	int res = i2c_write_blocking(SH1106_I2C_STRUCT, SH1106_ADDRESS, byte_buffer, len_bytes+COMMAND_OFFSET, 0);
	return res;
}

int sh1106_draw_rectangle(uint8_t pos_x, uint8_t pos_y, uint8_t width, uint8_t height, uint8_t color) {
	// Verify coordinates are within theoretical boundaries (avoids overflow in the next step)
	if (width>132 || height>64 || pos_x>=132 || pos_y>=64) {
		return -1;
	}
	// Once sure that nothing will make the test overflow, verify the rectangle can fit
	if (132-width<pos_x || 64-height<pos_y) {
		return -1;
	}
	uint8_t byte_array[132] = {0};

	// If the rectangle fits on the same page, we can optimise performance with that
	if (pos_y/8 == (pos_y+height)/8) {
		sh1106_set_page_number(pos_y/8);
		sh1106_set_column_number(pos_x);
		for (uint8_t i = 0; i < width; i++) {
			byte_array[i] = (0xFF << (pos_y%8)) & (0xFF >> (8-(pos_y+height)%8));
		}
		return sh1106_write_bytes(byte_array, width);
	}

	// Otherwise it ends up here, will treat the outer pages first and then the inner ones - if applicable

	// Lower side bytes
	sh1106_set_page_number(pos_y/8);
	sh1106_set_column_number(pos_x);
	for (uint8_t i = 0; i < width; i++) {
		byte_array[i] = 0xFF << (pos_y%8);
	}
	sh1106_write_bytes(byte_array, width);

	// Upper side bytes
	sh1106_set_page_number((pos_y+height)/8);
	sh1106_set_column_number(pos_x);
	for (uint8_t i = 0; i < width; i++) {
		byte_array[i] = 0xFF >> (8-(pos_y+height)%8);
	}
	sh1106_write_bytes(byte_array, width);

	// Inner bytes array filling
	for (uint8_t i = 0; i < width; i++) {
		byte_array[i] = 0xFF;
	}

	// Inner bytes bytes sending
	for (uint8_t page = pos_y/8+1; page < (pos_y+height)/8; page++) {
		sh1106_set_page_number(page);
		sh1106_set_column_number(pos_x);
		sh1106_write_bytes(byte_array, width);
	}
	return 0;
}

int sh1106_blit() {
	for (uint8_t i = 0; i < 8; i++) {
		sh1106_set_page_number(i);
		sh1106_set_column_number(0);
		sh1106_write_bytes(frame_buffer[i], 132);
	}
	return 0;
}

void sh1106_clear_display() {
	uint8_t buffer[132] = {0};
	for (uint8_t i = 0; i < 8; i++) {
		sh1106_set_page_number(i);
		sh1106_set_column_number(0);
		sh1106_write_bytes(buffer, 132);
	}
}