cmake_minimum_required(VERSION 3.7)
project(net-stepper)

set(CMAKE_C_STANDARD 99)

include_directories(
        .naos/tree/build/include
        .naos/xtensa-esp32-elf/xtensa-esp32-elf/include
        .naos/esp-idf/components/esp32/include
        .naos/esp-idf/components/freertos/include
        .naos/esp-idf/components/soc/esp32/include
        .naos/esp-idf/components/driver/include
        .naos/tree/components/esp-mqtt
        .naos/tree/components/naos-esp/include)

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

add_library(net-stepper ${SOURCE_FILES})
