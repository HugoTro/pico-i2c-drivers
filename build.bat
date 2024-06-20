@echo off
if not exist build/ (
    mkdir build/
)
cd build/
cmake -G "MinGW Makefiles" -DPICO_BOARD=pico_w ..
mingw32-make