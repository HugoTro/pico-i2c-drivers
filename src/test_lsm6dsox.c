#include "lsm6dsox.h"

#define I2C_BAUDRATE 400000


uint32_t i2c_setup();

int main() {
	stdio_init_all();
	while (stdio_usb_connected()==0) {}

	printf("I2C setup: %u\n", i2c_setup());

	// 104Hz and ±8g, 104Hz and ±1000°/s
	lsm6dsox_init(0b01001100, 0b01001000);
	int16_t temperature = 0;
	while (1) {
		uint8_t whoishe = 0;
		lsm6dsox_get_whoami(&whoishe);
		lsm6dsox_get_temperature(&temperature);
		int16_t out_x = 0;
		lsm6dsox_read_bytes(0x22, (uint8_t *)&out_x, 2);
		printf("identifier: 0x%hhX, temperature: %hd\n\tout_x: %hd\n", whoishe, temperature, out_x);
		sleep_ms(1000);
	}
	
}

uint32_t i2c_setup() {
	uint32_t ret = i2c_init(LSM6DSOX_I2C_STRUCT, I2C_BAUDRATE);
	gpio_set_function(18, GPIO_FUNC_I2C);
	gpio_set_function(19, GPIO_FUNC_I2C);
	return ret;
}