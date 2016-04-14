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

#define BH1745NUC_I2C_BUS 0
#define BH1745NUC_DEFAULT_I2C_ADDR 0x39

namespace upm {
  
  /**
   * @library bh1745nuc
   * @sensor bh1745nuc
   * @comname BH1745NUC ColorOprical sensor
   * @type light
   *
   * @man rohm
   * @con i2c gpio
   *
   * @snippet bh1745nuc.cxx Interesting
   */
  class BH1745NUC {
  public:

    // BH1745NUC I2C registers
    typedef enum {
      REG_SWRST			= 0x40,
      REG_MODE1			= 0x41,
      REG_MODE2			= 0x42,
      REG_MODE3			= 0x44,
      REG_RED_L			= 0x50,
      REG_RED_H			= 0x51,
      REG_GREEN_L		= 0x52,
      REG_GREEN_H		= 0x53,
      REG_BLUE_L		= 0x54,
      REG_BLUE_H		= 0x55,
      REG_CLEAR_L		= 0x56,
      REG_CLEAR_H		= 0x57,
      REG_DINT_L		= 0x58,
      REG_DINT_H		= 0x59,
      REG_INT			= 0x60,
      REG_PERSISTENCE		= 0x61,
      REG_TH_L			= 0x62,
      REG_TH_H			= 0x63,
      REG_TL_L			= 0x64,
      REG_TL_H			= 0x65,
      REG_ID			= 0x92
    } BH1745NUC_REG_T;

    // REG_ID value
    static const int ID_VAL	= 0xe0;

    // SWRST config bits
    typedef enum {
      SWRST_SYS			= 0x80,
      SWRST_INT			= 0x40
    } BH1745NUC_SWRST_BIT_T;

    static const uint8_t SWRST_PID = 0x0b;

    // MODE2 config bits
    typedef enum {
      MODE2_VALID		= 0x80,
      MODE2_RGBC_EN		= 0x10
    } BH1745NUC_MODE2_BIT_T;

    // MODE3 config value
    static const uint8_t MODE3_VALUE = 0x02;

    // INT config/status bits
    typedef enum {
      INT_STATUS		= 0x80,
      INT_LATCH			= 0x10,
      INT_EN			= 0x01
    } BH1745NUC_INT_BIT_T;

    static const int INT_SOURCE_SHIFT = 2;

    // Gain type
    typedef enum {
      GAIN_1		= 0,
      GAIN_2		= 1,
      GAIN_16		= 2
    } BH1745NUC_GAIN_T;

    // Interrupt persistence type
    typedef enum {
      EOM			= 0,
      UPDATE_EOM		= 1,
      UPDATE_4TIMES		= 2,
      UPDATE_8TIMES		= 3
    } BH1745NUC_PERSISTENCE_T;

    // Interrupt source type
    typedef enum {
      INT_RED			= 0,
      INT_GREEN			= 1,
      INT_BLUE			= 2,
      INT_CLEAR			= 3
    } BH1745NUC_INT_SOURCE_T;

    // Rate type
    typedef enum {
      RATE_160MS		= 0,
      RATE_320MS		= 1,
      RATE_640MS		= 2,
      RATE_1280MS		= 3,
      RATE_2560MS		= 4,
      RATE_5120MS		= 5
    } BH1745NUC_RATE_T;

    /**
     * bh1745nuc constructor
     *
     * @param bus i2c bus to use
     * @param address the address for this device
     */
    BH1745NUC(int bus=BH1745NUC_I2C_BUS,
	      uint8_t address=BH1745NUC_DEFAULT_I2C_ADDR);

    /**
     * bh1745nuc Destructor
     */
    virtual ~BH1745NUC();

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
     * Set messurament mode
     *
     * @param rate messurament rate
     * @return true if successful, false otherwise
     */
    bool setMode(BH1745NUC_RATE_T rate=RATE_160MS);

    /**
     * Set AGC gain
     *
     * @param gain AGC gain
     * @return true if successful, false otherwise
     */
    bool setGain(BH1745NUC_GAIN_T gain=GAIN_1);

    /**
     * Set watermarks
     * 
     * @param high high watermark
     * @param low low watermark
     * @return true if successful, false otherwise
     */
    bool setWatermark(uint16_t high, uint16_t low);

    /**
     * Set interrupt mode
     *
     * @param enable SOURCE color data triggers interrupt
     * @param source which color will trigger
     * @param persist interrupt persisience
     * @return true if successful, false otherwise
     */
    bool setInterruptMode(bool enable,
			  BH1745NUC_INT_SOURCE_T source=INT_CLEAR,
			  BH1745NUC_PERSISTENCE_T persist=EOM);

    /**
     * get interrupt status
     *
     * @param status interrupt status flag
     * @return true if successful, false otherwise
     */
    bool getInterruptStatus(bool& status);

    /**
     * Clear interrupt
     *
     * @return true if successful, false otherwise
     */
    bool clearInterrupt();

    /**
     * get the color data values
     *
     * @param r the returned the red value, if arg is non-NULL
     * @param g the returned the green value, if arg is non-NULL
     * @param b the returned the blue value, if arg is non-NULL
     * @param c the returned the clear value, if arg is non-NULL
     * @return true if successful, false otherwise
     */
    bool getColorData(uint16_t *r, uint16_t *g, uint16_t *b, uint16_t *c);

#if defined(SWIGJAVA) || defined(JAVACALLBACK)
    /**
     * get the color data values
     *
     * @return Array containing the color data values
     */
    short *getColorData();
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
     * default no-op interrupt handler.
     *
     */
    static void defaultISR(void *) {};

  protected:
    // color data values
    uint16_t m_red;
    uint16_t m_green;
    uint16_t m_blue;
    uint16_t m_clear;

    // modes
    BH1745NUC_RATE_T m_rate;
    BH1745NUC_GAIN_T m_gain;

    // Watermarks
    uint16_t m_high;
    uint16_t m_low;
 
    // Interrupt mode
    bool m_int_enable;
    BH1745NUC_INT_SOURCE_T m_int_source;
    BH1745NUC_PERSISTENCE_T m_int_persistence;

  private:
    mraa::I2c m_i2c;
    uint8_t m_addr;

    mraa::Gpio *m_gpioIRQ;
  };
}
