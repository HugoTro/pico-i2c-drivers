#include "lsm6dsox.h"

uint8_t LSM6DSOX_ADDRESS;
i2c_inst_t *LSM6DSOX_I2C_STRUCT;
uint8_t LSM6DSOX_ACC_SENSITIVITY;
uint16_t LSM6DSOX_GYRO_SENSITIVITY;

void lsm6dsox_setup(i2c_inst_t *i2c_channel, uint8_t i2c_address) {
	LSM6DSOX_I2C_STRUCT = i2c_channel;
	LSM6DSOX_ADDRESS = i2c_address;
}

i2c_inst_t *lsm6dsox_get_i2c_struct() {
	return LSM6DSOX_I2C_STRUCT;
}

/*!
 * \brief Initializes the LSM6DSOX.
 * \param acc_odr one of LSM6DSOX_ACC_ODR_xxxHZ
 * \param acc_mode 0 or LSM6DSOX_ACC_ALLOW_ALL_MODES
 * \param acc_scale one of LSM6DSOX_ACC_SCALE_xxx
 * \param gyro_odr one of LSM6DSOX_GYRO_ODR_xxxHZ
 * \param gyro_mode 0 or LSM6DSOX_GYRO_ALLOW_ALL_MODES
 * \param gyro_scale one of LSM6DSOX_GYRO_SCALE_xxx
*/
int lsm6dsox_init(uint8_t acc_odr, uint8_t acc_mode, uint8_t acc_scale, uint8_t gyro_odr, uint8_t gyro_mode, uint8_t gyro_scale) {
	uint8_t ret = 0;
	// Disable MIPI I3C, we don't need it.
	ret += lsm6dsox_change_register(0x18, 0x03, 0x03);
	// Check parameters
	switch (acc_odr) {
		case LSM6DSOX_ACC_ODR_1_6HZ:
		case LSM6DSOX_ACC_ODR_12_5HZ:
		case LSM6DSOX_ACC_ODR_26HZ:
		case LSM6DSOX_ACC_ODR_52HZ:
		case LSM6DSOX_ACC_ODR_104HZ:
		case LSM6DSOX_ACC_ODR_208HZ:
		case LSM6DSOX_ACC_ODR_416HZ:
		case LSM6DSOX_ACC_ODR_833HZ:
		case LSM6DSOX_ACC_ODR_1660HZ:
		case LSM6DSOX_ACC_ODR_3330HZ:
		case LSM6DSOX_ACC_ODR_6660HZ:
			break;
		default:
			return -1;
	}
	switch (gyro_odr) {
		case LSM6DSOX_GYRO_ODR_12_5HZ:
		case LSM6DSOX_GYRO_ODR_26HZ:
		case LSM6DSOX_GYRO_ODR_52HZ:
		case LSM6DSOX_GYRO_ODR_104HZ:
		case LSM6DSOX_GYRO_ODR_208HZ:
		case LSM6DSOX_GYRO_ODR_416HZ:
		case LSM6DSOX_GYRO_ODR_833HZ:
		case LSM6DSOX_GYRO_ODR_1660HZ:
		case LSM6DSOX_GYRO_ODR_3330HZ:
		case LSM6DSOX_GYRO_ODR_6660HZ:
			break;
		default:
			return -2;
	}
	switch (acc_mode) {
		case LSM6DSOX_ACC_ALLOW_ALL_MODES:
		case 0:
			break;
		default:
			return -3;
	}
	switch (gyro_mode) {
		case LSM6DSOX_GYRO_ALLOW_ALL_MODES:
		case 0:
			break;
		default:
			return -3;
	}
	switch (acc_scale) {
		case LSM6DSOX_ACC_SCALE_2G:
			LSM6DSOX_ACC_SENSITIVITY = 2;
			break;
		case LSM6DSOX_ACC_SCALE_4G:
			LSM6DSOX_ACC_SENSITIVITY = 4;
			break;
		case LSM6DSOX_ACC_SCALE_8G:
			LSM6DSOX_ACC_SENSITIVITY = 8;
			break;
		case LSM6DSOX_ACC_SCALE_16G:
			LSM6DSOX_ACC_SENSITIVITY = 16;
			break;
		default:
			return -4;
	}
	switch (gyro_scale) {
		case LSM6DSOX_GYRO_SCALE_250DPS:
			LSM6DSOX_GYRO_SENSITIVITY = 250;
			break;
		case LSM6DSOX_GYRO_SCALE_500DPS:
			LSM6DSOX_GYRO_SENSITIVITY = 500;
			break;
		case LSM6DSOX_GYRO_SCALE_1000DPS:
			LSM6DSOX_GYRO_SENSITIVITY = 1000;
			break;
		case LSM6DSOX_GYRO_SCALE_2000DPS:
			LSM6DSOX_GYRO_SENSITIVITY = 2000;
			break;
		default:
			return -5;
	}
	// All parameters are checked, we can continue with the function.

	// set accelerometer mode (always high perf or as is described in table 51)
	// CTRL6_C
	ret += lsm6dsox_change_register(0x15, acc_mode, 0x10);

	// CTRL1_XL
	ret += lsm6dsox_change_register(0x10, acc_odr | acc_scale, 0xFC);

	// CTRL7_G
	ret += lsm6dsox_change_register(0x16, gyro_mode, 0x80);
	
	// CTRL2_G
	ret += lsm6dsox_change_register(0x11, gyro_odr | gyro_scale, 0xFE);
	return 0;
}

