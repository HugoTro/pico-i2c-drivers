#include "bmp280.h"
#include <string.h>
#include <stdio.h>

Bmp280CalibrationSettings bmp280_calibration_settings;
uint8_t initialised = 0;

int bmp280_init(uint8_t config, uint8_t control_meas) {

	// Config registers
	uint8_t reg_address = 0xF4;
	uint8_t conf_reg = 0;
	uint8_t params[2];

	// ctrl_meas reg setup (0xF4)
	params[0] = reg_address;
	params[1] = control_meas;
	i2c_write_blocking(BMP280_I2C_STRUCT, BMP280_ADDRESS, params, 2, 0);

	// config reg setup (0xF5)
	reg_address++;
	params[0] = reg_address;
	params[1] = (conf_reg & 0x02);
	params[1] |= (config & 0b11111101);
	i2c_write_blocking(BMP280_I2C_STRUCT, BMP280_ADDRESS, params, 2, 0);

	// Get calibration settings
	reg_address = 0x88;
	i2c_write_blocking(BMP280_I2C_STRUCT, BMP280_ADDRESS, &reg_address, 1, 0);
	initialised = 1;
	return i2c_read_blocking(BMP280_I2C_STRUCT, BMP280_ADDRESS, (uint8_t *)&bmp280_calibration_settings, 24 /*(12*sizeof(uint16_t))/sizeof(uint8_t)*/, 0);
}

int bmp280_reset() {
	uint8_t params[2] = {
		0xE0,
		0xB6
	};
	return i2c_write_blocking(BMP280_I2C_STRUCT, BMP280_ADDRESS, params, 2, 0);
}

int bmp280_get_calibration_settings(Bmp280CalibrationSettings *ptr) {
	if (initialised==0) {
		return -1;
	}
	memcpy(ptr, &bmp280_calibration_settings, sizeof(Bmp280CalibrationSettings));
	return 0;
}

int bmp280_read_temp(uint32_t *temp) {
	*temp = 0;
	if (initialised==0) {
		return -1;
	}
	const uint8_t reg_address = 0xFA;
	i2c_write_blocking(BMP280_I2C_STRUCT, BMP280_ADDRESS, &reg_address, 1, 0);
	int ret = i2c_read_blocking(BMP280_I2C_STRUCT, BMP280_ADDRESS, (uint8_t *)temp+1, 3, 0);
	// Fix memory alignment
	(*temp) >>= 4;
	(*temp) &= 0x000FFFFFu;

	printf("\tTemp readout: %x\n", *temp);

	printf("\t%d\n", *temp);

	// Convert meaningless value to temperature
		// double var1 = ((*temp)/16384.0-27504/1024.0)*((double)26435);
	double var1 = ((*temp)/16384.0-bmp280_calibration_settings.dig_T1/1024.0)*bmp280_calibration_settings.dig_T2;
		printf("\tvar1: %f\n", var1);
	double var2 = ((*temp)/131072.0-bmp280_calibration_settings.dig_T1/8192.0)*((*temp)/131072.0-bmp280_calibration_settings.dig_T1/8192.0)*bmp280_calibration_settings.dig_T3;
		// double var2 = ((*temp)/131072.0-27504/8192.0)*((*temp)/131072.0-27504/8192.0)*((double)(-1000));
		printf("\tvar2: %f\n", var2);
	double t = (var1+var2)/5120.0;
		printf("\tActual temp: %f°C\n", t);
	return ret;
}

int bmp280_read_press(uint32_t *press) {
	*press = 0;
	if (initialised==0) {
		return -1;
	}
	const uint8_t reg_address = 0xF7;
	i2c_write_blocking(BMP280_I2C_STRUCT, BMP280_ADDRESS, &reg_address, 1, 0);
	int ret = i2c_read_blocking(BMP280_I2C_STRUCT, BMP280_ADDRESS, ((uint8_t *)press+1), 3, 0);
	// Fix memory alignment
	(*press) >>= 4;
	(*press) &= 0x000FFFFF;
	return ret;
}

int bmp280_read_all(uint32_t *temp, uint32_t *press) {
	if (initialised==0) {
		return -1;
	}
	const uint8_t reg_address = 0xF7;
	uint64_t tempo = 0;
	i2c_write_blocking(BMP280_I2C_STRUCT, BMP280_ADDRESS, &reg_address, 1, 0);
	int ret = i2c_read_blocking(BMP280_I2C_STRUCT, BMP280_ADDRESS, (uint8_t *)&tempo, 6, 0);
	// Keep only the value, and delete null bits at the same time.
	*(press) = (tempo & 0xFFFFF00000000000) >> 44;
	// same thing here
	*(temp) = (tempo & 0x0000000000FFFFF0) >> 4;
	return ret;
}