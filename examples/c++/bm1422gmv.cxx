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
#include "bm1422gmv.h"

using namespace std;
using namespace upm;

int shouldRun = true;
upm::BM1422GMV* magnetometer;

void
sig_handler(int signo)
{
  if (signo == SIGINT)
    shouldRun = false;
}

void
data_callback(void* data)
{
  float x, y, z;
  magnetometer->getMagnetometer(&x, &y, &z);
  printf("%.4f               %.4f               %.4f\n", x, y, z);
  // usleep(100);
}

int
main()
{
  signal(SIGINT, sig_handler);
  //! [Interesting]
  // Instantiate a BM1422GMV Magnetometer Sensor
  magnetometer = new upm::BM1422GMV();
  magnetometer->init();
  magnetometer->installISR(24, data_callback, NULL);
  magnetometer->setMode(BM1422GMV::MODE_CONTINUOUS, BM1422GMV::PREC_14BIT,
			BM1422GMV::RATE_10HZ);
  magnetometer->setAverageFilter(BM1422GMV::AVER_4);
  magnetometer->setSleep(false);

  while (shouldRun) {
    sleep(1);
  }

  //! [Interesting]
  cout << "Exiting" << endl;

  magnetometer->setSleep(true);

  delete magnetometer;

  return 0;
}
