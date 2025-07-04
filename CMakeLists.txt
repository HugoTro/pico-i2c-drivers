cmake_minimum_required(VERSION 3.13)
set(PICO_BOARD pico_w)
include(pico_sdk_import.cmake)
project(i2c-drivers)
pico_sdk_init()

# set(PICOTOOL_FORCE_FETCH_FROM_GIT 1)

if (TARGET tinyusb_device)
    add_executable(test src/test.c src/bme680.c src/bmp280.c src/fonts.c src/sh1106.c)
    add_executable(test_sh1106 src/test_sh1106.c src/fonts.c src/sh1106.c)
    add_executable(test_bmp280 src/test_bmp280.c src/bmp280.c)
    add_executable(test_bme680 src/test_bme680.c src/bme680.c)
	add_executable(test_lsm6dsox src/test_lsm6dsox.c src/lsm6dsox.c)

    target_link_libraries(test pico_stdlib hardware_i2c)
    target_link_libraries(test_sh1106 pico_stdlib hardware_i2c)
    target_link_libraries(test_bmp280 pico_stdlib hardware_i2c)
	target_link_libraries(test_bme680 pico_stdlib hardware_i2c)
	target_link_libraries(test_lsm6dsox pico_stdlib hardware_i2c)


    pico_enable_stdio_usb(test 1)
    pico_enable_stdio_usb(test_sh1106 1)
    pico_enable_stdio_usb(test_bmp280 1)
	pico_enable_stdio_usb(test_bme680 1)
	pico_enable_stdio_usb(test_lsm6dsox 1)

    pico_enable_stdio_uart(test 0)
    pico_enable_stdio_uart(test_sh1106 0)
    pico_enable_stdio_uart(test_bmp280 0)
	pico_enable_stdio_uart(test_bme680 0)
	pico_enable_stdio_uart(test_lsm6dsox 0)

    pico_add_extra_outputs(test)
    pico_add_extra_outputs(test_sh1106)
    pico_add_extra_outputs(test_bmp280)
	pico_add_extra_outputs(test_bme680)
	pico_add_extra_outputs(test_lsm6dsox)

elseif(PICO_ON_DEVICE)
    message(WARNING "not building because TinyUSB submodule is not initialized in the SDK")
endif()