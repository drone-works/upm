#!/usr/bin/python
# Copyright (c) 2016 Droneworks.
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

import time, sys, signal, atexit
import pyupm_rpr0521rs as sensorObj

# Instantiate an RPR0521RS on I2C bus 0
sensor = sensorObj.RPR0521RS()

## Exit handlers ##
# This function stops python from printing a stacktrace when you hit control-C
def SIGINTHandler(signum, frame):
	raise SystemExit

# This function lets you run code on exit
def exitHandler():
	print "Exiting"
        sensor.setSleep(True)
	sys.exit(0)

# Register exit handlers
atexit.register(exitHandler)
signal.signal(signal.SIGINT, SIGINTHandler)

sensor.init()

x = sensorObj.new_uint16p()
ab = sensorObj.new_uint16p()
ir = sensorObj.new_uint16p()
ps = sensorObj.new_boolp()
als = sensorObj.new_boolp()

# Enable PS and ALS interrupts. Update on each end of messurament.
sensor.setInterruptMode(True, True, sensor.INT_HIGH, sensor.UPDATE_EOM)
sensor.setAmbientLightWatermark(200, 0);
sensor.setProximityWatermark(100, 0);
# Pin24 is used for interrupt with the default no-op ISR.
sensor.installISR(24, sensorObj.DefaultISR, None)
sensor.setMode(True, True, False, False, sensor.LED_CURRENT_25MA,
               sensor.RATE_10HZ_10HZ)
sensor.setSleep(False)

while (1):
        sensor.getInterruptStatus(ps, als)
        if (sensorObj.boolp_value(ps)):
            sensor.getProximity(x)
            print "PS: ", sensorObj.uint16p_value(x)
        elif (sensorObj.boolp_value(als)):
            sensor.getAmbientLight(ab)
            sensor.getAmbientLightIr(ir)
            print "ALS: ", sensorObj.uint16p_value(ab),
            print "     ", sensorObj.uint16p_value(ir)
        else:
        	continue

	sensor.clearInterrupt()
