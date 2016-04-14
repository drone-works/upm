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

#include "bm1422gmv.h"

using namespace upm;
using namespace std;

BM1422GMV::BM1422GMV(int bus, uint8_t address) :
  m_i2c(bus), m_gpioIRQ(0)
{
  m_addr = address;

  m_dataX = 0.0;
  m_dataY = 0.0;
  m_dataZ = 0.0;

  m_offset_adjust = false;
  m_offsetX = 0;
  m_offsetY = 0;
  m_offsetZ = 0;

  m_mode = MODE_CONTINUOUS;
  m_prec = PREC_14BIT;
  m_rate = RATE_10HZ;
  m_aver = AVER_4;

  mraa::Result rv;
  if ( (rv = m_i2c.address(m_addr)) != mraa::SUCCESS)
    {
      throw std::runtime_error(std::string(__FUNCTION__) +
                               ": I2c.address() failed");
      return;
    }
}

BM1422GMV::~BM1422GMV()
{
  uninstallISR();
}

uint8_t
BM1422GMV::readReg(uint8_t reg)
{
  return m_i2c.readReg(reg);
}

void
BM1422GMV::readRegs(uint8_t reg, uint8_t *buffer, int len)
{
  m_i2c.readBytesReg(reg, buffer, len);
}

bool
BM1422GMV::writeReg(uint8_t reg, uint8_t val)
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
BM1422GMV::writeWordReg(uint8_t reg, uint16_t val)
{
  mraa::Result rv;
  if ((rv = m_i2c.writeWordReg(reg, val)) != mraa::SUCCESS)
    {
      throw std::runtime_error(std::string(__FUNCTION__) +
			       ": I2c.writeWordReg() failed");
      return false;
    } 
  
  return true;
}

bool
BM1422GMV::init()
{
  // Check bm1422 id (who_am_i) value
  uint8_t rc;
  rc = readReg(REG_WHO_AM_I);
  if (rc != WHO_AM_I_VAL)
    {
      throw std::runtime_error(std::string(__FUNCTION__) +
			       ": Bad bm1422gmv who_am_i value");
      return false;
    }

  return true;
}

bool
BM1422GMV::setSleep(bool enable)
{
  if (enable) {
    writeReg(REG_CNTL1, (CNTL1_PC1|CNTL1_RST_LV));
    return true;
  }

  uint8_t rc;
  rc = CNTL1_PC1;
  if (m_mode == MODE_SINGLE)
    rc |= CNTL1_FS1;
  if (m_prec == PREC_14BIT)
    rc |= CNTL1_OUT_BIT;
  switch (m_rate) {
  case RATE_20HZ:
    rc |= CNTL1_ODR1; break;
  case RATE_100HZ:
    rc |= CNTL1_ODR0; break;
  case RATE_1000HZ:
    rc |= (CNTL1_ODR1|CNTL1_ODR0); break;
  case RATE_10HZ:
  default:
    break;
  }

  writeReg(REG_CNTL1, rc);
  writeWordReg(REG_CNTL4, 0);
  // Set filter
  writeReg(REG_AVER, m_aver);
  // enable DRDY with setting it as active low
  writeReg(REG_CNTL2, CNTL2_DREN);
  if (m_offset_adjust)
    {
      writeWordReg(REG_OFFX_L, m_offsetX);
      writeWordReg(REG_OFFY_L, m_offsetY);
      writeWordReg(REG_OFFZ_L, m_offsetZ);
    }
  // trigger conversion
  writeReg(REG_CNTL3, CNTL3_FORCE);

  return true;
}

bool
BM1422GMV::setMode(BM1422GMV_MODE_T mode, BM1422GMV_PREC_T prec,
		   BM1422GMV_RATE_T rate)
{
  switch (mode) {
  case MODE_CONTINUOUS: case MODE_SINGLE: break;
  default:
    throw std::logic_error(string(__FUNCTION__) + 
                             ": internal error, unsupported mode");
  }
  m_mode = mode;
  switch (prec) {
  case PREC_14BIT: case PREC_12BIT: break;
  default:
    throw std::logic_error(string(__FUNCTION__) + 
                             ": internal error, unsupported precision");
  }
  m_prec = prec;
  switch (rate) {
  case RATE_10HZ: case RATE_20HZ: case RATE_100HZ: case RATE_1000HZ: break;
  default:
    throw std::logic_error(string(__FUNCTION__) + 
                             ": internal error, unsupported rate");
  }
  m_rate = rate;
  
  return true;
}

bool
BM1422GMV::setAverageFilter(BM1422GMV_AVER_T aver)
{
  switch (aver) {
  case AVER_1: case AVER_2: case AVER_4: case AVER_8: case AVER_16: break;
  default:
    throw std::logic_error(string(__FUNCTION__) + 
                             ": internal error, unsupported # of average");
  }
  m_aver = aver;

  return true;
}

bool
BM1422GMV::getStatus(bool& drdy)
{
  drdy = ((readReg(REG_STA1) & STA1_DRDY) != 0);

  return true;
}

bool
BM1422GMV::getMagnetometer(float *x, float *y, float *z)
{
  uint8_t buffer[6];
  readRegs(REG_DATAX_L, buffer, sizeof(buffer));

  int16_t ax, ay, az;
  ax = ((buffer[1] << 8) | buffer[0]);
  ay = ((buffer[3] << 8) | buffer[2]);
  az = ((buffer[5] << 8) | buffer[4]);

  m_dataX = float(ax);
  m_dataY = float(ay);
  m_dataZ = float(az);

  float scale = (m_prec == PREC_14BIT)? 24 : 6;
  if (x)
    *x = m_dataX/scale;

  if (y)
    *y = m_dataY/scale;

  if (z)
    *z = m_dataZ/scale;

  return true;
}

#if defined(SWIGJAVA) || defined(JAVACALLBACK)
void BM1422GMV::installISR(int gpio, jobject runnable)
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
BM1422GMV::installISR(int gpio, void (*isr)(void *), void *arg)
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
BM1422GMV::uninstallISR()
{
  if (m_gpioIRQ)
    {
      m_gpioIRQ->isrExit();
      delete m_gpioIRQ;

      m_gpioIRQ = 0;
    }
}
