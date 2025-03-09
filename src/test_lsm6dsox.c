#include "lsm6dsox.h"

#define I2C_BAUDRATE 400000


uint32_t i2c_setup();

int main() {
	stdio_init_all();
	while (stdio_usb_connected()==0) {}

	printf("I2C setup: %u\n", i2c_setup());

	// 104Hz and ±8g, 104Hz and ±1000°/s
	lsm6dsox_init(0b01001100, 0b01001000);
	lsm6dsoxAcceleration acc;
	lsm6dsoxAngVel ang_vel;
	float temperature = 0;
	while (1) {
		lsm6dsox_get_temperature(&temperature);
		lsm6dsox_get_acceleration(&acc);
		lsm6dsox_get_angular_velocity(&ang_vel);
		printf("Acceleration:\n\tx: %.3fg\n\ty: %.3fg\n\tz: %.3fg\n", acc.x, acc.y, acc.z);
		printf("Angular velocity:\n\tx: %.3f°/s\n\ty: %.3f°/s\n\tz: %.3f°/s\n", ang_vel.x, ang_vel.y, ang_vel.z);
		printf("temperature: %.3f°C\n\n", temperature);
		sleep_ms(100);
	}
}

uint32_t i2c_setup() {
	uint32_t ret = i2c_init(LSM6DSOX_I2C_STRUCT, I2C_BAUDRATE);
	gpio_set_function(18, GPIO_FUNC_I2C);
	gpio_set_function(19, GPIO_FUNC_I2C);
	return ret;
}