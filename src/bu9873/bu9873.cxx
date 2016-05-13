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
#include <sstream>
#include <string.h>

#include "bu9873.h"

using namespace upm;
using namespace std;

BU9873::BU9873(int bus, uint8_t address) :
  m_i2c(bus)
{
  m_addr = address;

  amPmMode = false;
  pm = false;

  mraa::Result rv;
  if ( (rv = m_i2c.address(m_addr)) != mraa::SUCCESS)
    {
      throw std::runtime_error(std::string(__FUNCTION__) +
                               ": I2c.address() failed");
      return;
    }
}

BU9873::~BU9873()
{
}

uint8_t
BU9873::readReg(uint8_t reg)
{
  return m_i2c.readReg(reg);
}

void
BU9873::readRegs(uint8_t reg, uint8_t *buffer, int len)
{
  m_i2c.readBytesReg(reg, buffer, len);
}

bool
BU9873::writeReg(uint8_t reg, uint8_t val)
{
  mraa::Result rv;
  if ((rv = m_i2c.writeReg(reg, val)) != mraa::SUCCESS)
    {
      throw std::runtime_error(std::string(__FUNCTION__) +
			       ": I2c.writeReg() failed");
      return false;
    } 
  
  return true;
}

bool
BU9873::writeRegs(uint8_t reg, uint8_t *buffer, int len)
{
  // create a buffer 1 byte larger than the supplied buffer, store
  // the register in the first byte and copy buffer to other bytes
  uint8_t buf2[len + 1];

  buf2[0] = reg;
  for (int i = 0; i < len; i++)
    buf2[i+1] = buffer[i];

  mraa::Result rv;
  if ((rv = m_i2c.write(buf2, len + 1)) != mraa::SUCCESS)
    {
      throw std::runtime_error(std::string(__FUNCTION__) +
			       ": I2c.writeRegs() failed");
      return false;
    } 
  
  return true;
}

// Convert decimal to BCD
uint8_t BU9873::decToBcd(unsigned int val)
{
  return ((val/10*16) + (val%10));
}

// Convert BCD to decimal
unsigned int BU9873::bcdToDec(uint8_t val)
{
  return ((val/16*10) + (val%16));
}

bool
BU9873::loadTime()
{
  uint8_t buf[8];
  // Read REG_CTRL2 first and then REG_SECONDS - REG_YEAR
  readRegs(REG_CTRL2, buf, sizeof (buf));

  amPmMode = !!(buf[0] & CTRL2_12_24);
  seconds = bcdToDec(buf[1]);
  minutes = bcdToDec(buf[2]);
  if (amPmMode)
    {
      hours = bcdToDec(buf[3] & 0x1f);
      pm = !!(buf[3] & 0x20);
    }
  else
    hours = bcdToDec(buf[3]);

  dayOfWeek = bcdToDec(buf[4]);
  dayOfMonth = bcdToDec(buf[5]);
  month = bcdToDec(buf[6]);
  year = bcdToDec(buf[7]);

  return true;
}

bool
BU9873::setTime()
{
  uint8_t buf[8];
  buf[0] = (amPmMode ? CTRL2_12_24 : 0);
  buf[1] = decToBcd(seconds);
  buf[2] = decToBcd(minutes);
  buf[3] = decToBcd(hours) | ((amPmMode && pm) ? 0x40 : 0);
  buf[4] = decToBcd(dayOfWeek);
  buf[5] = decToBcd(dayOfMonth);
  buf[6] = decToBcd(month);
  buf[7] = decToBcd(year);

  return writeRegs(REG_CTRL2, buf, 8);
}
