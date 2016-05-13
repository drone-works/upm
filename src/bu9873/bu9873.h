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

#pragma once

#include <string>
#include <mraa/common.hpp>
#include <mraa/i2c.hpp>

#include <mraa/gpio.hpp>

#define BU9873_I2C_BUS 0
#define BU9873_I2C_ADDR 0x32

namespace upm {
  
  /**
   * @library bu9873
   * @sensor bu9873
   * @comname BU9873 RTC (Real-Time Clock)
   * @type time
   *
   * @man rohm
   * @con i2c
   *
   * @snippet bu9873.cxx Interesting
   */
  class BU9873 {
  public:
    /**
     * bu9873 constructor
     *
     * @param bus i2c bus to use
     * @param address the address for this device
     */
    BU9873(int bus=BU9873_I2C_BUS,
	   uint8_t address=BU9873_I2C_ADDR);

    /**
     * bu9873 Destructor
     */
    virtual ~BU9873();

    /**
     * Loads all the time values
     *
     * @return True if time data loaded successfully
     */
    bool loadTime();

    /**
     * Sets the time. You should call loadTime() beforehand to
     * maintain consistency
     *
     * @return True if time is set successfully
     */
    bool setTime();

    /**
     * Holds seconds
     */
    unsigned int seconds;
    /**
     * Holds minutes
     */
    unsigned int minutes;
    /**
     * Holds hours; 1-12 in the am/pm format, 0-23 otherwise
     */
    unsigned int hours;
    /**
     * Holds a day of the week; 1-7, where 1 is Sunday
     */
    unsigned int dayOfWeek;
    /**
     * Holds a day of the month, 1-31
     */
    unsigned int dayOfMonth;
    /**
     * Holds a month, 1-12
     */
    unsigned int month;
    /**
     * Holds a year, 0-99
     */
    unsigned int year;
    /**
     * True if the am/pm format is used, false otherwise
     */
    bool amPmMode;
    /**
     * For the am/pm format, it is true if it's pm, false otherwise
     */
    bool pm;

  protected:
    // BU9873 I2C registers
    typedef enum {
      REG_SECONDS		= 0x00,
      REG_MINUTES		= 0x10,
      REG_HOURS			= 0x20,
      REG_DAYOFWEEK		= 0x30,
      REG_DAY			= 0x40,
      REG_MONTH			= 0x50,
      REG_YEAR			= 0x60,
      REG_ADJUST		= 0x70,
      REG_ALARM_A_MIN		= 0x80,
      REG_ALARM_A_HOUR		= 0x90,
      REG_ALARM_A_DOW		= 0xa0,
      REG_ALARM_B_MIN		= 0xb0,
      REG_ALARM_B_HOUR		= 0xc0,
      REG_ALARM_B_DOW		= 0xd0,
      REG_CTRL1			= 0xe0,
      REG_CTRL2			= 0xf0
    } BU9873_REG_T;

    // CTRL1 config bits
    typedef enum {
      CTRL1_AALE		= 0x80,
      CTRL1_BALE		= 0x40
    } BU9873_CTRL1_BIT_T;

    // CTRL2 config/status bits
    typedef enum {
      CTRL2_12_24		= 0x20,
      CTRL2_ADJ_XSTP		= 0x10,
      CTRL2_CLENB		= 0x08,
      CTRL2_CTFG		= 0x04,
      CTRL2_AAFG		= 0x02,
      CTRL2_BAFG		= 0x01
    } BU9873_CTRL2_BIT_T;

    /**
     * read a register
     *
     * @param reg the register to read
     * @return the value of the register
     */
    uint8_t readReg(uint8_t reg);

    /**
     * read contiguous refister into a buffer
     *
     * @param reg the register to start reading at
     * @param buffer the buffer to store the results
     * @param len the number of registers to read
     */
    void readRegs(uint8_t reg, uint8_t *buffer, int len);

    /**
     * write to a register
     *
     * @param reg the register to write to
     * @param val the value to write
     * @return true if successful, false otherwise
     */
    bool writeReg(uint8_t reg, uint8_t val);

    /**
     * Writes value(s) into registers
     *
     * @param reg Register location to start writing into
     * @param buffer Buffer for data storage
     * @param len Number of bytes to write
     * @return true if successful, false otherwise
     */
    bool writeRegs(uint8_t reg, uint8_t *buffer, int len);

    /**
     * Converts a BCD value into decimal
     *
     * @param val BCD value to convert
     * @return Converted decimal value 
     */
    unsigned int bcdToDec(uint8_t val);

    /**
     * Converts a decimal value into BCD
     *
     * @param val Decimal value to convert
     * @return Converted BCD value
     */
    uint8_t decToBcd(unsigned int val);

  private:
    mraa::I2c m_i2c;
    uint8_t m_addr;
  };
}
