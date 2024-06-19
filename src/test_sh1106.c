#include <stdio.h>

#include "pico/stdlib.h"
#include "sh1106.h"

#define I2C_BAUDRATE 400000

uint32_t i2c_setup();

int main() {

	// Preliminary setup (debug + i2c)
	stdio_init_all();
	// while (!stdio_usb_connected()) {};
	printf("Setting up I2C...\n");
	int baudrate = i2c_setup();
	printf("Baudrate set: %d\n", baudrate);

	// Setting up display
	printf("Setting up display...\n");
	sh1106_init();

	sh1106_reverse_display(1);

	// Clock to f_osc (0101) and divider = 1 (0000)
	// sh1106_clock_freq(0b01010000);

	// Reset settings
	printf("set page number: %d\n", sh1106_set_page_number(0));
	printf("set column number: %d\n", sh1106_set_column_number(0));

	uint8_t buf[132] = {0xFF};
	// printf("%d\n", sh1106_write_bytes(buf, 132));
	for (uint8_t i = 0; i < 8; i++) {
		printf("\tset page number: %d\n", sh1106_set_page_number(i));
		sh1106_set_column_number(0);
		printf("\t%hhu, %d\n", i, sh1106_write_bytes(buf, 132));
		sleep_ms(500);
	}
	sh1106_clear_display();
	printf("Display cleared\n");

	// sleep_ms(2000);
	// printf("Drawing rectangle 1...\n");
	// sh1106_draw_rectangle(20, 20, 20, 20, 0);

	// sleep_ms(2000);
	// printf("Drawing rectangle 2...\n");
	// sh1106_draw_rectangle(0, 50, 132, 1, 0);

	// printf("Rectangles should have appeared on the screen\n");
	// sleep_ms(2000);

	for (uint8_t i = 0; i<8;i++) {
		sh1106_set_page_number(i);
		sh1106_set_column_number(0);
		for (uint8_t j = 0; j<132; j++) {
			sh1106_set_column_number(j);
			sh1106_write_byte(0x00);
		}
	}

	uint8_t a = 0;
	while (true) {
		sh1106_set_page_number(4);
		printf("Starting scan...\n");
		scanf("%hhu", &a);
		printf("Column number: %hhu\n", a);
		printf("Result: %d\n", sh1106_set_column_number(a));
		printf("Result: %d\n", sh1106_write_byte(0));
	}
	
	return 0;
}

uint32_t i2c_setup() {
	uint32_t ret = i2c_init(i2c0, I2C_BAUDRATE);
	gpio_set_function(20, GPIO_FUNC_I2C);
	gpio_set_function(21, GPIO_FUNC_I2C);
	// Obey i2c spec by pulling up the line, I guess ?
	// gpio_pull_up(20);
	// gpio_pull_up(21);
	return ret;
}