#include <stdio.h>

#include "pico/stdlib.h"
#include "bmp280.h"
#include "bme680.h"
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
	printf("Setting up bmp280...\n");
	uint8_t results[6];
	uint8_t ctrl_meas = 0b00100100;
	uint8_t config = 0b10000000;
	bmp280_init(ctrl_meas, config);
	printf("bmp280 setup completed.\n");

	Bme680Results bme680_results;
	int status = bme680_init(0b01100100, 0b00000001);
	if (status) {
		printf("Could not intialize bme680 with error code %d\n", status);
		while (1) {}
	}
	bme680_init_calibration_settings();
	printf("bme680 initialization complete.\n");

	// Setting up display
	// printf("Setting up display...\n");
	// sh1106_init();

	// sh1106_set_contrast(5);

	// sh1106_clear_display();

	uint8_t pos[2] = {0};
	char char_buf[10] = {0};

	while (1) {
		printf("Starting measurement...\n");
		bmp280_start_measurement_blocking();
		// // sh1106_clear_display();
		bmp280_read_results(results);
		Bmp280Results converted_results;
		bmp280_convert_results(results, &converted_results);
		printf("bmp280 temperature: %.4f°C\n", converted_results.temperature);
		printf("bmp280 pressure: %.6fhPa\n", converted_results.pressure/100.0);

		// BME680 measurements
		bme680_start_measurement_non_blocking();
		bme680_read_results(&bme680_results);

		printf("bme680 temperature: %.6f°C\n", bme680_results.temperature);
		printf("Current difference: %.6f°C\n\n", bme680_results.temperature-converted_results.temperature);

		pos[0] = 4;
		pos[1] = 4;
		uint16_t lot_temperature = (uint16_t)(converted_results.temperature*1000.0);
		uint16_t lot_pressure = (uint16_t)(converted_results.pressure);

		// snprintf(char_buf, 10, "%hu", lot_temperature);

		for (uint8_t i = 0; i < 5; i++) {
			// sh1106_set_letter(pos[0], pos[1], char_buf[i]);
			pos[0]+=10;
			if (i==1) {
				// sh1106_set_letter(pos[0], pos[1], '.');
				pos[0]+=10;
			}
		}
		// sh1106_set_letter(pos[0], pos[1], 'C');

		// sh1106_blit();

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