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

#include "bh1745nuc.h"

using namespace upm;
using namespace std;

BH1745NUC::BH1745NUC(int bus, uint8_t address) :
  m_i2c(bus), m_gpioIRQ(0)
{
  m_addr = address;

  m_red = m_green = m_blue = m_clear = 0;

  m_rate = RATE_160MS;
  m_gain = GAIN_1;

  m_high = 0;
  m_low = 0;

  m_int_enable = false;
  m_int_source = INT_CLEAR;
  m_int_persistence = EOM;

  mraa::Result rv;
  if ( (rv = m_i2c.address(m_addr)) != mraa::SUCCESS)
    {
      throw std::runtime_error(std::string(__FUNCTION__) +
                               ": I2c.address() failed");
      return;
    }
}

BH1745NUC::~BH1745NUC()
{
  uninstallISR();
}

uint8_t
BH1745NUC::readReg(uint8_t reg)
{
  return m_i2c.readReg(reg);
}

void
BH1745NUC::readRegs(uint8_t reg, uint8_t *buffer, int len)
{
  m_i2c.readBytesReg(reg, buffer, len);
}

bool
BH1745NUC::writeReg(uint8_t reg, uint8_t val)
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
BH1745NUC::init()
{
  // Check BH1745NUC id value
  uint8_t rc;
  rc = readReg(REG_ID);
  if (rc != ID_VAL)
    {
      throw std::runtime_error(std::string(__FUNCTION__) +
			       ": Bad bh1745nuc id value");
      return false;
    }

  return true;
}

bool
BH1745NUC::setSleep(bool enable)
{
  if (enable) {
    writeReg(REG_SWRST, SWRST_INT);
    writeReg(REG_MODE2, 0);
    return true;
  }

  uint8_t ri;
  ri = 0;
  if (m_int_enable)
    ri |= INT_EN;
  ri |= (m_int_source << INT_SOURCE_SHIFT);
  writeReg(REG_INT, ri);

  writeReg(REG_PERSISTENCE, m_int_persistence);

  // set watermarks and offset to registers
  writeReg(REG_TH_L, m_high & 0xff);
  writeReg(REG_TH_H, m_high >> 8);
  writeReg(REG_TL_L, m_low & 0xff);
  writeReg(REG_TL_H, m_low >> 8);

  writeReg(REG_MODE1, m_rate);
  writeReg(REG_MODE2, (MODE2_RGBC_EN | m_gain));

  return true;
}

bool
BH1745NUC::setMode(BH1745NUC_RATE_T rate)
{
  if (rate < 0 || rate > RATE_5120MS)
    throw std::logic_error(string(__FUNCTION__) + 
                             ": internal error, unsupported rate");

  m_rate = rate;
  
  return true;
}

bool
BH1745NUC::setGain(BH1745NUC_GAIN_T gain)
{
  switch (gain) {
  case GAIN_1: case GAIN_2: case GAIN_16:
    break;
  default:
    throw std::logic_error(string(__FUNCTION__) + 
                             ": internal error, unsupported gain");
  }

  m_gain = gain;
  
  return true;
}

bool
BH1745NUC::setWatermark(uint16_t high, uint16_t low)
{
  m_high = high;
  m_low = low;
  
  return true;
}

bool
BH1745NUC::setInterruptMode(bool enable,
			    BH1745NUC_INT_SOURCE_T source,
			    BH1745NUC_PERSISTENCE_T persist)
{
  switch (source) {
  case INT_RED: case INT_GREEN: case INT_BLUE: case INT_CLEAR:
    break;
  default:
    throw std::logic_error(string(__FUNCTION__) + 
                             ": internal error, unsupported source");
  }
  switch (persist) {
  case EOM: case UPDATE_EOM: case UPDATE_4TIMES: case UPDATE_8TIMES:
    break;
  default:
    throw std::logic_error(string(__FUNCTION__) + 
                             ": internal error, unsupported persistence");
  }
  
  m_int_enable = enable;
  m_int_source = source;
  m_int_persistence = persist;

  return true;
}

bool
BH1745NUC::getInterruptStatus(bool& status)
{
  uint8_t ri;
  ri = readReg(REG_INT);
  status = ((ri & INT_STATUS) != 0);

  return true;
}

bool
BH1745NUC::clearInterrupt()
{
  writeReg(REG_SWRST, SWRST_INT);

  return true;
}

bool
BH1745NUC::getColorData(uint16_t *r, uint16_t *g, uint16_t *b, uint16_t *c)
{
  uint8_t buffer[8];
  readRegs(REG_RED_L, buffer, sizeof(buffer));

  m_red = (uint16_t)((buffer[1] << 8) | buffer[0]);
  m_green = (uint16_t)((buffer[3] << 8) | buffer[2]);
  m_blue = (uint16_t)((buffer[5] << 8) | buffer[4]);
  m_clear = (uint16_t)((buffer[7] << 8) | buffer[6]);

  if (r)
    *r = m_red;
  if (g)
    *g = m_green;
  if (b)
    *b = m_blue;
  if (c)
    *c = m_clear;

  return true;
}

#if defined(SWIGJAVA) || defined(JAVACALLBACK)
void BH1745NUC::installISR(int gpio, jobject runnable)
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
BH1745NUC::installISR(int gpio, void (*isr)(void *), void *arg)
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
BH1745NUC::uninstallISR()
{
  if (m_gpioIRQ)
    {
      m_gpioIRQ->isrExit();
      delete m_gpioIRQ;

      m_gpioIRQ = 0;
    }
}
