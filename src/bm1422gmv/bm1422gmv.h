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

#define BM1422GMV_I2C_BUS 0
#define BM1422GMV_DEFAULT_I2C_ADDR 0x0E

namespace upm {
  
  /**
   * @library bm1422gmv
   * @sensor bm1422gmv
   * @comname BM1422GMV 3-axis Magnetometer
   * @type compass
   *
   * @man rohm
   * @con i2c gpio
   *
   * @snippet bm1422gmv.cxx Interesting
   */
  class BM1422GMV {
  public:

    // BM1422GMV I2C registers
    typedef enum {
      REG_INFO_L		= 0x0d,
      REG_INFO_H		= 0x0e,
      REG_WHO_AM_I		= 0x0f,

      REG_DATAX_L		= 0x10,
      REG_DATAX_H		= 0x11,
      REG_DATAY_L		= 0x12,
      REG_DAYAY_H		= 0x13,
      REG_DATAZ_L		= 0x14,
      REG_DATAZ_H		= 0x15,

      REG_STA1			= 0x18,
      REG_CNTL1			= 0x1b,
      REG_CNTL2			= 0x1c,
      REG_CNTL3			= 0x1d,
      REG_PRET			= 0x30,
      REG_AVER			= 0x40,
      REG_CNTL4			= 0x5c,

      REG_TEMP_L		= 0x60,
      REG_TEMP_H		= 0x61,

      REG_OFFX_L		= 0x6c,
      REG_OFFX_H		= 0x6d,
      REG_OFFY_L		= 0x72,
      REG_OFFY_H		= 0x73,
      REG_OFFZ_L		= 0x78,
      REG_OFFZ_H		= 0x79,
      REG_FINEX_L		= 0x90,
      REG_FINEX_H		= 0x91,
      REG_FINEY_L		= 0x92,
      REG_FINEY_H		= 0x93,
      REG_FINEZ_L		= 0x94,
      REG_FINEZ_H		= 0x95,
      REG_SENSX_L		= 0x96,
      REG_SENSX_H		= 0x97,
      REG_SENSY_L		= 0x98,
      REG_SENSY_H		= 0x99,
      REG_SENSZ_L		= 0x9a,
      REG_SENSZ_H		= 0x9b,
      REG_PARAX_L		= 0x9c,
      REG_PARAX_H		= 0x9d,
      REG_PARAY_L		= 0x9e,
      REG_PARAY_H		= 0x9f,
      REG_PARAZ_L		= 0xa0,
      REG_PARAZ_H		= 0xa1,
      REG_ZEROX_L		= 0xf8,
      REG_ZEROX_H		= 0xf9,
      REG_ZEROY_L		= 0xfa,
      REG_ZEROY_H		= 0xfb,
      REG_ZEROZ_L		= 0xfb,
      REG_ZEROZ_H		= 0xfd
    } BM1422GMV_REG_T;

    // REG_WHO_AM_I value
    static const int WHO_AM_I_VAL = 0x41;

    // STA1 status bits
    typedef enum {
      STA1_DRDY			= 0x40
    } BM1422GMV_STA1_BIT_T;

    // CNTL1 config bits
    typedef enum {
      CNTL1_PC1			= 0x80,
      CNTL1_OUT_BIT		= 0x40,
      CNTL1_RST_LV		= 0x20,
      CNTL1_ODR1		= 0x10,
      CNTL1_ODR0		= 0x08,

      CNTL1_FS1			= 0x02
    } BM1422GMV_CNTL1_BIT_T;

    // CNTL2 config bits
    typedef enum {
      CNTL2_DREN		= 0x08,
      CNTL2_DRP			= 0x04
    } BM1422GMV_CNTL2_BIT_T;

    // CNTL3 config bits
    typedef enum {
      CNTL3_FORCE		= 0x40
    } BM1422GMV_CNTL3_BIT_T;

    // Conversion mode type
    typedef enum {
      MODE_CONTINUOUS,
      MODE_SINGLE
    } BM1422GMV_MODE_T;

