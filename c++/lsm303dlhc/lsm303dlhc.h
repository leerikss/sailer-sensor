#ifndef lsm303dlhc_h
#define lsm303dlhc_h

#include"I2C_Bus.h"
#include <libconfig.h++>

using namespace libconfig;

// register addresses map

/*******ACCELEORMETR***************/
#define LSM303_CTRL_REG1       0x20
#define LSM303_CTRL_REG2       0x21
#define LSM303_CTRL_REG3       0x22
#define LSM303_CTRL_REG4       0x23
#define LSM303_CTRL_REG5       0x24
#define LSM303_CTRL_REG6       0x25
#define LSM303_REFERENCE       0x26
#define LSM303_STATUS_REG      0x27

#define LSM303_OUT_X_L_A       0x28
#define LSM303_OUT_X_H_A       0x29
#define LSM303_OUT_Y_L_A       0x2A
#define LSM303_OUT_Y_H_A       0x2B
#define LSM303_OUT_Z_L_A       0x2C
#define LSM303_OUT_Z_H_A       0x2D

#define LSM303_FIFO_CTRL_REG   0x2E
#define LSM303_FIFO_SRC_REG    0x2F

#define LSM303_INT1_CFG        0x30
#define LSM303_INT1_SRC        0x31
#define LSM303_INT1_THS        0x32
#define LSM303_INT1_DURATION   0x33
#define LSM303_INT2_CFG        0x34
#define LSM303_INT2_SRC        0x35
#define LSM303_INT2_THS        0x36
#define LSM303_INT2_DURATION   0x37

#define LSM303_CLICK_CFG       0x38
#define LSM303_CLICK_SRC       0x39
#define LSM303_CLICK_THS       0x3A
#define LSM303_TIME_LIMIT      0x3B
#define LSM303_TIME_LATENCY    0x3C
#define LSM303_TIME_WINDOW     0x3D
/*****&**MAGNETOMETR***************/
#define LSM303_CRA_REG         0x00
#define LSM303_CRB_REG         0x01
#define LSM303_MR_REG          0x02

#define LSM303_OUT_X_H_M       0x03
#define LSM303_OUT_X_L_M       0x04
#define LSM303_OUT_Y_H_M       0x05
#define LSM303_OUT_Y_L_M       0x06
#define LSM303_OUT_Z_H_M       0x07
#define LSM303_OUT_Z_L_M       0x08

#define LSM303_SR_REG          0x09
#define LSM303_IRA_REG         0x0A
#define LSM303_IRB_REG         0x0B
#define LSM303_IRC_REG         0x0C

#define LSM303_TEMP_OUT_H      0x31
#define LSM303_TEMP_OUT_L      0x32

#define LSM303DLHC_OUT_Z_H     0x05
#define LSM303DLHC_OUT_Z_L     0x06
#define LSM303DLHC_OUT_Y_H     0x07
#define LSM303DLHC_OUT_Y_L     0x08

class lsm303dlhc
{

 public:

  lsm303dlhc();
  lsm303dlhc(const char * i2cDeviceName);  
  lsm303dlhc(const char * i2cDeviceName, const Config &cfg);

  typedef struct ivector
  {
    int x, y, z;
  } ivector;

  typedef struct fvector
  {
    float x, y, z;
  } fvector;

  ivector a; // accelerometer readings raw data
  ivector m; // magnetometer reading raw data
  float a_pitch; // Accelerometer pich

  void enable(void);
  void readAccRaw(void);
  void readMagRaw(void);
  int pitch(void);
  int heading(void);
  int heading(fvector from);
    
 private:
  I2CBus i2c_lsm303;

  uint8_t readAccRegister(uint8_t regAddr);
  uint8_t readMagRegister(uint8_t regAddr);
  void writeAccRegister(uint8_t regAddr, uint8_t byte);
  void writeMagRegister(uint8_t regAddr, uint8_t bytte);

  static void vector_cross(const fvector *a, const fvector *b, fvector *out);
  static float vector_dot(const fvector *a,const fvector *b);
  static void vector_normalize(fvector *a);

  ivector m_max; // maximum magnetometer values, used for calibration
  ivector m_min; // minimum magnetometer values, used for calibration
  ivector a_max; // maximum accelerometer values, used for calibration
  ivector a_min; // minimum accelerometer values, used for calibration
  ivector a_init; // Init accelerometer readings

};

#endif
