#include <stdio.h>

#include "pico/stdlib.h"
#include "bmp280.h"
#include "dht20.h"

#define I2C_BAUDRATE 400000

uint32_t i2c_setup();

int main() {
	// Bmp280CalibrationSettings b280_s;
	// stdio_init_all();
	// while (!stdio_usb_connected()) {};
	// printf("Starting setup...\n");
	// int baudrate = i2c_setup();
	// printf("Baudrate set: %d\n", baudrate);
	// // Init with 0.5ms standby time, full x16 filter
	// // temp 2x OVS, press 16x OVS, and normal mode
	// printf("Init answer: %d\n", bmp280_init(0b00011100, 0b01011111));
	// printf("Calibration values:\n");
	// bmp280_get_calibration_settings(&b280_s);
	// printf("\tdig_T1: %hu\n", b280_s.dig_T1);
	// printf("\tdig_T2: %hd\n", b280_s.dig_P2);
	// printf("\tdig_T3: %hd\n", b280_s.dig_P3);
	// printf("\tdig_P1: %hu\n", b280_s.dig_P1);
	// printf("\tdig_P2: %hd\n", b280_s.dig_P2);
	// printf("\tdig_P3: %hd\n", b280_s.dig_P3);
	// printf("\tdig_P4: %hd\n", b280_s.dig_P4);
	// printf("\tdig_P5: %hd\n", b280_s.dig_P5);
	// printf("\tdig_P6: %hd\n", b280_s.dig_P6);
	// printf("\tdig_P7: %hd\n", b280_s.dig_P7);
	// printf("\tdig_P8: %hd\n", b280_s.dig_P8);
	// printf("\tdig_P9: %hd\n", b280_s.dig_P9);

	// sleep_ms(1000);
	// uint32_t pressure, temperature;
	// while (1) {
	// 	printf("Test measurements:\n");
	// 	bmp280_read_temp(&temperature);
	// 	bmp280_read_press(&pressure);
	// 	printf("\tTest temp: %u\n", temperature);
	// 	printf("\tTest pressure: %u\n", pressure);
	// 	sleep_ms(1000);
	// }
	stdio_init_all();
	while (!stdio_usb_connected()) {};
	printf("Setting up I2C...\n");
	int baudrate = i2c_setup();
	printf("Baudrate set: %d\n", baudrate);
	// printf("Setting up display...\n");
	// sh1106_init();
	// sh1106_clock_freq(0b01010000);
	// sh1106_set_page_number(4);
	// uint8_t buf[132] = {0xFF};
	// printf("%d\n", sh1106_write_bytes(buf, 132));
	// for (uint8_t i = 0; i < 8; i++) {
	// 	sh1106_set_page_number(i);
	// 	uint8_t buf[132] = {0xFF};
	// 	printf("%hhu, %d, %hhu\n", i, sh1106_write_bytes(buf, 132), *buf);
	// 	sleep_ms(50);
	// }
	// sh1106_clear_display();
	// printf("Display cleared\n");

	// sleep_ms(2000);
	// printf("Drawing rectangle 1...\n");
	// sh1106_draw_rectangle(20, 20, 20, 20, 0);

	// sleep_ms(2000);
	// printf("Drawing rectangle 2...\n");
	// sh1106_draw_rectangle(0, 50, 132, 1, 0);

	// printf("Rectangles should have appeared on the screen\n");
	// sleep_ms(2000);

	// for (uint8_t i = 0; i<8;i++) {
	// 	sh1106_set_page_number(i);
	// 	sh1106_set_column_number(0);
	// 	for (uint8_t j = 0; j<132; j++) {
	// 		sh1106_set_column_number(j);
	// 		sh1106_write_byte(0xFF);
	// 	}
	// }
	dht20_init();
	uint32_t t, h;
	uint8_t s;
	while (1) {
		dht20_read_all(&t, &h);
		dht20_read_status(&s);
		printf("Status: %hhx\n", s);
		printf("\t%u, %u, %f°C, %f%%\n", t, h, (t/1048576.0)*200-500, (h/1048576.0)*100);
		sleep_ms(1000);
	}
	
	return 0;
}

uint32_t i2c_setup() {
	uint32_t ret = i2c_init(i2c1, I2C_BAUDRATE);
	gpio_set_function(14, GPIO_FUNC_I2C);
	gpio_set_function(15, GPIO_FUNC_I2C);
	// Obey i2c spec by pulling up the line, I guess ?
	gpio_pull_up(14);
	gpio_pull_up(15);
	return ret;
}