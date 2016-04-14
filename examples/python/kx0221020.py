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
import pyupm_kx0221020 as sensorObj

# Instantiate an KX0221020 on I2C bus 0
sensor = sensorObj.KX0221020()

## Exit handlers ##
# This function stops python from printing a stacktrace when you hit control-C
def SIGINTHandler(signum, frame):
	raise SystemExit

# This function lets you run code on exit
def exitHandler():
	print "Exiting"
        sensor.setSleep(True);
	sys.exit(0)

# Register exit handlers
atexit.register(exitHandler)
signal.signal(signal.SIGINT, SIGINTHandler)

sensor.init()

x = sensorObj.new_floatp()
y = sensorObj.new_floatp()
z = sensorObj.new_floatp()
full = sensorObj.new_boolp()
rdy = sensorObj.new_boolp()
watermark = sensorObj.new_boolp()

# Pin24 is used for interrupt with the default no-op ISR.
sensor.installISR(24, sensorObj.DefaultISR, None)
sensor.setSleep(False)

while (1):
        sensor.getInterruptStatus(full, rdy, watermark)
        if (not sensorObj.boolp_value(rdy)):
                continue
        n = sensor.getCount()
        i = 0
        while i < n:
	        sensor.getAccelerometer(x, y, z)
                print 'X: {0: 6f} Y: {1: 6f} Z: {2: 6f}'.format(
                        sensorObj.floatp_value(x),
                        sensorObj.floatp_value(y),
                        sensorObj.floatp_value(z))
                i = i + 1

	sensor.clearInterrupt()
