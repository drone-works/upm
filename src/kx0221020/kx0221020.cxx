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

#include "kx0221020.h"

using namespace upm;
using namespace std;

KX0221020::KX0221020(int bus, uint8_t address) :
  m_i2c(bus), m_gpioIRQ(0)
{
  m_addr = address;

  m_accelX = 0.0;
  m_accelY = 0.0;
  m_accelZ = 0.0;

  m_accelScale = 16384.0;
  m_rate = 50.0;

  m_sleep = true;

  mraa::Result rv;
  if ( (rv = m_i2c.address(m_addr)) != mraa::SUCCESS)
    {
      throw std::runtime_error(std::string(__FUNCTION__) +
                               ": I2c.address() failed");
      return;
    }
}

KX0221020::~KX0221020()
{
  uninstallISR();
}

uint8_t
KX0221020::readReg(uint8_t reg)
{
  return m_i2c.readReg(reg);
}

bool
KX0221020::writeReg(uint8_t reg, uint8_t val)
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
KX0221020::init()
{
  // Check kx022 id (who_am_i) value
  uint8_t rc;
  rc = readReg(REG_WHO_AM_I);
  if (rc != WHO_AM_I_VAL)
    {
      throw std::runtime_error(std::string(__FUNCTION__) +
			       ": Bad kx022 who_am_i value");
      return false;
    }

  // Some configure oparations can be done only on stand-by mode

  // High current, enable deta ready interrupt
  writeReg(REG_CNTL1, (CNTL1_RES|CNTL1_DRDYE));
  // set output data rate to 50Hz
  writeReg(REG_ODCNTL, RATE_50HZ);
  // Enable interrupt, active low, latches until INT_REL read
  writeReg(REG_INC1, INC1_IEN1);
  // Enable buffer full, watermark, data ready interrupts on INT1 pin
  writeReg(REG_INC4, (INC4_BFI1|INC4_WMI1|INC4_DRDYI1));

  // set sample buffer watermark to 32
  writeReg(REG_BUF_CNTL1, 32);
  // use 16-bit sample buffer at FIFO mode.  Enable buffer full interrupt
  writeReg(REG_BUF_CNTL2, (BUF_CNTL2_BFE|BUF_CNTL2_BRES|BUF_CNTL2_BFIE));
  // clear buffer
  writeReg(REG_BUF_CLEAR, 0);

  // clear latched interrupt
  (void)readReg(REG_INT_REL);
  
  // then wake up
  if (!setSleep(false))
    {
      throw std::runtime_error(std::string(__FUNCTION__) +
			       ": Unable to wake up device");
      return false;
    }

  // Wait 1.2/defaultODR=24ms
  usleep(24000);

  return true;
}

bool
KX0221020::setSleep(bool enable)
{
  if (m_sleep == enable)
    return true;

  m_sleep = enable;

  uint8_t rc;
  rc = readReg(REG_CNTL1);
  if (enable)
    rc &= ~CNTL1_PC1;
  else
    rc |= CNTL1_PC1;
  writeReg(REG_CNTL1, rc);

  // Wait 1.2/ODR sec
  usleep(int(1200000/m_rate));

  return true;
}

bool
KX0221020::setRange(KX0221020_RANGE_T range)
{
  if (!m_sleep)
    throw std::logic_error(string(__FUNCTION__) + 
			   ": internal error, can be called only on sleep");

  uint8_t gsel;

  switch (range) {
  case RANGE_2G:
    m_accelScale = 16384.0;
    gsel = 0;
    break;
  case RANGE_4G:
    m_accelScale = 8192.0;
    gsel = CNTL1_GSEL0;
    break;
  case RANGE_8G:
    m_accelScale = 4096.0;
    gsel = CNTL1_GSEL1;
    break;
  default: // should never occur, but...
    m_accelScale = 1.0;	// set a safe, though incorrect value
    throw std::logic_error(string(__FUNCTION__) + 
			   ": internal error, unsupported range");
    break;
  }

  uint8_t rc;
  rc = readReg(REG_CNTL1);
  rc = (rc & ~(CNTL1_GSEL1|CNTL1_GSEL0))|gsel;
  writeReg(REG_CNTL1, rc);
  return true;
}

bool
KX0221020::setRate(KX0221020_RATE_T rate)
{
  if (!m_sleep)
    throw std::logic_error(string(__FUNCTION__) + 
			   ": internal error, can be called only on sleep");

  switch (rate) {
  case RATE_12_5HZ:	m_rate = 12.5; break;
  case RATE_25HZ:	m_rate = 25.0; break;
  case RATE_50HZ:	m_rate = 50.0; break;
  case RATE_100HZ:	m_rate = 100.0; break;
  case RATE_200HZ:	m_rate = 200.0; break;
  case RATE_400HZ:	m_rate = 400.0; break;
  case RATE_800HZ:	m_rate = 800.0; break;
  case RATE_1600HZ:	m_rate = 1600.0; break;
  case RATE_0_781HZ:	m_rate = 0.781; break;
  case RATE_1_563HZ:	m_rate = 1.563; break;
  case RATE_3_125HZ:	m_rate = 3.125; break;
  case RATE_6_25HZ:	m_rate = 6.25; break;
  default: // should never occur, but...
    m_rate = 1.0; // set a safe, though incorrect value
    throw std::logic_error(string(__FUNCTION__) + 
			   ": internal error, unsupported rate");
    break;
  }

  writeReg(REG_ODCNTL, (uint8_t)rate);

  return true;
}

bool
KX0221020::getInterruptStatus(bool& full, bool& rdy, bool& watermark)
{
  uint8_t sr;
  sr = readReg(REG_STATUS_REG);

  uint8_t ins;
  ins = readReg(REG_INS2);
  full = ((ins & INS2_BFI) != 0);
  rdy = ((ins & INS2_DRDY) != 0);
  watermark = ((ins & INS2_WMI) != 0);

  return (sr != 0);
}

bool
KX0221020::clearInterrupt()
{
  (void)readReg(REG_INT_REL);
}

int
KX0221020::getCount()
{
  // get number of data bytes in FIFO
  uint8_t count;
  count = readReg(REG_BUF_STATUS_1);

  // 6 bytes for 3-axis
  return count/6;
}

bool
KX0221020::getAccelerometer(float *x, float *y, float *z)
{
  uint8_t buffer[6];
  for (int i = 0; i < 6; i++)
    buffer[i] = readReg(REG_BUF_READ);

  int16_t ax, ay, az;
  ax = ((buffer[1] << 8) | buffer[0]);
  ay = ((buffer[3] << 8) | buffer[2]);
  az = ((buffer[5] << 8) | buffer[4]);

  m_accelX = float(ax);
  m_accelY = float(ay);
  m_accelZ = float(az);

  if (x)
    *x = m_accelX/m_accelScale;

  if (y)
    *y = m_accelY/m_accelScale;

  if (z)
    *z = m_accelZ/m_accelScale;

  return true;
}

#if defined(SWIGJAVA) || defined(JAVACALLBACK)
void KX0221020::installISR(int gpio, jobject runnable)
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
KX0221020::installISR(int gpio, void (*isr)(void *), void *arg)
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
KX0221020::uninstallISR()
{
  if (m_gpioIRQ)
    {
      m_gpioIRQ->isrExit();
      delete m_gpioIRQ;

      m_gpioIRQ = 0;
    }
}
