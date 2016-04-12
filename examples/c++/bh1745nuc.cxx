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
#include "bh1745nuc.h"

using namespace std;
using namespace upm;

int shouldRun = true;
upm::BH1745NUC* colorsensor;

void
sig_handler(int signo)
{
  if (signo == SIGINT)
    shouldRun = false;
}

void
data_callback(void* data)
{
  bool status;
  colorsensor->getInterruptStatus(status);

  if (status) {
    uint16_t r, g, b, c;
    colorsensor->getColorData(&r, &g, &b, &c);
    printf("R: 0x%04x  G: 0x%04x  B: 0x%04x  C: 0x%04x\n", r, g, b, c);
    // usleep(100);
  }

  colorsensor->clearInterrupt();
}

int
main()
{
  signal(SIGINT, sig_handler);
  //! [Interesting]
  // Instantiate a BH1745NUC Color Sensor
  colorsensor = new upm::BH1745NUC();
  colorsensor->init();
  colorsensor->installISR(24, data_callback, NULL);

  colorsensor->setWatermark(200, 0x0);
  colorsensor->setInterruptMode(true, BH1745NUC::INT_CLEAR,
				BH1745NUC::EOM);
  colorsensor->setMode(BH1745NUC::RATE_160MS);
  colorsensor->setSleep(false);

  while (shouldRun) {
    sleep(1);
  }

  //! [Interesting]
  cout << "Exiting" << endl;

  colorsensor->setSleep(true);

  delete colorsensor;

  return 0;
}
