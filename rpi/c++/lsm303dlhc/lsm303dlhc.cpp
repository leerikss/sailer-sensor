#include"lsm303dlhc.h"
#include<math.h>
#include"I2C_Bus.h"
#include <libconfig.h++>

using namespace libconfig;

/*Conection to Raspberry PI:
  LSM303     Raspberry PI
  VDD    ->  3V3(PIN 1)
  SDA    ->  SDA(PIN 3)
  SCL    ->  SCL(PIN 5)
  GND    ->  GND(PIN 6)set
*/

#define LSM303DLHC_MAG_ADDRESS            (0x3C >> 1)
#define LSM303DLHC_ACC_ADDRESS            (0x32 >> 1)
#define R                                 320 / M_PI;

lsm303dlhc::lsm303dlhc(const char *i2cDeviceName) : i2c_lsm303(i2cDeviceName)
{
  // Use some default values if no config is passed
  m_max.x = 568; m_max.y = 374; m_max.z = 484;
  m_min.x = -669; m_min.y = -924; m_min.z = -567;
  a_init.x = 13; a_init.y = 10; a_init.z = 964;
}

lsm303dlhc::lsm303dlhc(const char *i2cDeviceName, const Config &cfg) : i2c_lsm303(i2cDeviceName)
{ 
  // Read in magnetometer settings
  m_max.x = cfg.lookup("magnetometer.max.x");
  m_max.y = cfg.lookup("magnetometer.max.y");
  m_max.z = cfg.lookup("magnetometer.max.z");
  m_min.x = cfg.lookup("magnetometer.min.x");
  m_min.y = cfg.lookup("magnetometer.min.y");
  m_min.z = cfg.lookup("magnetometer.min.z");
  // Read in accelerometer settings
  a_init.x = cfg.lookup("accelerometer.init.x");
  a_init.y = cfg.lookup("accelerometer.init.y");
  a_init.z = cfg.lookup("accelerometer.init.z");
  // Acc init must be abs
  a_init.z = (a_init.z<0) ? (a_init.z*-1) : a_init.z;
}

lsm303dlhc::~lsm303dlhc()
{

  //I2CBus *p = &i2c_lsm303;  
  //delete p;
}

uint8_t lsm303dlhc::readAccRegister(uint8_t regAddr)
{
  i2c_lsm303.addrSet(LSM303DLHC_ACC_ADDRESS);
  return i2c_lsm303.readByte(regAddr);
}

uint8_t lsm303dlhc::readMagRegister(uint8_t regAddr)
{
  i2c_lsm303.addrSet(LSM303DLHC_MAG_ADDRESS);
  return i2c_lsm303.readByte(regAddr);
}

void lsm303dlhc::writeAccRegister(uint8_t regAddr,uint8_t byte)
{
  i2c_lsm303.addrSet(LSM303DLHC_ACC_ADDRESS);
  i2c_lsm303.writeByte(regAddr, byte);

}

void lsm303dlhc::writeMagRegister(uint8_t regAddr, uint8_t byte)
{
  i2c_lsm303.addrSet(LSM303DLHC_MAG_ADDRESS);
  i2c_lsm303.writeByte(regAddr, byte);

}

void lsm303dlhc::enable(void)
{
  writeAccRegister(LSM303_CTRL_REG1, 0b00100111);
  writeAccRegister(LSM303_CTRL_REG4, 0b00001000);

  writeMagRegister(LSM303_MR_REG, 0x00);
}

void lsm303dlhc::readAccRaw(void)
{
  uint8_t block[6];

  i2c_lsm303.addrSet(LSM303DLHC_ACC_ADDRESS);

  i2c_lsm303.readBlock(0x80 | LSM303_OUT_X_L_A, sizeof(block), block);

  a.x = ((int16_t)(block[0] | block[1] << 8) >> 4);
  a.y = ((int16_t)(block[2] | block[3] << 8) >> 4);
  a.z = ((int16_t)(block[4] | block[5] << 8) >> 4);
} 

void lsm303dlhc::readMagRaw(void)
{
  uint8_t block[6];

  i2c_lsm303.addrSet(LSM303DLHC_MAG_ADDRESS);
  i2c_lsm303.readBlock(0x80 | LSM303_OUT_X_H_M, sizeof(block), block);

  m.x = (int16_t)(block[1] | block[0] << 8);
  m.y = (int16_t)(block[5] | block[4] << 8);
  m.z = (int16_t)(block[3] | block[2] << 8);
}

int lsm303dlhc::pitch(void)
{
  float x = (float)(a.x - a_init.x);
  float y = (float)(a.y - a_init.y);
  float z = (float)(a.z - a_init.z);

  return atan(  x / sqrt(y * y + z * z) ) * R;
}

// Returns the number of degrees from the -Y axis that it
// is pointing.
int lsm303dlhc::heading(void)
{
  return heading((fvector){0,-1,0});
}

// Returns the number of degrees from the From vector projected into
// the horizontal plane is away from north.
//
// Description of heading algorithm:
// Shift and scale the magnetic reading based on calibration data to
// to find the North vector. Use the acceleration readings to
// determine the Down vector. The cross product of North and Down
// vectors is East. The vectors East and North form a basis for the
// horizontal plane. The From vector is projected into the horizontal
// plane and the angle between the projected vector and north is
// returned.
int lsm303dlhc::heading(fvector from)
{
  fvector fm;

  // shift and scale
  fm.x = (float)(m.x - m_min.x) / (float)(m_max.x - m_min.x) * 2 - 1.0;
  fm.y = (float)(m.y - m_min.y) / (float)(m_max.y - m_min.y) * 2 - 1.0;
  fm.z = (float)(m.z - m_min.z) / (float)(m_max.z - m_min.z) * 2 - 1.0;

  fvector temp_a = { (float)a.x, (float)a.y, (float)a.z };

  // normalize
  vector_normalize(&temp_a);

  // compute E and N
  fvector E;
  fvector N;
  vector_cross(&fm, &temp_a, &E);
  vector_normalize(&E);
  vector_cross(&temp_a, &E, &N);

  // compute heading
  int heading = round(atan2(vector_dot(&E, &from), vector_dot(&N, &from)) * 180 / M_PI);
  if (heading < 0) heading += 360;
  return heading;
}

void lsm303dlhc::vector_cross(const fvector *a,const fvector *b, fvector *out)
{
  out->x = a->y*b->z - a->z*b->y;
  out->y = a->z*b->x - a->x*b->z;
  out->z = a->x*b->y - a->y*b->x;
}

float lsm303dlhc::vector_dot(const fvector *a,const fvector *b)
{
  return a->x*b->x+a->y*b->y+a->z*b->z;
}

void lsm303dlhc::vector_normalize(fvector *a)
{
  float mag = sqrt(vector_dot(a,a));
  a->x /= mag;
  a->y /= mag;
  a->z /= mag;
}


