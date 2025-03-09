#ifndef LSM6DSOX_H
#define LSM6DSOX_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define LSM6DSOX_ADDRESS 0x6A
#define LSM6DSOX_I2C_STRUCT i2c1

#define LSM6DSOX_ACC_SENSITIVITY 8 // Should be the ±x g you put in ctrl1_xl
#define LSM6DSOX_ANG_SENSITIVITY 1000 // same but for °/s

struct lsm6dsoxAcceleration {
	float x;
	float y;
	float z;
};
typedef struct lsm6dsoxAcceleration lsm6dsoxAcceleration;

struct lsm6dsoxAngVel {
	float x;
	float y;
	float z;
};
typedef struct lsm6dsoxAngVel lsm6dsoxAngVel;

int lsm6dsox_init(uint8_t, uint8_t);
int lsm6dsox_reset();
int lsm6dsox_get_whoami(uint8_t *);
int lsm6dsox_get_temperature(float *);
int lsm6dsox_get_acceleration(lsm6dsoxAcceleration *);
int lsm6dsox_get_angular_velocity(lsm6dsoxAngVel *);
int lsm6dsox_change_register(uint8_t, uint8_t, uint8_t);
int lsm6dsox_write_bytes(uint8_t, uint8_t *, uint8_t);
int lsm6dsox_read_bytes(uint8_t, uint8_t *, uint8_t);

#endif