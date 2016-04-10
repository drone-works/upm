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
#include "bm1383glv.h"

using namespace std;
using namespace upm;

int shouldRun = true;
upm::BM1383GLV* barometer;

void
sig_handler(int signo)
{
  if (signo == SIGINT)
    shouldRun = false;
}

void
data_callback(void* data)
{
  bool high, low;
  barometer->getInterruptStatus(high, low);

  if (high) {
    float x;
    barometer->getPressure(&x);
    printf("%.4f\n", x);
    // usleep(100);
  }

  barometer->clearInterrupt();
}

int
main()
{
  signal(SIGINT, sig_handler);
  //! [Interesting]
  // Instantiate a BM1383GLV Pressure Sensor
  barometer = new upm::BM1383GLV();
  barometer->init();
  barometer->installISR(24, data_callback, NULL);
  // With setting high watermark to 0, high watermark interrupt can
  // report the end of measurement.
  barometer->setWatermark(0.0, 0.0);
  barometer->setInterruptMode(true, false, true);
  barometer->setMode(BM1383GLV::MODE_RATE_10HZ);
  barometer->setAverageFilter(BM1383GLV::AVER_NO, false);
  barometer->setSleep(false);

  while (shouldRun) {
    sleep(1);
  }

  //! [Interesting]
  cout << "Exiting" << endl;

  barometer->setSleep(true);

  delete barometer;

  return 0;
}
