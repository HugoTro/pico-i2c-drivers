#include "bme680.h"


int bme680_init() {
	return i2c_write_blocking();
}

/*!
 * \brief Writes bytes to the bme680
 * \param address Address of the lowest register to write to
 * \param *data Pointer to the data to be written
 * \param data_len How many bytes to write from *data, with auto-incrementing memory addresses
 * \returns Result of the write operation
 */
int bme680_write_bytes(uint8_t address, uint8_t *data, uint8_t data_len) {
	uint8_t *to_write = malloc(2*data_len);
	uint8_t idx = 0;
	for (uint8_t i = 0; i<data_len; i++) {
		*(to_write+idx) = address;
		address++;
		idx++;
		*(to_write+idx) = *(data+i);
		idx++;
	}
	int ret = i2c_write_blocking(BME680_I2C_STRUCT, BME680_ADDRESS, to_write, data_len*2, 0);
	free(to_write);
	return ret;
}

/*!
 * \brief Reads bytes from the bme680
 * \param address Address of the lowest register to read from
 * \param *data Pointer to where the read data will be stored
 * \param data_len Length of the data to be read, with auto incremented registers addresses
 * \returns Result of the read operation
 */
int bmp280_read_bytes(uint8_t address, uint8_t *data, uint8_t data_len) {
	i2c_write_blocking(BMP280_I2C_STRUCT, BMP280_ADDRESS, &address, 1, 0);
	return i2c_read_blocking(BMP280_I2C_STRUCT, BMP280_ADDRESS, data, data_len, 0);
}