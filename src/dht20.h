#ifndef DHT20_H
#define DHT20_H

#include <stdint.h>
#include "hardware/i2c.h"

// MAX DHT20 I2C SPEED: 400kbit/s

// User parameters
#define DHT20_ADDRESS 0x3C
#define DHT20_I2C_STRUCT i2c1

void dht20_init();
int dht20_init_reg(uint8_t);
void dht20_init_regs();
int dht20_reset();
int dht20_start_measurement();
int dht20_read_all(uint32_t *, uint32_t *);
int dht20_read_status(uint8_t *);
uint8_t dht20_check_occupied();
uint8_t dht20_check_calibrated();

#endif // DHT20_H