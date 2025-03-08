#include "lsm6dsox.h"

int lsm6dsox_init(uint8_t ctrl1_xl, uint8_t ctrl2_g) {
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

int lsm6dsox_get_whoami(uint8_t *whoami) {
	return lsm6dsox_read_bytes(0x0F, whoami, 1);
}

int lsm6dsox_get_temperature(int16_t *temp) {
	return lsm6dsox_read_bytes(0x20, (uint8_t *)temp, 2);
}

/*!
 *	\brief Change only some bits of a register
 *	\param mask bits to change should have a 1 in their spot
 */
int lsm6dsox_change_register(uint8_t address, uint8_t new_val, uint8_t mask) {
	uint8_t val = 0;
	lsm6dsox_read_bytes(address, &val, 1);
	// printf("Register 0x%hhX:\n\told value: 0x%hhX\n", address, val);
	val &= ~(mask);
	val |= new_val & mask;
	// printf("\tnew value: 0x%hhX\n", val);
	return lsm6dsox_write_bytes(address, &val, 1);
}

int lsm6dsox_write_bytes(uint8_t address, uint8_t *data, uint8_t data_len) {
	// See Table 12
	return i2c_write_blocking(LSM6DSOX_I2C_STRUCT, LSM6DSOX_ADDRESS, data, data_len, 0);
}

int lsm6dsox_read_bytes(uint8_t address, uint8_t *data, uint8_t data_len) {
	// See Table 14
	i2c_write_blocking(LSM6DSOX_I2C_STRUCT, LSM6DSOX_ADDRESS, &address, 1, 1);
	return i2c_read_blocking(LSM6DSOX_I2C_STRUCT, LSM6DSOX_ADDRESS, data, data_len, 0);
}