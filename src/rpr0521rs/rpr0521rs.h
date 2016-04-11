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

#define RPR0521RS_I2C_BUS 0
#define RPR0521RS_DEFAULT_I2C_ADDR 0x38

namespace upm {
  
  /**
   * @library rpr0521rs
   * @sensor rpr0521rs
   * @comname RPR-0521RS Oprical sensor
   * @type light
   *
   * @man rohm
   * @con i2c gpio
   *
   * @snippet rpr0521rs.cxx Interesting
   */
  class RPR0521RS {
  public:

    // RPR0521RS I2C registers
    typedef enum {
      REG_SWRST			= 0x40,
      REG_MODE			= 0x41,
      REG_ALS_CNTL		= 0x42,
      REG_PS_CNTL		= 0x43,
      REG_PS_DADA_L		= 0x44,
      REG_PS_DATA_H		= 0x45,
      REG_ALS_DADA0_L		= 0x46,
      REG_ALS_DATA0_H		= 0x47,
      REG_ALS_DADA1_L		= 0x48,
      REG_ALS_DATA1_H		= 0x49,
      REG_INT			= 0x4a,
      REG_PS_TH_L		= 0x4b,
      REG_PS_TH_H		= 0x4c,
      REG_PS_TL_L		= 0x4d,
      REG_PS_TL_H		= 0x4e,
      REG_ALS_TH_L		= 0x4f,
      REG_ALS_TH_H		= 0x50,
      REG_ALS_TL_L		= 0x51,
      REG_ALS_TL_H		= 0x52,
      REG_PS_OFFS_L		= 0x44,
      REG_PS_OFFS_H		= 0x45,
      REG_ID			= 0x92
    } RPR0521RS_REG_T;

    // REG_ID value
    static const int ID_VAL	= 0xe0;

    // SWRST config bits
    typedef enum {
      SWRST_SYS			= 0x80,
      SWRST_INT			= 0x40
    } RPR0521RS_SWRST_BIT_T;

    static const uint8_t SWRST_PID = 0x0a;

    // MODE config bits
    typedef enum {
      MODE_ALS_EN		= 0x80,
      MODE_PS_EN		= 0x40,
      MODE_PS_PULSE		= 0x20,
      MODE_PS_TWICE		= 0x10
    } RPR0521RS_MODE_BIT_T;

    static const uint8_t MODE_RATE_MASK = 0x0f;
 
    // ALS_CNTL config
    static const int ALS_DATA0_GAIN_SHIFT = 4;
    static const int ALS_DATA1_GAIN_SHIFT = 2;

    // PS_CNTL config
    static const int AMBIENT_IR_SHIFT = 6;
    static const int PS_GAIN_SHIFT = 4;

    // INT config/status bits
    typedef enum {
      INT_PS_STATUS		= 0x80,
      INT_ALS_STATUS		= 0x40,
      INT_ASSERT		= 0x08,
      INT_LATCH			= 0x04,
      INT_ALS_EN		= 0x02,
      INT_PS_EN			= 0x01
    } RPR0521RS_INT_BIT_T;

    static const int INT_MODE_SHIFT = 4;

    // ALS gain type
    typedef enum {
      ALS_GAIN_1		= 0,
      ALS_GAIN_2		= 1,
      ALS_GAIN_64		= 2,
      ALS_GAIN_128		= 3
    } RPR0521RS_ALS_GAIN_T;

    // Current type
    typedef enum {
      LED_CURRENT_25MA		= 0,
      LED_CURRENT_50MA		= 1,
      LED_CURRENT_100MA		= 2,
      LED_CURRENT_200MA		= 3
    } RPR0521RS_LED_CURRENT_T;

    // Ambient IR type
    typedef enum {
      IR_WEAK			= 0,
      IR_STRONG			= 1,
      IR_VERY_STRONG		= 3
    } RPR0521RS_AMBIENT_IR_T;

    // PS gain type
    typedef enum {
      PS_GAIN_1			= 0,
      PS_GAIN_2			= 1,
      PS_GAIN_4			= 2
    } RPR0521RS_PS_GAIN_T;

    // Interrupt persistence type
    typedef enum {
      EOM			= 0,
      UPDATE_EOM		= 1,
      UPDATE_TWICE		= 2,
      UPDATE_PERSIST		= 3
    } RPR0521RS_PERSISTENCE_T;

    // Interrupt mode type
    typedef enum {
      INT_HIGH			= 0,
      INT_HYSTERESIS		= 1,
      INT_OUT_OF_RANGE		= 2
    } RPR0521RS_INT_MODE_T;

    // Rate type RATE_rateALS_ratePS
    typedef enum {
      RATE_NO_NO		= 0,
      RATE_NO_100HZ		= 1,
      RATE_NO_25HZ		= 2,
      RATE_NO_10HZ		= 3,
      RATE_NO_2_5HZ		= 4,
      RATE_10HZ_20HZ		= 5,
      RATE_10HZ_10HZ		= 6,
      RATE_10HZ_2_5HZ		= 7,
      RATE_2_5HZ_20HZ		= 8,
      RATE_2_5HZ_10HZ		= 9,
      RATE_2_5HZ_NO		= 10,
      RATE_2_5HZ_2_5HZ		= 11,
      RATE_20HZ_20HZ		= 12
    } RPR0521RS_RATE_T;

