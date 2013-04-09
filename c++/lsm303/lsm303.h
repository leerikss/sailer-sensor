#ifndef lsm303_h
#define lsm303_h
#include <stdint.h>
#include"I2C_Bus.h"

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

class lsm303
{

 public:

  typedef struct vector
  {
    float x, y, z;
  } vector;

  vector a; // accelerometer readings raw data
  vector m; // magnetometer reading raw data
  vector flat; // x,y,z of values when device is flat (tilted or not)
  float a_pitch; // Accelerometer pich
  float a_roll; // Accelerometer roll
  
  lsm303(const char * i2cDeviceName);

  void setAccRatios(void);
  uint8_t readAccRegister(uint8_t regAddr);
  uint8_t readMagRegister(uint8_t regAddr);
  void writeAccRegister(uint8_t regAddr, uint8_t byte);
  void writeMagRegister(uint8_t regAddr, uint8_t bytte);
  void enable(void);
  void readAccRaw(void);
  void readMagRaw(void);
  void readAccRow(void);
  int addTilt(float angle, float tilt);
  int heading(void);
  int heading(vector from);
  static void vector_cross(const vector *a, const vector *b, vector *out);
  static float vector_dot(const vector *a,const vector *b);
  static void vector_normalize(vector *a);
    
 private:
  I2CBus i2c_lsm303;

  vector m_max; // maximum magnetometer values, used for calibration
  vector m_min; // minimum magnetometer values, used for calibration
  vector a_max; // maximum accelerometer values, used for calibration
  vector a_min; // minimum accelerometer values, used for calibration
  float cal_mag; // Calibrate magnet
};

#endif
