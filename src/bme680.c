#include "bme680.h"

Bme680CalibrationSettings bme680calib;

/*!
 * \brief Initializes the bme680. Must be called before any other function.
 * It will use the address and i2c channel given in bme680.h
 * \param ctrl_meas Register containing the oversampling settings for temperature and pressure.
 * \param ctrl_hum Register contatining the oversamplig settings for humidity.
 * \returns result of the last writing function
 */
int bme680_init(uint8_t ctrl_meas, uint8_t ctrl_hum) {
	ctrl_meas &= 0b11111100; // Make sure a measurement isn't started.
	ctrl_hum &= 0b00000111; // Make sure we don't change reserved values.

	// ctrl_hum has reserved values, so we get them from the register and then write.
	uint8_t ctrl_hum_value;
	printf("first read: %d\n", bme680_read_bytes(0x72, &ctrl_hum_value, 1));
	printf("ctr_hum value: %hhu\n", ctrl_hum_value);

	if (bme680_write_bytes(0x74, &ctrl_meas, 1)==-1) {
		return -3;
	}
	if (bme680_write_bytes(0x72, &ctrl_hum, 1)==-1) {
		return -1;
	}
	sleep_ms(1000);
	bme680_init_calibration_settings();
	return 0;
}

int bme680_init_calibration_settings() {
	bme680_read_bytes(0xEA, (uint8_t *)(&bme680calib.par_t1)+1, 1);
	// bme680calib.par_t1 <<= 8;
	bme680_read_bytes(0xE9, (uint8_t *)&bme680calib.par_t1, 1);

	bme680_read_bytes(0x81, (uint8_t *)(&bme680calib.par_t2), 2);

	bme680_read_bytes(0x8C, &bme680calib.par_t3, 1);

	bme680_read_bytes(0x24, (uint8_t *)(&bme680calib.temp_adc), 1);
	bme680_read_bytes(0x23, (uint8_t *)(&bme680calib.temp_adc)+1, 1);
	bme680_read_bytes(0x22, (uint8_t *)(&bme680calib.temp_adc)+2, 1);
	bme680calib.temp_adc >>= 4;
	bme680calib.temp_adc &= 0x000FFFFF;

	return 0;
}

int bme680_reset() {
	uint8_t reset_byte = 0xB6;
	return bme680_write_bytes(0xE0, &reset_byte, 1);
}

int bme680_get_chipid(uint8_t *chipid_byte) {
	return bme680_read_bytes(0xD0, chipid_byte, 1);
}

/*!
 * \brief Starts a measurement. Function will not block and bme680_read_results will have to be used to get measurements.
 */
int bme680_start_measurement_non_blocking() {
	// The mode bits are in the ctrl_meas (0x74) register.
	uint8_t ctrl_reg = 0;
	bme680_read_bytes(0x74, &ctrl_reg, 1);
	ctrl_reg |= 0x01; // Forced mode
	int ret = bme680_write_bytes(0x74, &ctrl_reg, 1);
	return ret;
}

int bme680_get_status(uint8_t *status) {
	return bme680_read_bytes(0x1D, status, 1);
}

/*!
 * \brief reads and converts the results, because no one cares about raw sensor values.
 */
int bme680_read_results(Bme680Results *results) {
	uint8_t status;
	do {
		bme680_get_status(&status);
		sleep_us(100);
	} while ((status & 0x80) >> 7);
	uint8_t raw_results[8] = {0};
	bme680_read_bytes(0x1F, raw_results, 8);

	uint32_t raw_pressure = raw_results[0] << 12;
	raw_pressure |= raw_results[1] << 4;
	raw_pressure |= (raw_results[2] >> 4) & 0x0F;

	uint32_t raw_temperature = raw_results[3] << 12;
	raw_temperature |= raw_results[4] << 4;
	raw_temperature |= (raw_results[5] >> 4) & 0x0F;

	uint16_t raw_humidity = raw_results[6] << 8;
	raw_humidity |= raw_results[7];

	printf("Raw pressure: 0x%X\n", raw_pressure);
	printf("Raw temperature: 0x%X\n", raw_temperature);
	printf("Raw humidity: 0x%X\n", raw_humidity);

	// temperature
	double var1 = (raw_temperature/16384.0 - bme680calib.par_t1/1024.0)*bme680calib.par_t2;
	double var2 = (raw_temperature/131072.0 - bme680calib.par_t1/8192.0)*(raw_temperature/131072.0 - bme680calib.par_t1/8192.0)*(bme680calib.par_t3*16.0);
	results->temperature = (var1+var2)/5120.0;

	return 0;
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
int bme680_read_bytes(uint8_t address, uint8_t *data, uint8_t data_len) {
	i2c_write_blocking(BME680_I2C_STRUCT, BME680_ADDRESS, &address, 1, 0);
	return i2c_read_blocking(BME680_I2C_STRUCT, BME680_ADDRESS, data, data_len, 0);
}