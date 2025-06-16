#include "lsm6dsox.h"

#define I2C_BAUDRATE 400000
#define I2C_STRUCT i2c1

uint32_t i2c_setup();

int main() {
	stdio_init_all();
	while (stdio_usb_connected()==0) {}

	printf("I2C setup: %u\n", i2c_setup());

	lsm6dsox_setup(I2C_STRUCT, 0x6A);
	lsm6dsox_init(LSM6DSOX_ACC_ODR_104HZ, LSM6DSOX_ACC_ALLOW_ALL_MODES, LSM6DSOX_ACC_SCALE_8G, LSM6DSOX_GYRO_ODR_104HZ, LSM6DSOX_GYRO_ALLOW_ALL_MODES, LSM6DSOX_GYRO_SCALE_1000DPS);
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
	uint32_t ret = i2c_init(I2C_STRUCT, I2C_BAUDRATE);
	gpio_set_function(18, GPIO_FUNC_I2C);
	gpio_set_function(19, GPIO_FUNC_I2C);
	return ret;
}