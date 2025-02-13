#ifndef BME680_H
#define BME680_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define BME680_ADDRESS 0x77
#define BME680_I2C_STRUCT i2c0

struct Bme680CalibrationSettings {
	// temperature calibration variables
	uint32_t temp_adc;
	uint16_t par_t1;
	uint16_t par_t2;
	uint8_t par_t3;
	// pressure calibration variables
	uint16_t par_p1;
	uint16_t par_p2;
	uint8_t par_p3;
	uint16_t par_p4;
	uint16_t par_p5;
	uint8_t par_p6;
	uint8_t par_p7;
	uint16_t par_p8;
	uint16_t par_p9;
	uint8_t par_p10;
	uint32_t press_adc;
	// humidity calibration variables
	uint16_t par_h1;
	uint16_t par_h2;
	uint8_t par_h3;
	uint8_t par_h4;
	uint8_t par_h5;
	uint8_t par_h6;
	uint8_t par_h7;
	uint16_t hum_adc;
	
};
typedef struct Bme680CalibrationSettings Bme680CalibrationSettings;

struct Bme680Results {
	double temperature; // °C
	double pressure; // Pa
	double humidity; // %
};
typedef struct Bme680Results Bme680Results;

int bme680_init(uint8_t, uint8_t);
int bme680_init_calibration_settings();
int bme680_reset();
int bme680_get_chipid(uint8_t *);
int bme680_start_measurement_non_blocking();
int bme680_read_results(Bme680Results *);
int bme680_write_bytes(uint8_t, uint8_t *, uint8_t);
int bme680_read_bytes(uint8_t, uint8_t *, uint8_t);

#endif