#ifndef BMP280_H
#define BMP280_H

#include <stdint.h>
#include "hardware/i2c.h"

// MAX BMP280 I2C SPEED: 3.4Mbit/s

// User parameters
#define BMP280_ADDRESS 0x77
#define I2C_STRUCT i2c1

/*!
 *	\brief Initialize communication with the bmp280 module.
 *	See datasheet for how to set the registers up.
 *	\param config content to put into the "control" register, address 0xF5.
 *	\param control_meas content to put into the "ctrl_meas" register, address 0xF4.
*/
int bmp280_init(uint8_t, uint8_t);
int bmp280_get_calibration_settings(uint16_t *);
int bmp280_read_temp(uint32_t *);
int bmp280_read_press(uint32_t *);
int bmp280_read_all(uint32_t *, uint32_t *);

#endif