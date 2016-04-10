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

#include "bm1383glv.h"

using namespace upm;
using namespace std;

BM1383GLV::BM1383GLV(int bus, uint8_t address) :
  m_i2c(bus), m_gpioIRQ(0)
{
  m_addr = address;

  m_press = 0.0;

  m_mode = MODE_RATE_10HZ;
  m_aver = AVER_NO;
  m_taver = false;

  m_high = 0.0;
  m_low = 0.0;
  m_enable_high = true;
  m_enable_low = false;

  mraa::Result rv;
  if ( (rv = m_i2c.address(m_addr)) != mraa::SUCCESS)
    {
      throw std::runtime_error(std::string(__FUNCTION__) +
                               ": I2c.address() failed");
      return;
    }
}

BM1383GLV::~BM1383GLV()
{
  uninstallISR();
}

uint8_t
BM1383GLV::readReg(uint8_t reg)
{
  return m_i2c.readReg(reg);
}

void
BM1383GLV::readRegs(uint8_t reg, uint8_t *buffer, int len)
{
  m_i2c.readBytesReg(reg, buffer, len);
}

bool
BM1383GLV::writeReg(uint8_t reg, uint8_t val)
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
BM1383GLV::init()
{
  // Check bm1383 id value
  uint8_t rc;
  rc = readReg(REG_ID);
  if (rc != ID_VAL)
    {
      throw std::runtime_error(std::string(__FUNCTION__) +
			       ": Bad bm1383glv id value");
      return false;
    }

  // Power up
  writeReg(REG_PWR, PWR_ON);
  
  return true;
}

bool
BM1383GLV::setSleep(bool enable)
{
  if (enable) {
    writeReg(REG_SWRST, SWRST_INT);
    writeReg(REG_MODE, 0);
    writeReg(REG_RST, 0);
    return true;
  }

  writeReg(REG_RST, RST_RSTB);

  if (m_enable_high) {
    writeReg(REG_PDTH_H_H, m_high >> 8);
    writeReg(REG_PDTH_H_L, m_high & 0xff);
  }
  if (m_enable_low) {
    writeReg(REG_PDTH_L_H, m_low >> 8);
    writeReg(REG_PDTH_L_L, m_low & 0xff);
  }
  uint8_t ri;
  ri = 0;
  if (m_enable_high)
    ri |= INT_H_EN;
  if (m_enable_low)
    ri |= INT_L_EN;
  if (!m_pullup)
    ri |= INT_PU_EN;
  if (m_enable_high || m_enable_low)
    ri |= INT_EN;
  writeReg(REG_INT, ri);
  
  uint8_t rc;
  rc = (m_aver << AVER_SHIFT)|m_mode;
  if (m_taver)
    rc |= T_AVE;
  writeReg(REG_MODE, rc);

  return true;
}

bool
BM1383GLV::setMode(BM1383GLV_MODE_T mode)
{
  switch (mode) {
  case MODE_NO: case MODE_ONESHOT: case MODE_RATE_20HZ:
  case MODE_RATE_10HZ: case MODE_RATE_5HZ:
    break;
  default:
    throw std::logic_error(string(__FUNCTION__) + 
                             ": internal error, unsupported mode");
  }
  m_mode = mode;
  
  return true;
}

bool
BM1383GLV::setAverageFilter(BM1383GLV_AVER_T aver, bool taver)
{
  switch (aver) {
  case AVER_NO: case AVER_2: case AVER_4: case AVER_8: case AVER_16:
  case AVER_32: case AVER_64: break;
  default:
    throw std::logic_error(string(__FUNCTION__) + 
                             ": internal error, unsupported # of average");
  }
  m_aver = aver;
  m_taver = taver;

  return true;
}

bool
BM1383GLV::setWatermark(float high, float low)
{
  high *= 32;
  low *= 32;
  if (high < 0 || (int)high > 0xffff)
    throw std::logic_error(string(__FUNCTION__) + 
                             ": internal error, bad high watermark value");
  if (low < 0 || (int)low > 0xffff)
    throw std::logic_error(string(__FUNCTION__) + 
                             ": internal error, bad high watermark value");

  m_high = 0xffff & (int)high;
  m_low = 0xffff & (int)low;

  return true;
}

bool
BM1383GLV::setInterruptMode(bool high, bool low, bool pullup)
{
  m_enable_high = high;
  m_enable_low = low;
  m_pullup = pullup;

  return true;
}

bool
BM1383GLV::getInterruptStatus(bool& high, bool& low)
{
  uint8_t ri;
  ri = readReg(REG_INT);
  high = ((ri & INT_H_STATUS) != 0);
  low = ((ri & INT_L_STATUS) != 0);

  return true;
}

bool
BM1383GLV::clearInterrupt()
{
  writeReg(REG_SWRST, SWRST_INT);

  return true;
}

bool
BM1383GLV::getPressure(float *x)
{
  uint8_t buffer[6];
  readRegs(REG_PRESS_H, buffer, sizeof(buffer));

  uint32_t raw;
  raw = ((buffer[0] << 14) | (buffer[1] << 6) | (buffer[2] >> 2));
  m_press = float(raw);

  if (x)
    *x = m_press/2048;

  return true;
}

#if defined(SWIGJAVA) || defined(JAVACALLBACK)
void BM1383GLV::installISR(int gpio, jobject runnable)
{
  // delete any existing ISR and GPIO context
  uninstallISR();

  // greate gpio context
  m_gpioIRQ = new mraa::Gpio(gpio);

  m_gpioIRQ->dir(mraa::DIR_IN);
  m_gpioIRQ->isr(mraa::EDGE_FALLING, runnable);
}
#else
void
BM1383GLV::installISR(int gpio, void (*isr)(void *), void *arg)
{
  // delete any existing ISR and GPIO context
  uninstallISR();

  // greate gpio context
  m_gpioIRQ = new mraa::Gpio(gpio);

  m_gpioIRQ->dir(mraa::DIR_IN);
  m_gpioIRQ->isr(mraa::EDGE_FALLING, isr, arg);
}
#endif

void
BM1383GLV::uninstallISR()
{
  if (m_gpioIRQ)
    {
      m_gpioIRQ->isrExit();
      delete m_gpioIRQ;

      m_gpioIRQ = 0;
    }
}
