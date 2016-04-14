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
import pyupm_bh1745nuc as sensorObj

# Instantiate an BH1745NUC on I2C bus 0
sensor = sensorObj.BH1745NUC()

## Exit handlers ##
# This function stops python from printing a stacktrace when you hit control-C
def SIGINTHandler(signum, frame):
	raise SystemExit

# This function lets you run code on exit
def exitHandler():
	print "Exiting"
	sys.exit(0)

# Register exit handlers
atexit.register(exitHandler)
signal.signal(signal.SIGINT, SIGINTHandler)

sensor.init()

r = sensorObj.new_uint16p()
g = sensorObj.new_uint16p()
b = sensorObj.new_uint16p()
c = sensorObj.new_uint16p()
status = sensorObj.new_boolp()

# Enable interrupt on each end of messurament. The second argument is dummy.
sensor.setInterruptMode(True, sensor.INT_CLEAR, sensor.EOM)
# Pin24 is used for interrupt with the default no-op ISR.
sensor.installISR(24, sensorObj.DefaultISR, None)
sensor.setMode(sensor.RATE_160MS)
sensor.setSleep(False)

while (1):
        sensor.getInterruptStatus(status)
        if (not sensorObj.boolp_value(status)):
        	continue
        sensor.getColorData(r, g, b, c)
        print "Color data: R: ", sensorObj.uint16p_value(r), 
        print " G: ", sensorObj.uint16p_value(g),
        print " B: ", sensorObj.uint16p_value(b),
        print " C: ", sensorObj.uint16p_value(c)
        print

	sensor.clearInterrupt()
