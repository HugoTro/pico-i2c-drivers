#ifndef BMP280_H
#define BMP280_H

#include <stdint.h>
#include <stdlib.h>
#include "hardware/i2c.h"

// MAX BMP280 I2C SPEED: 3.4Mbit/s

// User parameters
#define BMP280_ADDRESS 0x77
#define BMP280_I2C_STRUCT i2c0

struct Bmp280CalibrationSettings {
						// LSB	/ MSB
	uint16_t dig_T1;	// 0x88	/ 0x89
	int16_t dig_T2;		// 0x8A	/ 0x8B
	int16_t dig_T3;		// 0x8C	/ 0x8D
	uint16_t dig_P1;	// 0x8E	/ 0x8F
	int16_t dig_P2;		// 0x90	/ 0x91
	int16_t dig_P3;		// 0x92	/ 0x93
	int16_t dig_P4;		// 0x94	/ 0x95
	int16_t dig_P5;		// 0x96 / 0x97
	int16_t dig_P6;		// 0x98 / 0x99
	int16_t dig_P7;		// 0x9A	/ 0x9B
	int16_t dig_P8;		// 0x9C	/ 0x9D
	int16_t dig_P9;		// 0x9E	/ 0x9F
};
typedef struct Bmp280CalibrationSettings Bmp280CalibrationSettings;

struct Bmp280Results {
	double temperature; // °C
	double pressure; // Pa
};
typedef struct Bmp280Results Bmp280Results;

int bmp280_init(uint8_t, uint8_t);
int bmp280_get_chipid(uint8_t *);
int bmp280_get_status(uint8_t *);
int bmp280_init_calibration_settings();
int bmp280_write_bytes(uint8_t, uint8_t *, uint8_t);
int bmp280_read_bytes(uint8_t, uint8_t *, uint8_t);
int bmp280_read_results(uint8_t *);
void bmp280_start_measurement_blocking();
void bmp280_convert_results(uint8_t *, Bmp280Results *);

#endif