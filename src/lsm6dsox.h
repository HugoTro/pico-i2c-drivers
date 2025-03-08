#ifndef LSM6DSOX_H
#define LSM6DSOX_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define LSM6DSOX_ADDRESS 0x6A
#define LSM6DSOX_I2C_STRUCT i2c1

int lsm6dsox_init(uint8_t, uint8_t);
int lsm6dsox_get_whoami(uint8_t *);
int lsm6dsox_get_temperature(int16_t *);
int lsm6dsox_change_register(uint8_t, uint8_t, uint8_t);
int lsm6dsox_write_bytes(uint8_t, uint8_t *, uint8_t);
int lsm6dsox_read_bytes(uint8_t, uint8_t *, uint8_t);

#endif