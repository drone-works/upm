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
import pyupm_bm1383glv as sensorObj

# Instantiate an BM1383GLV on I2C bus 0
sensor = sensorObj.BM1383GLV()

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
high = sensorObj.new_boolp()
low = sensorObj.new_boolp()

# Pin24 is used for interrupt with the default no-op ISR.
sensor.installISR(24, sensorObj.DefaultISR, None)
# With setting high watermark to 0, high watermark interrupt can
# report the end of measurement
sensor.setWatermark(0.0, 0.0)
sensor.setInterruptMode(True, False, True);
sensor.setSleep(False)

while (1):
        sensor.getInterruptStatus(high, low)
        if (not sensorObj.boolp_value(high)):
                continue
	sensor.getPressure(x)
        print '{0: 6f} hPa'.format(sensorObj.floatp_value(x))

	sensor.clearInterrupt()