    /**
     * rpr0521rs constructor
     *
     * @param bus i2c bus to use
     * @param address the address for this device
     */
    RPR0521RS(int bus=RPR0521RS_I2C_BUS,
	      uint8_t address=RPR0521RS_DEFAULT_I2C_ADDR);

    /**
     * rpr0521rs Destructor
     */
    virtual ~RPR0521RS();

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
     * @param als enable ambient light sensor
     * @param ps enable proximity sensor
     * @param pswide select 330us pulse istead of 200us pulse for PS LED
     * @param pstwice messure PS twice
     * @param led PS led current
     * @param rate messurament rate
     * @return true if successful, false otherwise
     */
    bool setMode(bool als, bool ps, bool pswide, bool pstwice,
		 RPR0521RS_LED_CURRENT_T led=LED_CURRENT_25MA,
		 RPR0521RS_RATE_T rate=RATE_10HZ_10HZ);

    /**
     * Set ambient light gain
     *
     * @param gain0 gain for visible + infrared
     * @param gain1 gain for infrared
     * @return true if successful, false otherwise
     */
    bool setAmbientLightGain(RPR0521RS_ALS_GAIN_T gain0=ALS_GAIN_1,
			     RPR0521RS_ALS_GAIN_T gain1=ALS_GAIN_1);

    /**
     * Set proximity
     *
     * @param gain gain for proximity sensor
     * @return true if successful, false otherwise
     */
    bool setProximityGain(RPR0521RS_PS_GAIN_T gain=PS_GAIN_1);

    /**
     * Set ALS watermarks
     * 
     * @param high high watermark
     * @param low low watermark
     * @return true if successful, false otherwise
     */
    bool setAmbientLightWatermark(uint16_t high, uint16_t low);

    /**
     * Set PS watermarks
     * 
     * @param high high watermark
     * @param low low watermark
     * @return true if successful, false otherwise
     */
    bool setProximityWatermark(uint16_t high, uint16_t low);

    /**
     * Set PS offset
     * 
     * @param offs PS offset value
     * @return true if successful, false otherwise
     */
    bool setProximityOffset(uint16_t offs);

    /**
     * Set interrupt mode
     *
     * @param ps PS triggers interrupt
     * @param als ALS triggers interrupt
     * @param mode interrupt mode
     * @param persist interrupt persisience
     * @return true if successful, false otherwise
     */
    bool setInterruptMode(bool ps, bool als,
			  RPR0521RS_INT_MODE_T mode=INT_HIGH,
			  RPR0521RS_PERSISTENCE_T persist=EOM);

    /**
     * get interrupt status
     *
     * @param ps PS interrupt
     * @param als ALS interrupt
     * @return true if successful, false otherwise
     */
    bool getInterruptStatus(bool& ps, bool& als);

    /**
     * Clear interrupt
     *
     * @return true if successful, false otherwise
     */
    bool clearInterrupt();

    /**
     * get the ambient light
     *
     * @param x the returned the value, if arg is non-NULL
     * @return true if successful, false otherwise
     */
    bool getAmbientLight(uint16_t *x);

#if defined(SWIGJAVA) || defined(JAVACALLBACK)
    /**
     * get the ambient light values
     *
     * @return Memory containing the ambient light value
     */
    short *getAmbientLight();
#endif

    /**
     * get the ambient light Ir only
     *
     * @param x the returned the value, if arg is non-NULL
     * @return true if successful, false otherwise
     */
    bool getAmbientLightIr(uint16_t *x);

#if defined(SWIGJAVA) || defined(JAVACALLBACK)
    /**
     * get the ambient light Ir value
     *
     * @return Memory containing the ambient light Ir value
     */
    short *getAmbientLightIr();
#endif

    /**
     * get the proximity
     *
     * @param x the returned the value, if arg is non-NULL
     * @return true if successful, false otherwise
     */
    bool getProximity(uint16_t *x);

#if defined(SWIGJAVA) || defined(JAVACALLBACK)
    /**
     * get the Proximity value
     *
     * @return Memory containing the proximity value
     */
    short *getProximity();
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

  protected:
    // optical values
    uint16_t m_als_data0;
    uint16_t m_als_data1;
    uint16_t m_ps_data;

    // modes
    uint8_t m_mode;
    RPR0521RS_ALS_GAIN_T m_als_data0_gain;
    RPR0521RS_ALS_GAIN_T m_als_data1_gain;
    RPR0521RS_PS_GAIN_T m_ps_gain;
    RPR0521RS_LED_CURRENT_T m_led_current;

    // Watermarks
    uint16_t m_als_high;
    uint16_t m_als_low;
    uint16_t m_ps_high;
    uint16_t m_ps_low;

    // offset
    uint16_t m_ps_offset;
 
    // Interrupt mode
    bool m_int_als;
    bool m_int_ps;
    RPR0521RS_INT_MODE_T m_int_mode;
    RPR0521RS_PERSISTENCE_T m_int_persistence;

  private:
    mraa::I2c m_i2c;
    uint8_t m_addr;

    mraa::Gpio *m_gpioIRQ;
  };
}
