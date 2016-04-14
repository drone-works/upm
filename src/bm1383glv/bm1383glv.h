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

#define BM1383GLV_I2C_BUS 0
#define BM1383GLV_DEFAULT_I2C_ADDR 0x5d

namespace upm {
  
  /**
   * @library bm1383glv
   * @sensor bm1383glv
   * @comname BM1383GLV Barometer
   * @type pressure
   *
   * @man rohm
   * @con i2c gpio
   *
   * @snippet bm1383glv.cxx Interesting
   */
  class BM1383GLV {
  public:

    // BM1383GLV I2C registers
    typedef enum {
      REG_ID			= 0x10,
      REG_SWRST			= 0x11,
      REG_PWR			= 0x12,
      REG_RST			= 0x13,
      REG_MODE			= 0x14,
      REG_PDTH_H_H		= 0x15,
      REG_PDTH_H_L		= 0x16,
      REG_PDTH_L_H		= 0x17,
      REG_PDTH_L_L		= 0x18,
      REG_INT			= 0x19,
      REG_PRESS_H		= 0x1c,
      REG_PRESS_L		= 0x1d,
      REG_PRESS_XL		= 0x1e
    } BM1383GLV_REG_T;

    // REG_ID value
    static const int ID_VAL	= 0x31;

    // SWRST config bits
    typedef enum {
      SWRST_SYS			= 0x80,
      SWRST_INT			= 0x40
    } BM1383GLV_SWRST_BIT_T;

    // PWR config bits
    typedef enum {
      PWR_ON			= 0x01
    } BM1383GLV_PWR_BIT_T;

    // RSTB config bits
    typedef enum {
      RST_RSTB			= 0x01
    } BM1383GLV_RST_BIT_T;

    // MODE config bits
    typedef enum {
      T_AVE			= 0x08
    } BM1383GLV_MODE_BIT_T;

    // INT config/status bits
    typedef enum {
      INT_H_STATUS		= 0x80,
      INT_L_STATUS		= 0x40,
      INT_H_EN			= 0x20,
      INT_L_EN			= 0x10,
      INT_PU_EN			= 0x08,
      INT_MODE			= 0x02,
      INT_EN			= 0x01
    } BM1383GLV_INT_BIT_T;

    // Avarage of # of samples
    typedef enum {
      AVER_NO			= 0,
      AVER_2			= 1,
      AVER_4			= 2,
      AVER_8			= 3,
      AVER_16			= 4,
      AVER_32			= 5,
      AVER_64			= 6
    } BM1383GLV_AVER_T;

    static const int AVER_SHIFT = 5;

    // Mode type
    typedef enum {
      MODE_NO			= 0,
      MODE_ONESHOT		= 1,
      MODE_RATE_20HZ		= 2,
      MODE_RATE_10HZ		= 3,
      MODE_RATE_5HZ		= 4
    } BM1383GLV_MODE_T;

    /**
     * bm1383glv constructor
     *
     * @param bus i2c bus to use
     * @param address the address for this device
     */
    BM1383GLV(int bus=BM1383GLV_I2C_BUS,
	      uint8_t address=BM1383GLV_DEFAULT_I2C_ADDR);

    /**
     * bm1383glv Destructor
     */
    virtual ~BM1383GLV();

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
     * Set conversion mode - continuos/single and data rate
     *
     * @param mode
     * @return true if successful, false otherwise
     */
    bool setMode(BM1383GLV_MODE_T mode=MODE_RATE_10HZ);

    /**
     * Set average filter
     *
     * @param aver
     */
    bool setAverageFilter(BM1383GLV_AVER_T aver=AVER_NO, bool taver=false);

    /**
     * Set watermarks
     * 
     * NOTE: With setting high watermark to 0, high watermark interrupt
     * can report the end of measurement.
     *
     * @param high high watermark
     * @param low low watermark
     * @return true if successful, false otherwise
     */
    bool setWatermark(float high, float low);

    /**
     * Set interrupt mode
     *
     * @param high enable high watermark interrupt
     * @param low enable low watermark interrupt
     * @param pullup enable internal pull-up register on int pin
     * @return true if successful, false otherwise
     */
    bool setInterruptMode(bool high, bool low, bool pullup);

    /**
     * get interrupt status
     *
     * @param high water mark exceeded
     * @param low water mark exceeded
     * @return true if successful, false otherwise
     */
    bool getInterruptStatus(bool& high, bool& low);

    /**
     * Clear interrupt
     *
     * @return true if successful, false otherwise
     */
    bool clearInterrupt();

    /**
     * get the pressure value
     *
     * @param x the returned the value, if arg is non-NULL
     * @return true if successful, false otherwise
     */
    bool getPressure(float *x);

#if defined(SWIGJAVA) || defined(JAVACALLBACK)
    /**
     * get the pressure value
     *
     * @return Memory containing the pressure value
     */
    float *getPressure();
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
    // pressure values
    float m_press;

    // modes
    BM1383GLV_MODE_T m_mode;
    BM1383GLV_AVER_T m_aver;
    bool m_taver;

    // Watermarks
    uint16_t m_high;
    uint16_t m_low;

    // Interrupt configurarion
    bool m_enable_high;
    bool m_enable_low;
    bool m_pullup;

  private:
    mraa::I2c m_i2c;
    uint8_t m_addr;

    mraa::Gpio *m_gpioIRQ;
  };
}
