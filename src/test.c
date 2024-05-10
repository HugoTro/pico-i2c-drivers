#include <stdio.h>

#include "pico/stdlib.h"
#include "bmp280.h"

#define I2C_BAUDRATE 400000

uint32_t i2c_setup();

int main() {
	uint16_t bmp280cal_settings[12] = {0};
	stdio_init_all();
	while (!stdio_usb_connected()) {};
	printf("Starting setup...\n");
	int baudrate = i2c_setup();
	printf("Baudrate set: %d\n", baudrate);
	// Init with 0.5ms standby time, full x16 filter
	// temp 2x OVS, press 16x OVS, and normal mode
	printf("Init answer: %d\n", bmp280_init(0b00011100, 0b00111111));
	printf("Calibration values:\n");
	bmp280_get_calibration_settings(bmp280cal_settings);
	printf("\tdig_T1: %hu\n", bmp280cal_settings[0]);
	printf("\tdig_T2: %hd\n", bmp280cal_settings[1]);
	printf("\tdig_T3: %hd\n", bmp280cal_settings[2]);
	printf("\tdig_P1: %hu\n", bmp280cal_settings[3]);
	printf("\tdig_P2: %hd\n", bmp280cal_settings[4]);
	printf("\tdig_P3: %hd\n", bmp280cal_settings[5]);
	printf("\tdig_P4: %hd\n", bmp280cal_settings[6]);
	printf("\tdig_P5: %hd\n", bmp280cal_settings[7]);
	printf("\tdig_P6: %hd\n", bmp280cal_settings[8]);
	printf("\tdig_P7: %hd\n", bmp280cal_settings[9]);
	printf("\tdig_P8: %hd\n", bmp280cal_settings[10]);
	printf("\tdig_P9: %hd\n", bmp280cal_settings[11]);

	sleep_ms(1000);
	uint32_t pressure, temperature;
	while (1) {
		printf("Test measurements:\n");
		bmp280_read_temp(&temperature);
		bmp280_read_press(&pressure);
		printf("\tTest temp: %u\n", temperature);
		printf("\tTest pressure: %u\n", pressure);
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