    // Precision type
    typedef enum {
      PREC_12BIT,
      PREC_14BIT
    } BM1422GMV_PREC_T;

    // Precision type
    typedef enum {
      RATE_10HZ,
      RATE_20HZ,
      RATE_100HZ,
      RATE_1000HZ
    } BM1422GMV_RATE_T;

    // Avarage of # of samples
    typedef enum {
      AVER_4			= 0,
      AVER_1			= 4,
      AVER_2			= 8,
      AVER_8			= 12,
      AVER_16			= 16
    } BM1422GMV_AVER_T;

    /**
     * bm1422gmv constructor
     *
     * @param bus i2c bus to use
     * @param address the address for this device
     */
    BM1422GMV(int bus=BM1422GMV_I2C_BUS,
	      uint8_t address=BM1422GMV_DEFAULT_I2C_ADDR);

    /**
     * bm1422gmv Destructor
     */
    virtual ~BM1422GMV();

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
     * @return the value of the register
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
     * Write a word to a register
     *
     * @param reg the register to write to
     * @param valu the value to write
     * @return true if successful, false otherwise
     */
    bool writeWordReg(uint8_t reg, uint16_t val);

    /**
     * set up initial values and start operation
     *
     * @return true if successful
     */
    bool init();

    /**
     * enable or disable device sleep
     *
     * @param enable true to put device to sleep, false to wake up
     * @return true if successful, false otherwise
     */
    bool setSleep(bool enable);

    /**
     * Set conversion mode - continuos/single, precision and data rate
     *
     * @param mode
     * @param prec
     * @param rate
     */
    bool setMode(BM1422GMV_MODE_T mode=MODE_CONTINUOUS,
		 BM1422GMV_PREC_T prec=PREC_14BIT,
		 BM1422GMV_RATE_T rate=RATE_10HZ);

    /**
     * Set average filter
     *
     * @param aver
     */
    bool setAverageFilter(BM1422GMV_AVER_T aver=AVER_4);

    /**
     * get status
     *
     * @param drdy data ready flag
     * @return true if successful, false otherwise
     */
    bool getStatus(bool& drdy);

    /**
     * get the magnetometer values
     *  NOTE: Data ready flag will be cleared with this
     *
     * @param x the returned x value, if arg is non-NULL
     * @param y the returned y value, if arg is non-NULL
     * @param z the returned z value, if arg is non-NULL
     * @return true if successful, false otherwise
     */
    bool getMagnetometer(float *x, float *y, float *z);

#if defined(SWIGJAVA) || defined(JAVACALLBACK)
    /**
     * get the magnetometer values
     *
     * @return Array containing X, Y, Z magnetometer values
     */
    float *getMagnetometer();
#endif

    /**
     * install an interrupt handler.
     *
     * @param gpio gpio pin to use as interrupt pin
     * @param isr the interrupt handler, accepting a void * argument
     * @param arg the argument to pass the the interrupt handler
     */
#if defined(SWIGJAVA) || defined(JAVACALLBACK)
    void installISR(int gpio, jobject runnable);
#else
    void installISR(int gpio, void (*isr)(void *), void *arg);
#endif

    /**
     * uninstall a previously installed interrupt handler
     *
     */
    void uninstallISR();

    /**
     * default no-op interrupt handler
     *
     */
    static void defaultISR(void *) {};

  protected:
    // magnetometer values
    float m_dataX;
    float m_dataY;
    float m_dataZ;

    // offset values
    uint16_t m_offsetX;
    uint16_t m_offsetY;
    uint16_t m_offsetZ;

    // modes
    BM1422GMV_MODE_T m_mode;
    BM1422GMV_PREC_T m_prec;
    BM1422GMV_RATE_T m_rate;
    BM1422GMV_AVER_T m_aver;

    // offset adjustment done
    bool m_offset_adjust;

  private:
    mraa::I2c m_i2c;
    uint8_t m_addr;

    mraa::Gpio *m_gpioIRQ;
  };
}
