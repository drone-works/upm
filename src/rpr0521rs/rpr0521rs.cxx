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

#include "rpr0521rs.h"

using namespace upm;
using namespace std;

RPR0521RS::RPR0521RS(int bus, uint8_t address) :
  m_i2c(bus), m_gpioIRQ(0)
{
  m_addr = address;

  m_ps_data = 0;
  m_als_data0 = 0;
  m_als_data1 = 0;

  m_mode = 0;
  m_als_data0_gain = ALS_GAIN_1;
  m_als_data1_gain = ALS_GAIN_1;
  m_led_current = LED_CURRENT_25MA;
  m_ps_gain = PS_GAIN_1;

  m_ps_high = 0;
  m_ps_low = 0;
  m_als_high = 0;
  m_als_low = 0;
  m_ps_offset = 0;

  m_int_als = false;
  m_int_ps = false;
  m_int_mode = INT_HIGH;
  m_int_persistence = EOM;

  mraa::Result rv;
  if ( (rv = m_i2c.address(m_addr)) != mraa::SUCCESS)
    {
      throw std::runtime_error(std::string(__FUNCTION__) +
                               ": I2c.address() failed");
      return;
    }
}

RPR0521RS::~RPR0521RS()
{
  uninstallISR();
}

uint8_t
RPR0521RS::readReg(uint8_t reg)
{
  return m_i2c.readReg(reg);
}

void
RPR0521RS::readRegs(uint8_t reg, uint8_t *buffer, int len)
{
  m_i2c.readBytesReg(reg, buffer, len);
}

bool
RPR0521RS::writeReg(uint8_t reg, uint8_t val)
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
RPR0521RS::init()
{
  // Check bm1383 id value
  uint8_t rc;
  rc = readReg(REG_ID);
  if (rc != ID_VAL)
    {
      throw std::runtime_error(std::string(__FUNCTION__) +
			       ": Bad rpr0521rs id value");
      return false;
    }

  return true;
}

bool
RPR0521RS::setSleep(bool enable)
{
  if (enable) {
    writeReg(REG_SWRST, SWRST_INT);
    writeReg(REG_MODE, 0);
    return true;
  }

  uint8_t ri;
  ri = 0;
  if (m_int_ps)
    ri |= INT_PS_EN;
  if (m_int_als)
    ri |= INT_ALS_EN;
  ri |= (m_int_mode << INT_MODE_SHIFT);
  writeReg(REG_INT, ri);

  uint8_t rc;
  rc = ((m_als_data0_gain << ALS_DATA0_GAIN_SHIFT)
	| (m_als_data1_gain << ALS_DATA1_GAIN_SHIFT)
	| m_led_current);
  writeReg(REG_ALS_CNTL, rc);

  rc = ((m_ps_gain << PS_GAIN_SHIFT) | m_int_persistence);
  writeReg(REG_PS_CNTL, rc);

  // set watermarks and offset to registers
  writeReg(REG_ALS_TH_L, m_als_high & 0xff);
  writeReg(REG_ALS_TH_H, m_als_high >> 8);
  writeReg(REG_ALS_TL_L, m_als_low & 0xff);
  writeReg(REG_ALS_TL_H, m_als_low >> 8);
  writeReg(REG_PS_TH_L, m_ps_high & 0xff);
  writeReg(REG_PS_TH_H, m_ps_high >> 8);
  writeReg(REG_PS_TL_L, m_ps_low & 0xff);
  writeReg(REG_PS_TL_H, m_ps_low >> 8);
  writeReg(REG_PS_OFFS_L, m_ps_offset & 0xff);
  writeReg(REG_PS_OFFS_H, m_ps_offset >> 8);

  writeReg(REG_MODE, m_mode);

  return true;
}

bool
RPR0521RS::setMode(bool als, bool ps, bool pswide, bool pstwice,
		   RPR0521RS_LED_CURRENT_T led,
		   RPR0521RS_RATE_T rate)
{
  if (rate < 0 || rate > RATE_20HZ_20HZ)
    throw std::logic_error(string(__FUNCTION__) + 
                             ": internal error, unsupported rate");

  uint8_t mode;
  mode = rate;
  if (als)
    mode |= MODE_ALS_EN;
  if (ps)
    mode |= MODE_PS_EN;
  if (pswide)
    mode |= MODE_PS_PULSE;
  if (pstwice)
    mode |= MODE_PS_TWICE;
  m_mode = mode;

  switch (led) {
  case LED_CURRENT_25MA: case LED_CURRENT_50MA:
  case LED_CURRENT_100MA: case LED_CURRENT_200MA:
    break;
  default:
    throw std::logic_error(string(__FUNCTION__) + 
                             ": internal error, unsupported led current");
  }
  m_led_current = led;
  
  return true;
}

