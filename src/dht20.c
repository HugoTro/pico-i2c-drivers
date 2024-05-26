#include "dht20.h"

/*!
 *	\brief Init DHT20 sensor. Function takes at least 140ms
*/
void dht20_init() {
	const uint8_t init_b[] = {
		0xbe,
		0x08,
		0x00
	};
	sleep_ms(100);
	dht20_reset();
	// Wait 20ms after soft reset + 10ms just to be safe
	sleep_ms(30);
	// Write initialization bytes to the DHT20
	i2c_write_blocking(DHT20_I2C_STRUCT, DHT20_ADDRESS, init_b, sizeof(init_b), 0);
	// wait 10ms, as per datasheet
	sleep_ms(10);
	// init registers, assuming that status byte is null
	//dht20_init_regs();
	if (dht20_check_calibrated()==0) {
		uint8_t calib_b[] = {0xe1, 0x08, 0x00};
		i2c_write_blocking(DHT20_I2C_STRUCT, DHT20_ADDRESS, calib_b, sizeof(calib_b), 0);
	}
}

int dht20_init_reg(uint8_t reg) {
	uint8_t commands[] = {
		0x70,
		reg,
		0,
		0
	};
	return i2c_write_blocking(DHT20_I2C_STRUCT, DHT20_ADDRESS, commands, sizeof(commands), 0);
}

void dht20_init_regs() {
	dht20_init_reg(0x1B);
	dht20_init_reg(0x1C);
	dht20_init_reg(0x1E);
}

int dht20_reset() {
	// "Soft reset" (whatever that means) from the AHT20 datasheet (page 8 table 9)
	static const uint8_t s_b = 0b10111010;
	return i2c_write_blocking(DHT20_I2C_STRUCT, DHT20_ADDRESS, &s_b, 1, 0);
}

/*!
 *	\brief Send comand to start measurement
*/
int dht20_start_measurement() {
	static const uint8_t commands[3] = {
		0xAC,
		0x33,
		0x00
	};
	return i2c_write_blocking(DHT20_I2C_STRUCT, DHT20_ADDRESS, commands, 3, 0);
}

/*!
 *	\brief Read temperature and humidity.
 *	This function will block until it receives something from the sensor.
 *	\param temp Pointer to a 32bit unsigned integer, which will contain the temperature
 *	\param hum Pointer to a 32bit unsigned integer, which will contain the humidity
 *	\returns Result of the read operation (determines if the reading is correct)
*/
int dht20_read_all(uint32_t *temp, uint32_t *hum) {
	// Receiving bytes array
	// [0]: temp msb, [1]: temp lsb, [2]: temp xlsb/hum msb, [3]: hum lsb, [4]: hum xlsb
	uint8_t bytes[6] = {0};

	dht20_start_measurement();
	// Wait 75ms for measurement to complete, as per datasheet (confusing, sometimes it says 80ms and on page 9 it says 75ms)
	sleep_ms(75);
	while (dht20_check_occupied()) {
		sleep_ms(10);
	}
	int res = i2c_read_blocking(DHT20_I2C_STRUCT, DHT20_ADDRESS, bytes, 6, 0);
	*temp = (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
	*temp >>= 4;
	*hum = ((bytes[3] & 0x0F) << 16) | (bytes[4] << 8) | bytes[5];
	return res;
}

/*!
 *	\brief Read status byte
*/
int dht20_read_status(uint8_t *status) {
	static const uint8_t b = 0x71;
	i2c_write_blocking(DHT20_I2C_STRUCT, DHT20_ADDRESS, &b, 1, 0);
	return i2c_read_blocking(DHT20_I2C_STRUCT, DHT20_ADDRESS, status, 1, 0);
}

/*!
 *	\brief Checks the occupied bit
 *	\returns occupied = 1; free = 0
*/
uint8_t dht20_check_occupied() {
	uint8_t status_byte;
	dht20_read_status(&status_byte);
	return (status_byte&0x80) >> 7;
}

/*!
 *	\brief Checks the calibrated bit
 *	\returns if calibated = 1; else = 0
*/
uint8_t dht20_check_calibrated() {
	uint8_t status_byte;
	dht20_read_status(&status_byte);
	return (status_byte&0x08) >> 3;
}