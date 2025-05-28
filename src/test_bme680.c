#include <stdio.h>

#include "pico/stdlib.h"

#include "bme680.h"

#define I2C_BAUDRATE 400000

uint32_t i2c_setup();

int main() {

	stdio_init_all();
	while (!stdio_usb_connected()) {}

	printf("I2C setup: %u\n", i2c_setup());
	// x1 ovs on all values but temperature: x4
	bme680_setup(i2c0, 0x76);
	int status = bme680_init(0b01101100, 0b00000001);
	if (status) {
		printf("Could not intialize bme680 with error code %d\n", status);
		while (1) {}
	}
	uint8_t chip_id = 0;
	bme680_get_chipid(&chip_id);
	printf("Chip ID: %hhu\n", chip_id);
	bme680_init_calibration_settings();
	bme680_print_calibration_setings();

	Bme680Results results;
	while (1) {
		bme680_start_measurement_non_blocking();
		bme680_read_results(&results);
		printf("Temperature: %.6f°C\nPressure: %.2f hPa\nHumidity: %.2f %%\n", results.temperature, results.pressure/100.0, results.humidity);
		sleep_ms(1000);
	}
	
	return 0;
}

uint32_t i2c_setup() {
	uint32_t ret = i2c_init(BME680_I2C_STRUCT, I2C_BAUDRATE);
	gpio_set_function(20, GPIO_FUNC_I2C);
	gpio_set_function(21, GPIO_FUNC_I2C);
	return ret;
}