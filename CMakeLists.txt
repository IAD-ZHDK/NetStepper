# minimal required cmake version
cmake_minimum_required(VERSION 3.7)

# you can set your own project name
project(NetStepper)

# this should not be changed
set(CMAKE_C_STANDARD 99)

# add your source files
set(SOURCE_FILES
        src/led.c
        src/led.h
        src/main.c
        src/net_stepper.c
        src/net_stepper.h
        src/sensors.c
        src/sensors.h
        src/stepper_driver.c
        src/stepper_driver.h)

# create a fake library target
add_library(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})

# include naos include paths
add_subdirectory(naos)