bool
RPR0521RS::setAmbientLightGain(RPR0521RS_ALS_GAIN_T gain0,
			       RPR0521RS_ALS_GAIN_T gain1)
{
  switch (gain0) {
  case ALS_GAIN_1: case ALS_GAIN_2: case ALS_GAIN_64: case ALS_GAIN_128:
    break;
  default:
    throw std::logic_error(string(__FUNCTION__) + 
                             ": internal error, unsupported gain");
  }
  switch (gain1) {
  case ALS_GAIN_1: case ALS_GAIN_2: case ALS_GAIN_64: case ALS_GAIN_128:
    break;
  default:
    throw std::logic_error(string(__FUNCTION__) + 
                             ": internal error, unsupported gain");
  }

  m_als_data0_gain = gain0;
  m_als_data1_gain = gain1;
  
  return true;
}

bool
RPR0521RS::setProximityGain(RPR0521RS_PS_GAIN_T gain)
{
  switch (gain) {
  case PS_GAIN_1: case PS_GAIN_2: case PS_GAIN_4:
    break;
  default:
    throw std::logic_error(string(__FUNCTION__) + 
                             ": internal error, unsupported gain");
  }
  m_ps_gain = gain;
  
  return true;
}

bool
RPR0521RS::setAmbientLightWatermark(uint16_t high, uint16_t low)
{
  m_als_high = high;
  m_als_low = low;
  
  return true;
}

bool
RPR0521RS::setProximityWatermark(uint16_t high, uint16_t low)
{
  if (high > 0xfff)
    throw std::logic_error(string(__FUNCTION__) + 
                             ": internal error, bad high watermark value");
  if (low > 0xfff)
    throw std::logic_error(string(__FUNCTION__) + 
                             ": internal error, bad low watermark value");

  m_ps_high = high;
  m_ps_low = low;

  return true;
}

bool
RPR0521RS::setProximityOffset(uint16_t offs)
{
  if (offs > 0x3ff)
    throw std::logic_error(string(__FUNCTION__) + 
                             ": internal error, bad offset value");
  m_ps_offset = offs;

  return true;
}

bool
RPR0521RS::setInterruptMode(bool ps, bool als,
			    RPR0521RS_INT_MODE_T mode,
			    RPR0521RS_PERSISTENCE_T persist)
{
  switch (mode) {
  case INT_HIGH: case INT_HYSTERESIS: case INT_OUT_OF_RANGE:
    break;
  default:
    throw std::logic_error(string(__FUNCTION__) + 
                             ": internal error, unsupported mode");
  }
  switch (persist) {
  case EOM: case UPDATE_EOM: case UPDATE_TWICE: case UPDATE_PERSIST:
    break;
  default:
    throw std::logic_error(string(__FUNCTION__) + 
                             ": internal error, unsupported persistence");
  }
  
  m_int_ps = ps;
  m_int_als = als;
  m_int_mode = mode;
  m_int_persistence = persist;

  return true;
}

bool
RPR0521RS::getInterruptStatus(bool& ps, bool& als)
{
  uint8_t ri;
  ri = readReg(REG_INT);
  ps = ((ri & INT_PS_STATUS) != 0);
  als = ((ri & INT_ALS_STATUS) != 0);

  return true;
}

bool
RPR0521RS::clearInterrupt()
{
  writeReg(REG_SWRST, SWRST_INT);

  return true;
}

bool
RPR0521RS::getAmbientLight(uint16_t *x)
{
  uint8_t buffer[2];
  readRegs(REG_ALS_DADA0_L, buffer, sizeof(buffer));

  m_als_data0 = (uint16_t)((buffer[1] << 8) | buffer[0]);
  // ALS 20Hz(50ms) mode needs a special handling
  if ((m_mode & MODE_RATE_MASK) == RATE_20HZ_20HZ) {
    if (m_als_data0 & 0x8000)
      m_als_data0 = 0x7fff;
  }

  if (x)
    *x = m_als_data0;

  return true;
}

bool
RPR0521RS::getAmbientLightIr(uint16_t *x)
{
  uint8_t buffer[2];
  readRegs(REG_ALS_DADA1_L, buffer, sizeof(buffer));

  m_als_data1 = (uint16_t)((buffer[1] << 8) | buffer[0]);
  // ALS 20Hz(50ms) mode needs a special handling
  if ((m_mode & MODE_RATE_MASK) == RATE_20HZ_20HZ) {
    if (m_als_data1 & 0x8000)
      m_als_data1 = 0x7fff;
  }
      
  if (x)
    *x = m_als_data1;

  return true;
}

bool
RPR0521RS::getProximity(uint16_t *x)
{
  uint8_t buffer[2];
  readRegs(REG_PS_DADA_L, buffer, sizeof(buffer));

  m_ps_data = (uint16_t)((buffer[1] << 8) | buffer[0]);

  if (x)
    *x = m_ps_data;

  return true;
}

#if defined(SWIGJAVA) || defined(JAVACALLBACK)
void RPR0521RS::installISR(int gpio, jobject runnable)
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
RPR0521RS::installISR(int gpio, void (*isr)(void *), void *arg)
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
RPR0521RS::uninstallISR()
{
  if (m_gpioIRQ)
    {
      m_gpioIRQ->isrExit();
      delete m_gpioIRQ;

      m_gpioIRQ = 0;
    }
}
