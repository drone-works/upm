/*
  # Copyright (c) 2016 Droneworks.
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
#include <time.h>
#include "bu9873.h"

using namespace std;

static const char *DoW[] = {
  "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};
static const char *MoY[] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep",
  "Oct", "Nov", "Dec"
};

int
main(int argc, char **argv)
{
  //! [Interesting]
  // Instantiate a BU9873 RTC on I2C
  upm::BU9873 *rtc = new upm::BU9873();

  if (argc == 2 && argv[1][0] == '-' && argv[1][1] == 's')
    {
      time_t ti;
      time(&ti);
      struct tm *tp = localtime(&ti);
      rtc->dayOfWeek = tp->tm_wday;
      rtc->month = tp->tm_mon;
      rtc->dayOfMonth = tp->tm_mday;
      rtc->hours = tp->tm_hour;
      rtc->minutes = tp->tm_min;
      rtc->seconds = tp->tm_sec;
      rtc->year = tp->tm_year - 100;
      rtc->amPmMode = false;
      cout << "Set BU9873 RTC to localtime." << endl;
      if (!rtc->setTime())
	{
	  delete rtc;
	  return EXIT_FAILURE;
	}
    }
  else if (!rtc->loadTime())
    {
      delete rtc;
      return EXIT_FAILURE;
    }

  printf ("%s %s %2d %02d:%02d:%02d     %4d\n",
	  DoW[rtc->dayOfWeek], MoY[rtc->month], rtc->dayOfMonth,
	  rtc->hours, rtc->minutes, rtc->seconds, rtc->year + 2000);

  delete rtc;
  return 0;
}
