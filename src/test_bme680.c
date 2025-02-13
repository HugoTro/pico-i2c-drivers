#include <stdio.h>

#include "pico/stdlib.h"

#include "bme680.h"

#define I2C_BAUDRATE 400000

uint32_t i2c_setup();

int main() {

	stdio_init_all();
	while (!stdio_usb_connected()) {}

	printf("I2C setup: %u\n", i2c_setup());
	// x1 ovs on all values
	int status = bme680_init(0b00100100, 0b00000001);
	if (status) {
		printf("Could not intialize bme680 with error code %d\n", status);
		while (1) {}
	}
	uint8_t chip_id = 0;
	bme680_get_chipid(&chip_id);
	printf("Chip ID: %hhu\n", chip_id);

	uint8_t debug_buffer = 0;
	bme680_read_bytes(0xE9, &debug_buffer, 1);
	printf("par_t1 LSB: 0x%hhX\n", debug_buffer);
	bme680_read_bytes(0xEA, &debug_buffer, 1);
	printf("par_t1 MSB: 0x%hhX\n", debug_buffer);

	bme680_read_bytes(0x8A, &debug_buffer, 1);
	printf("par_t2 LSB: 0x%hhX\n", debug_buffer);
	bme680_read_bytes(0x8B, &debug_buffer, 1);
	printf("par_t2 MSB: 0x%hhX\n", debug_buffer);

	bme680_read_bytes(0x8C, &debug_buffer, 1);
	printf("par_t3: 0x%hhX\n", debug_buffer);

	bme680_read_bytes(0x24, &debug_buffer, 1);
	printf("temp_adc XLSB: 0x%hhX\n", debug_buffer);
	bme680_read_bytes(0x23, &debug_buffer, 1);
	printf("temp_adc LSB: 0x%hhX\n", debug_buffer);
	bme680_read_bytes(0x22, &debug_buffer, 1);
	printf("temp_adc MSB: 0x%hhX\n", debug_buffer);

	bme680_init_calibration_settings();

	Bme680Results results;
	while (1) {
		bme680_start_measurement_non_blocking();
		bme680_read_results(&results);
		printf("Temperature: %.6f°C\n\n", results.temperature);
		sleep_ms(5000);
	}
	
	return 0;
}

uint32_t i2c_setup() {
	uint32_t ret = i2c_init(BME680_I2C_STRUCT, I2C_BAUDRATE);
	gpio_set_function(20, GPIO_FUNC_I2C);
	gpio_set_function(21, GPIO_FUNC_I2C);
	return ret;
}