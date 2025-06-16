# pico-i2c-drivers
This library is a collection of various i2c drivers made for various sensors and actuators (temperature sensors, screens, etc)

## Module state
- bmp280: <span style="color:green">Working</span>
- dht20: <span style="color:red">Broken</span>
- sh1106: <span style="color:green">Working</span>, rectangle round corners not implemented
- bme680: <span style="color:yellow">Partially working</span>, **only temperature works for now.**
- lsm6dsox: <span style="color:red">Working</span>, with some manual parameters to enter

## Current test file state:
Tests the bme680 and bmp280 in parallel
