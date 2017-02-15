#!/usr/bin/env bash

export PATH=$HOME/.gp/256dpi/bin:$PATH

gcode strip Controller1.top.drill.gcode Controller1.top.drill.gcode
gcode strip Controller1.top.etch.gcode Controller1.top.etch.gcode

gcode offset Controller1.top.drill.gcode Controller1.top.drill.off.gcode 17 17 0
gcode offset Controller1.top.etch.gcode Controller1.top.etch.off.gcode 17 17 0
