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

#define KX0221020_I2C_BUS 0
#define KX0221020_DEFAULT_I2C_ADDR 0x1E

namespace upm {
  
  /**
   * @library kx0221020
   * @sensor kx0221020
   * @comname KX022-1020 3-axis Accelerometer
   * @type accelerometer
   *
   * @man kionix rohm
   * @con i2c gpio
   *
   * @snippet kx0221020.cxx Interesting
   */
  class KX0221020 {
  public:

    // NOTE: KX022 has not only simple accelerometer function but also
    // various features like tilt, directional tap and orientation sensing.
    // Currently, this driver handles basic accelerometer functions only.

    // KX022-1020 I2C registers
    typedef enum {
      REG_XHP_L			= 0x00,
      REG_XHP_H			= 0x01,
      REG_YHP_L			= 0x02,
      REG_YHP_H			= 0x03,
      REG_ZHP_L			= 0x04,
      REG_ZHP_H			= 0x05,
      REG_XOUT_L		= 0x06,
      REG_XOUT_H		= 0x07,
      REG_YOUT_L		= 0x08,
      REG_YOUT_H		= 0x09,
      REG_ZOUT_L		= 0x0a,
      REG_ZOUT_H		= 0x0b,
      REG_COTR			= 0x0c,

      REG_WHO_AM_I		= 0x0f,
      REG_TSCP			= 0x10,
      REG_TSPP			= 0x11,
      REG_INS1			= 0x12,
      REG_INS2			= 0x13,
      REG_INS3			= 0x14,
      REG_STATUS_REG		= 0x15,

      REG_INT_REL		= 0x17,
      REG_CNTL1			= 0x18,
      REG_CNTL2			= 0x19,
      REG_CNTL3			= 0x1a,
      REG_ODCNTL		= 0x1b,
      REG_INC1			= 0x1c,
      REG_INC2			= 0x1d,
      REG_INC3			= 0x1e,
      REG_INC4			= 0x1f,
      REG_INC5			= 0x20,
      REG_INC6			= 0x21,
      REG_TILT_TIMER		= 0x22,
      REG_WUFC			= 0x23,
      REG_TDTRC			= 0x24,
      REG_TDTC			= 0x25,
      REG_TTH			= 0x26,
      REG_TTL			= 0x27,
      REG_FTD			= 0x28,
      REG_STD			= 0x29,
      REG_TLT			= 0x2a,
      REG_TWS			= 0x2b,

      REG_ATH			= 0x30,

      REG_TILT_ANGLE_LL		= 0x32,
      REG_TILT_ANGLE_HL		= 0x33,
      REG_HYST_SET		= 0x34,
      REG_LP_CNTL		= 0x35,

      REG_BUF_CNTL1		= 0x3a,
      REG_BUF_CNTL2		= 0x3b,
      REG_BUF_STATUS_1		= 0x3c,
      REG_BUF_STATUS_2		= 0x3d,
      REG_BUF_CLEAR		= 0x3e,
      REG_BUF_READ		= 0x3f,

      REG_SELF_TEST		= 0x60
    } KX0221020_REG_T;

    // REG_WHO_AM_I value
    static const int WHO_AM_I_VAL = 0x14;

    // INS2 status bits
    typedef enum {
      INS2_BFI			= 0x40,
      INS2_WMI			= 0x20,
      INS2_DRDY			= 0x10,
      INS2_TDTS1		= 0x08,
      INS2_TDTS2		= 0x04,
      INS2_WUFS			= 0x02,
      INS2_TPS			= 0x01
    } KX0221020_INS2_BIT_T;

    // CNTL1 config bits
    // PC1 enables KX022 operation and needs 1.2/ODR delay time for
    // stand-by/operating mode change.  
    typedef enum {
      CNTL1_PC1			= 0x80,
      CNTL1_RES			= 0x40,
      CNTL1_DRDYE		= 0x20,
      CNTL1_GSEL1		= 0x10,
      CNTL1_GSEL0		= 0x08,
      CNTL1_TDTE		= 0x04,
      CNTL1_WUFE		= 0x02,
      CNTL1_TPE			= 0x01
    } KX0221020_CNTL1_BIT_T;

    // CNTL2 config bits
    typedef enum {
      CNTL2_SRST		= 0x80,
      CNTL2_COTC		= 0x40,
      CNTL2_TLEM		= 0x20,
      CNTL2_TRIM		= 0x10,
      CNTL2_TDOM		= 0x08,
      CNTL2_TUPM		= 0x04,
      CNTL2_TFDM		= 0x02,
      CNTL2_TFUM		= 0x01
    } KX0221020_CNTL2_BIT_T;

    // CNTL3 config bits
    typedef enum {
      CNTL3_OTP1		= 0x80,
      CNTL3_OTP0		= 0x40,
      CNTL3_OTDT2		= 0x20,
      CNTL3_OTDT1		= 0x10,
      CNTL3_OTDT0		= 0x08,
      CNTL3_OWUF2		= 0x04,
      CNTL3_OWUF1		= 0x02,
      CNTL3_OWUF0		= 0x01
    } KX0221020_CNTL3_BIT_T;

    // ODCNTL config bits
    typedef enum {
      ODCNTL_IIR_BYPASS		= 0x80,
      ODCNTL_LPRO		= 0x40,

      ODCNTL_OSA3		= 0x08,
      ODCNTL_OSA2		= 0x04,
      ODCNTL_OSA1		= 0x02,
      ODCNTL_OSA0		= 0x01
    } KX0221020_ODCNTL_BIT_T;

