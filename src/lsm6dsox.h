#ifndef LSM6DSOX_H
#define LSM6DSOX_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define LSM6DSOX_ACC_ODR_1_6HZ (0b1011 << 4)
#define LSM6DSOX_ACC_ODR_12_5HZ (0b0001 << 4)
#define LSM6DSOX_ACC_ODR_26HZ (0b0010 << 4)
#define LSM6DSOX_ACC_ODR_52HZ (0b0011 << 4)
#define LSM6DSOX_ACC_ODR_104HZ (0b0100 << 4)
#define LSM6DSOX_ACC_ODR_208HZ (0b0101 << 4)
#define LSM6DSOX_ACC_ODR_416HZ (0b0110 << 4)
#define LSM6DSOX_ACC_ODR_833HZ (0b0111 << 4)
#define LSM6DSOX_ACC_ODR_1660HZ (0b1000 << 4)
#define LSM6DSOX_ACC_ODR_3330HZ (0b1001 << 4)
#define LSM6DSOX_ACC_ODR_6660HZ (0b1010 << 4)

#define LSM6DSOX_GYRO_ODR_12_5HZ (0b0001 << 4)
#define LSM6DSOX_GYRO_ODR_26HZ (0b0010 << 4)
#define LSM6DSOX_GYRO_ODR_52HZ (0b0011 << 4)
#define LSM6DSOX_GYRO_ODR_104HZ (0b0100 << 4)
#define LSM6DSOX_GYRO_ODR_208HZ (0b0101 << 4)
#define LSM6DSOX_GYRO_ODR_416HZ (0b0110 << 4)
#define LSM6DSOX_GYRO_ODR_833HZ (0b0111 << 4)
#define LSM6DSOX_GYRO_ODR_1660HZ (0b1000 << 4)
#define LSM6DSOX_GYRO_ODR_3330HZ (0b1001 << 4)
#define LSM6DSOX_GYRO_ODR_6660HZ (0b1010 << 4)

// disable performance mode for all ODRs, instead referencing to the modes indicated in table 51
#define LSM6DSOX_ACC_ALLOW_ALL_MODES 1 << 4
// same but table 55
#define LSM6DSOX_GYRO_ALLOW_ALL_MODES 1 << 7

// ±x G
#define LSM6DSOX_ACC_SCALE_2G 0
#define LSM6DSOX_ACC_SCALE_4G (0b10 << 2)
#define LSM6DSOX_ACC_SCALE_8G (0b11 << 2)
#define LSM6DSOX_ACC_SCALE_16G (0b11 << 2)

// ±x dps
#define LSM6DSOX_GYRO_SCALE_250DPS 0
#define LSM6DSOX_GYRO_SCALE_500DPS (0b01 << 2)
#define LSM6DSOX_GYRO_SCALE_1000DPS (0b10 << 2)
#define LSM6DSOX_GYRO_SCALE_2000DPS (0b11 << 2)

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

void lsm6dsox_setup(i2c_inst_t *, uint8_t);
int lsm6dsox_init(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
i2c_inst_t *lsm6dsox_get_i2c_struct();
int lsm6dsox_reset();
int lsm6dsox_get_whoami(uint8_t *);
int lsm6dsox_get_temperature(float *);
int lsm6dsox_get_acceleration(lsm6dsoxAcceleration *);
int lsm6dsox_get_angular_velocity(lsm6dsoxAngVel *);
int lsm6dsox_change_register(uint8_t, uint8_t, uint8_t);
int lsm6dsox_write_bytes(uint8_t, uint8_t *, uint8_t);
int lsm6dsox_read_bytes(uint8_t, uint8_t *, uint8_t);

#endif