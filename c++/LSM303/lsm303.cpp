#include"lsm303.h"
#include<math.h>
#include<stdio.h>

/*Conection to Raspberry PI:
  LSM303     Raspberry PI
  VDD    ->  3V3(PIN 1)
  SDA    ->  SDA(PIN 3)
  SCL    ->  SCL(PIN 5)
  GND    ->  GND(PIN 6)
*/

#define LSM303DLHC_MAG_ADDRESS            (0x3C >> 1)
#define LSM303DLHC_ACC_ADDRESS            (0x32 >> 1)

lsm303::lsm303(const char * i2cDeviceName) : i2c_lsm303(i2cDeviceName)
{
  // Maximum values fetched titling sensor while running calibrate.

  // TODO: Get these from a textile
  m_max.x = 536;  m_max.y = 326; m_max.z = 404;
  m_min.x = -621;  m_min.y = -895;  m_min.z = -555;
  a_max.x = 1020; a_max.y = 1061; a_max.z = 1118;
  a_min.x = -1120; a_min.y = -1068; a_min.z = -1060;
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

void lsm303::readAccelerationRaw(void)
{
  uint8_t block[6];

  i2c_lsm303.addrSet(LSM303DLHC_ACC_ADDRESS);

  i2c_lsm303.readBlock(0x80 | LSM303_OUT_X_L_A, sizeof(block), block);

  a.x = ((int16_t)(block[0] | block[1] << 8) >> 4);
  a.y = ((int16_t)(block[2] | block[3] << 8) >> 4);
  a.z = ((int16_t)(block[4] | block[5] << 8) >> 4);

}

void lsm303::readMagnetometerRaw(void)
{
  uint8_t block[6];

  i2c_lsm303.addrSet(LSM303DLHC_MAG_ADDRESS);
  i2c_lsm303.readBlock(0x80 | LSM303_OUT_X_H_M, sizeof(block), block);

  m.x = (int16_t)(block[1] | block[0] << 8);
  m.y = (int16_t)(block[5] | block[4] << 8);
  m.z = (int16_t)(block[3] | block[2] << 8);

}

void lsm303::readAcceleration(void)
{
  readAccelerationRaw();
}



