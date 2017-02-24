#!/usr/bin/env bash

export PATH=$HOME/.gp/256dpi/bin:$PATH

gcode strip PositionSensor.top.drill.gcode PositionSensor.top.drill.gcode
gcode strip PositionSensor.top.etch.gcode PositionSensor.top.etch.gcode

gcode offset PositionSensor.top.drill.gcode PositionSensor.top.drill.off.gcode 17 17 0
gcode offset PositionSensor.top.etch.gcode PositionSensor.top.etch.off.gcode 17 17 0
