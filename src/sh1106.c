#include "sh1106.h"

int sh1106_start() {
	uint8_t startup_seq[10] = {
		0b10000000, // not last instruction
		0b10101101, // Control mode set DC-DC converter
		0b10000000, // not last instruction
		0b10001011, // Set DC-DC converter ON
		0b10000000, // not last instruction
		0b10101111, // Display ON
		0b10000000, // not last instruction
		0b10100110, // Display not color inverted
		0b10000000,	// !last inst.
		0b11001000, // Invert scanning
	};
	int ret = i2c_write_blocking(I2C_STRUCT, SH1106_ADDRESS, startup_seq, 10, 0);
	// Make sure first orders are only sent 100ms after init sequence
	sleep_ms(100);
	return ret;
}

int sh1106_screen_on() {
	uint8_t commands[2] = {
		0b00000000,
		0b10101111,
	};
	return i2c_write_blocking(I2C_STRUCT, SH1106_ADDRESS, commands, 2, 0);
}

int sh1106_screen_off() {
	uint8_t commands[2] = {
		0b00000000,
		0b10101110,
	};
	return i2c_write_blocking(I2C_STRUCT, SH1106_ADDRESS, commands, 2, 0);
}

/*
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
	return i2c_write_blocking(I2C_STRUCT, SH1106_ADDRESS, commands, 4, 0);
}

int sh1106_set_column_number(uint8_t column_number) {
	if (column_number>132) {
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
	return i2c_write_blocking(I2C_STRUCT, SH1106_ADDRESS, commands, 4, 0);
}

int sh1106_set_page_number(uint8_t page_number) {
	// printf("Page number %hhu set.\n", page_number);
	uint8_t commands[2] = {
		0b00000000,
		0b10110000,
	};
	commands[1] |= (page_number & 0b00000111);
	return i2c_write_blocking(I2C_STRUCT, SH1106_ADDRESS, commands, 2, 0);
}

int sh1106_write_pixel(uint8_t x, uint8_t y, uint8_t value) {
	if (x>=132 || y>=64) {
		return -1;
	}
	value &= 0x01;
	sh1106_set_page_number(y/8);
	sh1106_set_column_number(x);
	return sh1106_write_byte((value << y%8));
}

int sh1106_write_byte(uint8_t byte) {
	uint8_t commands[6] = {
		0b10000000,
		0b11100000,
		0b11000000,
		byte,
		0b00000000,
		0b11101110,
	};
	return i2c_write_blocking(I2C_STRUCT, SH1106_ADDRESS, commands, 6, 0);
}

/*
 *	\brief Write multiple bytes to display
 *	\param buffer well, the buffer
 *	\param len_bytes length of the byte array
 */
int sh1106_write_bytes(uint8_t *buffer, uint8_t len_bytes) {
	uint8_t commands[2] = {
		// ctrl byte
		0b00000000,
		// RMW mode
		0b11100000,
	};

	// Send preliminary command
	i2c_write_blocking(I2C_STRUCT, SH1106_ADDRESS, commands, 2, 0);

	// Add ctrl byte and send buffer
	*buffer = 0b01000000;
	i2c_write_blocking(I2C_STRUCT, SH1106_ADDRESS, buffer, len_bytes, 0);

	// get out of RMW mode
	commands[1] = 0b11101110; // cmd
	// Send closing command
	return i2c_write_blocking(I2C_STRUCT, SH1106_ADDRESS, commands, 2, 0);
}

void sh1106_clear_display() {
	uint8_t clear_array[132] = {0};
	for (uint8_t i = 0; i < 8; i++) {
		sh1106_set_page_number(i);
		sh1106_write_bytes(clear_array, 132);
	}
}