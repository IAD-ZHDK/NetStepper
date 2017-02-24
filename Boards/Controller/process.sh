#!/usr/bin/env bash

export PATH=$HOME/.gp/256dpi/bin:$PATH

gcode strip Controller.top.drill.gcode Controller.top.drill.gcode
gcode strip Controller.top.etch.gcode Controller.top.etch.gcode

gcode offset Controller.top.drill.gcode Controller.top.drill.off.gcode 17 17 0
gcode offset Controller.top.etch.gcode Controller.top.etch.off.gcode 17 17 0