int lsm6dsox_reset() {
	// Send reset bit, while taking into account default value
	uint8_t reset_byte = 0b00000101;
	lsm6dsox_write_bytes(0x12, &reset_byte, 1);
	uint8_t ctrl3_c = 0;
	do {
		lsm6dsox_read_bytes(0x12, &ctrl3_c, 1);
		// printf("ctrl3_c: 0x%hhX\n", ctrl3_c);
		sleep_ms(500);
	} while (ctrl3_c%2==1);
	return 0;
}

int lsm6dsox_get_whoami(uint8_t *whoami) {
	return lsm6dsox_read_bytes(0x0F, whoami, 1);
}

int lsm6dsox_get_temperature(float *temp) {
	int16_t raw_value = 0;
	int ret = lsm6dsox_read_bytes(0x20, (uint8_t *)&raw_value, 2);
	*temp = raw_value/256.0 + 25;

	return ret;
}

int lsm6dsox_get_acceleration(lsm6dsoxAcceleration *acc) {
	int16_t raw_acc[3] = {0};
	int ret = lsm6dsox_read_bytes(0x28, (uint8_t *)raw_acc, 6);
	acc->x = raw_acc[0]*LSM6DSOX_ACC_SENSITIVITY/32768.0;
	acc->y = raw_acc[1]*LSM6DSOX_ACC_SENSITIVITY/32768.0;
	acc->z = raw_acc[2]*LSM6DSOX_ACC_SENSITIVITY/32768.0;
	return ret;
}

int lsm6dsox_get_angular_velocity(lsm6dsoxAngVel *ang_vel) {
	int16_t raw_vel[3] = {0};
	int ret = lsm6dsox_read_bytes(0x22, (uint8_t *)raw_vel, 6);
	ang_vel->x = raw_vel[0]*LSM6DSOX_GYRO_SENSITIVITY/32768.0;
	ang_vel->y = raw_vel[1]*LSM6DSOX_GYRO_SENSITIVITY/32768.0;
	ang_vel->z = raw_vel[2]*LSM6DSOX_GYRO_SENSITIVITY/32768.0;
	return ret;
}

/*!
 *	\brief Change only some bits of a register
 *	\param mask bits to change should have a 1 in their spot
 */
int lsm6dsox_change_register(uint8_t address, uint8_t new_val, uint8_t mask) {
	uint8_t val = 0;
	lsm6dsox_read_bytes(address, &val, 1);
	val &= ~(mask);
	val |= new_val & mask;
	return lsm6dsox_write_bytes(address, &val, 1);
}

int lsm6dsox_write_bytes(uint8_t address, uint8_t *data, uint8_t data_len) {
	// See Table 12
	if (data_len==1) {
		uint8_t buffer[2] = {address, *data};
		int ret = i2c_write_blocking(LSM6DSOX_I2C_STRUCT, LSM6DSOX_ADDRESS, buffer, 2, 0);
		// printf("Code for value 0x%hhX to reg 0x%hhX [%hhX]: %d\n", *data, data_len, address, ret);
		uint8_t read_value = 0;
		lsm6dsox_read_bytes(address, &read_value, 1);
		// printf("Confirming written value:\n\twanted: 0x%hhX\n\tactual: 0x%hhX\n", *data, read_value);
		return ret;
	}
	return -1;
}

int lsm6dsox_read_bytes(uint8_t address, uint8_t *data, uint8_t data_len) {
	// See Table 14
	i2c_write_blocking(LSM6DSOX_I2C_STRUCT, LSM6DSOX_ADDRESS, &address, 1, 1);
	return i2c_read_blocking(LSM6DSOX_I2C_STRUCT, LSM6DSOX_ADDRESS, data, data_len, 0);
}