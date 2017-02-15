#!/usr/bin/env bash

export PATH=$HOME/.gp/256dpi/bin:$PATH

gcode strip PositionSensor.top.drill.gcode PositionSensor.top.drill.gcode
gcode strip PositionSensor.top.etch.gcode PositionSensor.top.etch.gcode

gcode offset PositionSensor.top.drill.gcode PositionSensor.top.drill.off.gcode 0 15 0
gcode offset PositionSensor.top.etch.gcode PositionSensor.top.etch.off.gcode 0 15 0

awk 'NR==5 {$0="G0 X20 Y20"} 1' PositionSensor.top.drill.off.gcode > PositionSensor.top.drill.fix.gcode
awk 'NR==5 {$0="G0 X20 Y20"} 1' PositionSensor.top.etch.off.gcode > PositionSensor.top.etch.fix.gcode
