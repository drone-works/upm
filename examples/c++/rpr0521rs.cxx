/*
 * Copyright (c) 2016 Droneworks.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <unistd.h>
#include <iostream>
#include <signal.h>
#include "rpr0521rs.h"

using namespace std;
using namespace upm;

int shouldRun = true;
upm::RPR0521RS* lightsensor;

void
sig_handler(int signo)
{
  if (signo == SIGINT)
    shouldRun = false;
}

void
data_callback(void* data)
{
  bool ps, als;
  lightsensor->getInterruptStatus(ps, als);

  if (ps) {
    uint16_t x;
    lightsensor->getProximity(&x);
    printf("PS: 0x%04x\n", x);
    // usleep(100);
  }
  if (als) {
    uint16_t x, y;
    lightsensor->getAmbientLight(&x);
    lightsensor->getAmbientLightIr(&y);
    printf("ALS: 0x%04x             0x%04x\n", x, y);
    // usleep(100);
  }

  lightsensor->clearInterrupt();
}

int
main()
{
  signal(SIGINT, sig_handler);
  //! [Interesting]
  // Instantiate a RPR0521RS Optical Sensor
  lightsensor = new upm::RPR0521RS();
  lightsensor->init();
  lightsensor->installISR(24, data_callback, NULL);

  lightsensor->setAmbientLightWatermark(200, 0x0);
  lightsensor->setProximityWatermark(100, 0);
  lightsensor->setProximityOffset(0);
  lightsensor->setInterruptMode(true, true, RPR0521RS::INT_HIGH,
				RPR0521RS::UPDATE_EOM);
  lightsensor->setMode(true, true, false, false,
		       RPR0521RS::LED_CURRENT_25MA,
		       RPR0521RS::RATE_10HZ_10HZ);
  lightsensor->setSleep(false);

  while (shouldRun) {
    sleep(1);
  }

  //! [Interesting]
  cout << "Exiting" << endl;

  lightsensor->setSleep(true);

  delete lightsensor;

  return 0;
}
