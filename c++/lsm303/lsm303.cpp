#include"lsm303.h"
#include<math.h>

/*Conection to Raspberry PI:
  LSM303     Raspberry PI
  VDD    ->  3V3(PIN 1)
  SDA    ->  SDA(PIN 3)
  SCL    ->  SCL(PIN 5)
  GND    ->  GND(PIN 6)
*/

#define LSM303DLHC_MAG_ADDRESS            (0x3C >> 1)
#define LSM303DLHC_ACC_ADDRESS            (0x32 >> 1)
#define RADIUS                            320 / M_PI;

lsm303::lsm303(const char * i2cDeviceName) : i2c_lsm303(i2cDeviceName)
{
  // Maximum values fetched titling sensor while running calibrate.

  // Set these values by running ./calibrate.bin, turn the sensor in all dirs, and copy the Mag values
  m_max.x = 568;    m_max.y = 374;    m_max.z = 484;
  m_min.x = -669;   m_min.y = -924;   m_min.z = -567;

  // Set these values by running ./serial.bin and copy the Acc: X,Y,Z values here
  flat.x = 29; flat.y = 13; flat.z = -971;
  //flat.x = 722; flat.y = 13; flat.z = -632;
  //flat.x = -134; flat.y = 12; flat.z = -972;
  // flat.x = 54; flat.y = -395; flat.z = -895;

  // abs flat.z
  flat.z = (flat.z<0) ? (flat.z*-1) : flat.z;
}

uint8_t lsm303::readAccRegister(uint8_t regAddr)
{
  i2c_lsm303.addrSet(LSM303DLHC_ACC_ADDRESS);
  return i2c_lsm303.readByte(regAddr);
}

uint8_t lsm303::readMagRegister(uint8_t regAddr)
{
  i2c_lsm303.addrSet(LSM303DLHC_MAG_ADDRESS);
  return i2c_lsm303.readByte(regAddr);
}

void lsm303::writeAccRegister(uint8_t regAddr,uint8_t byte)
{
  i2c_lsm303.addrSet(LSM303DLHC_ACC_ADDRESS);
  i2c_lsm303.writeByte(regAddr, byte);

}

void lsm303::writeMagRegister(uint8_t regAddr, uint8_t byte)
{
  i2c_lsm303.addrSet(LSM303DLHC_MAG_ADDRESS);
  i2c_lsm303.writeByte(regAddr, byte);

}

void lsm303::enable(void)
{
  writeAccRegister(LSM303_CTRL_REG1, 0b00100111);
  writeAccRegister(LSM303_CTRL_REG4, 0b00001000);

  writeMagRegister(LSM303_MR_REG, 0x00);
}

void lsm303::readAccRaw(void)
{
  uint8_t block[6];

  i2c_lsm303.addrSet(LSM303DLHC_ACC_ADDRESS);

  i2c_lsm303.readBlock(0x80 | LSM303_OUT_X_L_A, sizeof(block), block);

  a.x = ((int16_t)(block[0] | block[1] << 8) >> 4);
  a.y = ((int16_t)(block[2] | block[3] << 8) >> 4);
  a.z = ((int16_t)(block[4] | block[5] << 8) >> 4);
} 

void lsm303::readMagRaw(void)
{
  uint8_t block[6];

  i2c_lsm303.addrSet(LSM303DLHC_MAG_ADDRESS);
  i2c_lsm303.readBlock(0x80 | LSM303_OUT_X_H_M, sizeof(block), block);

  m.x = (int16_t)(block[1] | block[0] << 8);
  m.y = (int16_t)(block[5] | block[4] << 8);
  m.z = (int16_t)(block[3] | block[2] << 8);
}

void lsm303::readAccPitch(void)
{
  readAccRaw();
  
  float x = a.x - flat.x;
  float y = a.y - flat.y;
  float z = a.z - flat.z;

  a_pitch = atan(  x / sqrt(y * y + z * z) ) * RADIUS;
}

// Returns the number of degrees from the -Y axis that it
// is pointing.
int lsm303::heading(void)
{
  return heading((vector){0,-1,0});
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
int lsm303::heading(vector from)
{
  // shift and scale
  m.x = (m.x - m_min.x) / (m_max.x - m_min.x) * 2 - 1.0;
  m.y = (m.y - m_min.y) / (m_max.y - m_min.y) * 2 - 1.0;
  m.z = (m.z - m_min.z) / (m_max.z - m_min.z) * 2 - 1.0;

  vector temp_a = a;
  // normalize
  vector_normalize(&temp_a);
  //vector_normalize(&m);

  // compute E and N
  vector E;
  vector N;
  vector_cross(&m, &temp_a, &E);
  vector_normalize(&E);
  vector_cross(&temp_a, &E, &N);

  // compute heading
  int heading = round(atan2(vector_dot(&E, &from), vector_dot(&N, &from)) * 180 / M_PI);
  if (heading < 0) heading += 360;
  return heading;
}

void lsm303::vector_cross(const vector *a,const vector *b, vector *out)
{
  out->x = a->y*b->z - a->z*b->y;
  out->y = a->z*b->x - a->x*b->z;
  out->z = a->x*b->y - a->y*b->x;
}

float lsm303::vector_dot(const vector *a,const vector *b)
{
  return a->x*b->x+a->y*b->y+a->z*b->z;
}

void lsm303::vector_normalize(vector *a)
{
  float mag = sqrt(vector_dot(a,a));
  a->x /= mag;
  a->y /= mag;
  a->z /= mag;
}


