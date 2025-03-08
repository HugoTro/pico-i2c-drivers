# pico-i2c-drivers
This library is a collection of various i2c drivers made for various sensors and actuators (temperature sensors, screens, etc)

## Module state
- bmp280: <span style="color:green">Working</span>
- dht20: <span style="color:red">Broken</span>
- sh1106: <span style="color:green">Working</span>, rectangle round corners not implemented
- bme680: <span style="color:yellow">Partially working</span>, **only temperature works for now.**
- lsm6dsox: <span style="color:red">Planned</span>

## Current test file state:
Tests the bme680 and bmp280 in parallel

# Drivers
In the register parameters shown below, x indicates another unrelated value. It may be another parameter. For each setting, I've also indicated the table from which it's extracted (see respective datasheets).
## BME680
### Oversampling settings:
 - Humidity (**ctrl_hum**):<br/>
	No measurement: 0bxxxxx000<br/>
	x1 ovs: 0bxxxxx001<br/>
	x2 ovs: 0bxxxxx010<br/>
	x4 ovs: 0bxxxxx011<br/>
	x8 ovs: 0bxxxxx100<br/>
	x16 ovs: all other values<br/>
 - Temperature (**ctrl_meas**):<br/>
	No measurement: 0b000xxxxx<br/>
	ovs x1: 0b001xxxxx<br/>
	ovs x2: 0b010xxxxx<br/>
	ovs x4: 0b011xxxxx<br/>
	ovs x8: 0b100xxxxx<br/>
	ovs x16: all other values<br/>
 - Pressure (**ctrl_meas**):<br/>
	No measurement: 0bxxx000xx<br/>
	ovs x1: 0bxxx001xx<br/>
	ovs x2: 0bxxx010xx<br/>
	ovs x4: 0bxxx011xx<br/>
	ovs x8: 0bxxx100xx<br/>
	ovs x16: all other values<br/>

## LSM6DSOX
### Scale settings:
 - Accelerometer scale selection (**ctrl1_xl**), Table 52:
	±2g: 0bxxxx00xx
	±4g: 0bxxxx10xx
	±8g: 0bxxxx11xx
	±16g: 0bxxxx01xx

### Output Data Rate settings:
 - Accelerometer ODR (**ctrl1_xl**), Table 51:
	Power down: 0b0000xxxx
	*low-power mode*
	01.6Hz: 0b1011xxxx
	12.5Hz: 0b0001xxxx
	26.0Hz: 0b0010xxxx
	52.0Hz: 0b0011xxxx
	*normal mode*
	104Hz: 0b0100xxxx
	208Hz: 0b0101xxxx
	*high performance mode*
	416Hz: 0b0110xxxx
	833Hz: 0b0111xxxx
	1.66kHz: 0b1000xxxx
	3.33kHz: 0b1001xxxx
	6.66kHz: 0b1010xxxx
