#include "lsm6dsox.h"

uint8_t LSM6DSOX_ADDRESS;
i2c_inst_t *LSM6DSOX_I2C_STRUCT;

void lsm6dsox_setup(i2c_inst_t *i2c_channel, uint8_t i2c_address) {
	LSM6DSOX_I2C_STRUCT = i2c_channel;
	LSM6DSOX_ADDRESS = i2c_address;
}

int lsm6dsox_init(uint8_t ctrl1_xl, uint8_t ctrl2_g) {
	// Disable MIPI I3C
	lsm6dsox_change_register(0x18, 0x03, 0x03);
	// Change ctrl6_c to disable high preformance operation (only broadens the possible range of choices of accelerometer ODRs)
	lsm6dsox_change_register(0x15, 0x10, 0x10);

	// same for ctrl7_g
	lsm6dsox_change_register(0x16, 0x80, 0x80);

	// Change ctrl1_xl reg
	lsm6dsox_change_register(0x10, ctrl1_xl, 0xFC);

	// Change ctrl2_g reg
	lsm6dsox_change_register(0x11, ctrl2_g, 0xFE);
	return 0;
}

int lsm6dsox_reset() {
	// Send reset bit, while taking into account default value
	uint8_t reset_byte = 0b00000101;
	lsm6dsox_write_bytes(0x12, &reset_byte, 1);
	uint8_t ctrl3_c = 0;
	do {
		lsm6dsox_read_bytes(0x12, &ctrl3_c, 1);
		printf("ctrl3_c: 0x%hhX\n", ctrl3_c);
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
	ang_vel->x = raw_vel[0]*LSM6DSOX_ANG_SENSITIVITY/32768.0;
	ang_vel->y = raw_vel[1]*LSM6DSOX_ANG_SENSITIVITY/32768.0;
	ang_vel->z = raw_vel[2]*LSM6DSOX_ANG_SENSITIVITY/32768.0;
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
		printf("Code for value 0x%hhX to reg 0x%hhX [%hhX]: %d\n", *data, data_len, address, ret);
		uint8_t read_value = 0;
		lsm6dsox_read_bytes(address, &read_value, 1);
		printf("Confirming written value:\n\twanted: 0x%hhX\n\tactual: 0x%hhX\n", *data, read_value);
		return ret;
	}
	return -1;
}

int lsm6dsox_read_bytes(uint8_t address, uint8_t *data, uint8_t data_len) {
	// See Table 14
	i2c_write_blocking(LSM6DSOX_I2C_STRUCT, LSM6DSOX_ADDRESS, &address, 1, 1);
	return i2c_read_blocking(LSM6DSOX_I2C_STRUCT, LSM6DSOX_ADDRESS, data, data_len, 0);
}