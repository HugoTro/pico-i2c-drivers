#include "bmp280.h"
#include <string.h>
#include <stdio.h>

Bmp280CalibrationSettings bmp280_calib;
uint8_t initialised = 0;

/*!
 *	\brief Initialize communication with the bmp280 module.
 *	See datasheet for how to set the registers up.
 *	\param control_meas content to put into the "ctrl_meas" register, address 0xF4.
 *	\param config content to put into the "control" register, address 0xF5.
 *	\returns Result of the bmp280_init_calibration_settings() function
*/
int bmp280_init(uint8_t ctrl_meas, uint8_t config) {
	ctrl_meas &= 0xFC; // make sure not to start it in normal, but rather let it sleep
	config &= 0xFD; // bit[1] reserved
	bmp280_write_bytes(0xF4, &ctrl_meas, 1);
	bmp280_write_bytes(0xF5, &config, 1);

	return bmp280_init_calibration_settings();
}

int bmp280_get_chipid(uint8_t *chipid_byte) {
	return bmp280_read_bytes(0xD0, chipid_byte, 1);
}

int bmp280_get_status(uint8_t *status_byte) {
	int ret = bmp280_read_bytes(0xF3, status_byte, 1);
	*status_byte &= 0b00001001;
	return ret;
}

/*!
 * \brief This function doesn't do anything visible.
 * It is filling in the calibration settings struct present in this file.
 */
int bmp280_init_calibration_settings() {
	return bmp280_read_bytes(0x88, (uint8_t *)&bmp280_calib, 24);
}
/*!
 * \brief Writes bytes to the bmp280
 * \param address Address of the lowest register to write to
 * \param *data Pointer to the data to be written
 * \param data_len How many bytes to write from *data, with auto-incrementing memory addresses
 * \returns Result of the write operation
 */
int bmp280_write_bytes(uint8_t address, uint8_t *data, uint8_t data_len) {
	uint8_t *to_write = malloc(2*data_len);
	uint8_t idx = 0;
	for (uint8_t i = 0; i<data_len; i++) {
		*(to_write+idx) = address;
		address++;
		idx++;
		*(to_write+idx) = *(data+i);
		idx++;
	}
	int ret = i2c_write_blocking(BMP280_I2C_STRUCT, BMP280_ADDRESS, to_write, data_len*2, 0);
	free(to_write);
	return ret;
}

/*!
 * \brief Reads bytes from the bmp280
 * \param address Address of the lowest register to read from
 * \param *data Pointer to where the read data will be stored
 * \param data_len Length of the data to be read, with auto incremented registers addresses
 * \returns Result of the read operation
 */
int bmp280_read_bytes(uint8_t address, uint8_t *data, uint8_t data_len) {
	i2c_write_blocking(BMP280_I2C_STRUCT, BMP280_ADDRESS, &address, 1, 0);
	return i2c_read_blocking(BMP280_I2C_STRUCT, BMP280_ADDRESS, data, data_len, 0);
}

void bmp280_start_measurement_blocking() {
	uint8_t ctrl_reg = 0;
	bmp280_read_bytes(0xF4, &ctrl_reg, 1);
	ctrl_reg |= 0x01;
	bmp280_write_bytes(0xF4, &ctrl_reg, 1);
	uint8_t status = 0;
	sleep_us(1000);
	do {
		bmp280_get_status(&status);
	} while (status!=0);
	return;
}

int bmp280_read_results(uint8_t *results) {
	return bmp280_read_bytes(0xF7, results, 6);
}

/*!
 * \brief Converts meaningless values to temperature and pressure
 * \param *values pointer to an array of 6 bytes, straight from the bmp280's value registers (0xF7 through 0xFC)
 * \param *results pointer to a result struct, detailed in bmp280.h
 * \returns nothing.
 */
void bmp280_convert_results(uint8_t *values, Bmp280Results *results) {
	uint32_t temp = 0;
	*((uint8_t *)(&temp)+2) = values[3];
	*((uint8_t *)(&temp)+1) = values[4];
	*(uint8_t *)(&temp) = values[5];
	temp >>= 4;
	uint32_t press = 0;
	*((uint8_t *)(&press)+2) = values[0];
	*((uint8_t *)(&press)+1) = values[1];
	*(uint8_t *)(&press) = values[2];
	press >>= 4;
	// See datasheet for more info, but this is basically copy pasted from it
	double var1 = (temp/16384.0-bmp280_calib.dig_T1/1024.0)*bmp280_calib.dig_T2;
	double var2 = (temp/131072.0-bmp280_calib.dig_T1/8192.0)*(temp/131072.0-bmp280_calib.dig_T1/8192.0)*bmp280_calib.dig_T3;
	double t_fine = var1+var2;
	results->temperature = t_fine/5120.0;

	// Pressure
	var1 = t_fine/2.0 - 64000.0;
	var2 = var1*var1*bmp280_calib.dig_P6/32768.0;
	var2 += var1*bmp280_calib.dig_P5*2.0;
	var2 = var2/4.0 + bmp280_calib.dig_P4*65536.0;
	var1 = (bmp280_calib.dig_P3*var1*var1/524288.0 + bmp280_calib.dig_P2*var1)/524288.0;
	var1 = (1.0 + var1/32768.0)*bmp280_calib.dig_P1;
	double p = 1048576.0 - press;
	p = (p - var2/4096.0)*6250.0/var1;
	var1 = bmp280_calib.dig_P9*p*p/2147483648.0;
	var2 = p*bmp280_calib.dig_P8/32768.0;
	p += (var1 + var2 + bmp280_calib.dig_P7)/16.0;
	results->pressure = p;
	return;
}