#ifndef BME680_H
#define BME680_H

#include <stdint.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define BME680_ADDRESS 0x76
#define BME680_I2C_STRUCT i2c0

int bme680_write_bytes(uint8_t, uint8_t *, uint8_t);

#endif