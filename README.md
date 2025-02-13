# pico-i2c-drivers
This library is a collection of various i2c drivers made for various sensors and actuators (temperature sensors, screens, etc)

## Module state
- bmp280: <span style="color:green">Working</span>
- dht20: <span style="color:red">Broken</span>
- sh1106: <span style="color:green">Working</span>, rectangle round corners not implemented
- bme280: <span style="color:red">Not ready</span>, **gas not supported**, only temperature, humidity, and pressure.
- lsm6dsox: <span style="color:red">Planned</span>

# Drivers
## BME680
### Oversampling settings:
x indicates another unrelated value. It may be another parameter (in ctrl_meas for example).
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