#!/usr/bin/env bash

export PATH=$HOME/.gp/256dpi/bin:$PATH

gcode strip ControllerESP32.top.drill.gcode ControllerESP32.top.drill.gcode
gcode strip ControllerESP32.top.etch.gcode ControllerESP32.top.etch.gcode

gcode offset ControllerESP32.top.drill.gcode ControllerESP32.top.drill.off.gcode 17 17 0
gcode offset ControllerESP32.top.etch.gcode ControllerESP32.top.etch.off.gcode 17 17 0
