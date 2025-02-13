#ifndef BME680_H
#define BME680_H

#include <stdint.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define BME680_ADDRESS 0x76
#define BME680_I2C_STRUCT i2c0

struct Bme680Results {
	double temperature; // °C
	double pressure; // Pa
};
typedef struct Bme680Results Bme680Results;

int bme680_init(uint8_t, uint8_t);
int bme680_reset();
int bme680_get_chipid(uint8_t *);
int bme680_start_measurement_non_blocking();
int bme680_read_results(Bme680Results *);
int bme680_write_bytes(uint8_t, uint8_t *, uint8_t);
int bme680_read_bytes(uint8_t, uint8_t *, uint8_t);

#endif