    // INC1 config bits
    typedef enum {
      INC1_IEN1			= 0x20,
      INC1_IEA1			= 0x10,
      INC1_IEL1			= 0x08,

      INC1_STPOL		= 0x02,
      INC1_SPI3E		= 0x01
    } KX0221020_INC1_BIT_T;

    // INC2 config bits
    typedef enum {
      INC2_XNWUE		= 0x20,
      INC2_XPWUE		= 0x10,
      INC2_YNWUE		= 0x08,
      INC2_YPWUE		= 0x04,
      INC2_ZNWUE		= 0x02,
      INC2_ZPWUE		= 0x01
    } KX0221020_INC2_BIT_T;

    // INC3 config bits
    typedef enum {
      INC3_TLEM			= 0x20,
      INC3_TRIM			= 0x10,
      INC3_TDOM			= 0x08,
      INC3_TUPM			= 0x04,
      INC3_TFDM			= 0x02,
      INC3_TFUM			= 0x01
    } KX0221020_INC3_BIT_T;

    // INC4 config bits
    typedef enum {
      INC4_BFI1			= 0x40,
      INC4_WMI1			= 0x20,
      INC4_DRDYI1		= 0x10,

      INC4_TDTI1		= 0x04,
      INC4_WUFI1		= 0x02,
      INC4_TPI1			= 0x01
    } KX0221020_INC4_BIT_T;

    // INC5 config bits
    typedef enum {
      INC5_IEN2			= 0x20,
      INC5_IEA2			= 0x10,
      INC5_IEL2			= 0x08
    } KX0221020_INC5_BIT_T;

    // INC6 config bits
    typedef enum {
      INC6_BFI2			= 0x40,
      INC6_WMI2			= 0x20,
      INC6_DRDYI2		= 0x10,

      INC6_TDTI2		= 0x04,
      INC6_WUFI2		= 0x02,
      INC6_TPI2			= 0x01
    } KX0221020_INC6_BIT_T;

    // TDTRC config bits
    typedef enum {
      TDTRC_DTRE		= 0x02,
      TDTRC_STRE		= 0x01
    } KX0221020_TDTRC_BIT_T;

    // LP_CNTL config bits
    typedef enum {
      LP_CNTL_AVC2		= 0x40,
      LP_CNTL_AVC1		= 0x20,
      LP_CNTL_AVC0		= 0x10
    } KX0221020_LP_CNTL_BIT_T;

    // BUF_CNTL2 config bits
    typedef enum {
      BUF_CNTL2_BFE		= 0x80,
      BUF_CNTL2_BRES		= 0x40,
      BUF_CNTL2_BFIE		= 0x20,
      BUF_CNTL2_BM1		= 0x02,
      BUF_CNTL2_BM0		= 0x01
    } KX0221020_BUF_CNTL2_BIT_T;

    // BUF_STATUS_2 status bits
    typedef enum {
      BUF_STATUS_2_BUF_TRIG	= 0x80
    } KX0221020_BUF_STATUS_2_BIT_T;

    // Range type
    typedef enum {
      RANGE_2G,
      RANGE_4G,
      RANGE_8G
    } KX0221020_RANGE_T;

    // Output data rate type
    typedef enum {
      RATE_12_5HZ		= 0,
      RATE_25HZ			= 1,
      RATE_50HZ			= 2,
      RATE_100HZ		= 3,
      RATE_200HZ		= 4,
      RATE_400HZ		= 5,
      RATE_800HZ		= 6,
      RATE_1600HZ		= 7,
      RATE_0_781HZ		= 8,
      RATE_1_563HZ		= 9,
      RATE_3_125HZ		= 10,
      RATE_6_25HZ		= 11
    } KX0221020_RATE_T;

    /**
     * kx0221020 constructor
     *
     * @param bus i2c bus to use
     * @param address the address for this device
     */
    KX0221020(int bus=KX0221020_I2C_BUS,
	      uint8_t address=KX0221020_DEFAULT_I2C_ADDR);

    /**
     * kx0221020 Destructor
     */
    virtual ~KX0221020();

    /**
     * read a register
     *
     * @param reg the register to read
     * @return the value of the register
     */
    uint8_t readReg(uint8_t reg);

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
     * Set range
     *
     * @param range
     */
    bool setRange(KX0221020_RANGE_T range=RANGE_2G);

    /**
     * Set out put data rate
     *
     * @param rate
     */
    bool setRate(KX0221020_RATE_T rate=RATE_50HZ);

    /**
     * get interrupt status
     *
     * @param buffer full
     * @param data ready
     * @param water mark exceeded
     * @return true if successful, false otherwise
     */
    bool getInterruptStatus(bool& full, bool& rdy, bool& watermark);

    /**
     * clear interrupt
     *
     * @return true if successful, false otherwise
     */
    bool clearInterrupt();

    /**
     * get interrupt status
     *
     * @return # of acceleration data in FIFO
     */
    int getCount();

    /**
     * get the accelerometer values
     *
     * @param x the returned x value, if arg is non-NULL
     * @param y the returned y value, if arg is non-NULL
     * @param z the returned z value, if arg is non-NULL
     * @return true if successful, false otherwise
     */
    bool getAccelerometer(float *x, float *y, float *z);

#if defined(SWIGJAVA) || defined(JAVACALLBACK)
    /**
     * get the accelerometer values
     *
     * @return Array containing X, Y, Z accelerometer values
     */
    float *getAccelerometer();
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
    // accelerometer values
    float m_accelX;
    float m_accelY;
    float m_accelZ;
    // accelerometer scaling factor
    float m_accelScale;
    // output data rate
    float m_rate;

  private:
    mraa::I2c m_i2c;
    uint8_t m_addr;
    bool m_sleep;

    mraa::Gpio *m_gpioIRQ;
  };
}
