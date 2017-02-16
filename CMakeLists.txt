cmake_minimum_required(VERSION 3.5)
project(NetStepper)

include_directories(
        /Applications/Arduino.app/Contents/Java/hardware/arduino/avr/cores/arduino/
        /Applications/Arduino.app/Contents/Java/hardware/arduino/avr/libraries/SPI/src/
        /Users/256dpi/Development/Arduino/libraries/WiFi101/src
        /Users/256dpi/Development/GitHub/256dpi/arduino-mqtt/src)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        NetStepper.ino
        src/Stepper.cpp
        src/Stepper.h src/Manager.cpp src/Manager.h)

add_executable(NetStepper ${SOURCE_FILES})
