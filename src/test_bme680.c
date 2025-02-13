#include <stdio.h>

#include "pico/stdlib.h"

#include "bme680.h"

#define I2C_BAUDRATE 400000

uint32_t i2c_setup();

int main() {
	stdio_init_all();
	// x1 ovs on all values
	bme680_init(0b00100100, 0b00000001);

}

uint32_t i2c_setup() {
	uint32_t ret = i2c_init(BME680_I2C_STRUCT, I2C_BAUDRATE);
	gpio_set_function(20, GPIO_FUNC_I2C);
	gpio_set_function(21, GPIO_FUNC_I2C);
	return ret;
}