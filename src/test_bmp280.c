#include <stdio.h>

#include "pico/stdlib.h"
#include "bmp280.h"

#define I2C_BAUDRATE 400000

uint32_t i2c_setup();

int main() {

	// Preliminary setup (debug + i2c)
	stdio_init_all();
	while (!stdio_usb_connected()) {};
	printf("Setting up I2C...\n");
	int baudrate = i2c_setup();
	printf("Baudrate set: %d\n", baudrate);

	// Setting up display
	printf("Setting up bmp280...\n");
	uint8_t results[6];
	uint8_t ctrl_meas = 0b00100100;
	uint8_t config = 0b10000000;
	bmp280_init(ctrl_meas, config);

	while (1) {
		printf("Starting measurement...\n");
		bmp280_start_measurement_blocking();
		bmp280_read_results(results);
		Bmp280Results converted_results;
		bmp280_convert_results(results, &converted_results);
		printf("Temperature: %.4f°C\n", converted_results.temperature);
		printf("Pressure: %.6fhPa\n", converted_results.pressure/100.0);
		sleep_ms(1000);
	}
	
	return 0;
}

uint32_t i2c_setup() {
	uint32_t ret = i2c_init(i2c0, I2C_BAUDRATE);
	gpio_set_function(20, GPIO_FUNC_I2C);
	gpio_set_function(21, GPIO_FUNC_I2C);
	return ret;
}