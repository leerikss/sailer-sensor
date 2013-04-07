#include "I2C_Bus.h"
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "smbus.h"



/*DeviceName path to pherpih np. /dev/i2c-1*/
I2CBus::I2CBus(const char * devName)
{
    stream = open(devName, O_RDWR);
    if (stream == -1)
    {
        perror("Failed to open %s device.");
    }
}

I2CBus::~I2CBus()
{
    close(stream);
}

void I2CBus::addrSet(uint8_t address)
{
    if (ioctl(stream, I2C_SLAVE, address) == -1)
    {
        perror("Failed to acquire bus access and/or talk to slave");
        exit(1);
    }
}

void I2CBus::writeByte(uint8_t regAddr, uint8_t byte)
{

    if (i2c_smbus_write_byte_data(stream, regAddr, byte) == -1)
    {
        perror("Failed to write byte to I2C.");
        exit(1);
    }
}

uint8_t I2CBus::readByte(uint8_t regAddr)
{
    uint8_t result;
    if (( result = i2c_smbus_read_byte_data(stream, regAddr)) == -1)
    {
        perror("Failed to read byte from I2C.");
        exit(1);
    }
    return result;
}

void I2CBus::readBlock(uint8_t regAddr, uint8_t size, uint8_t * data)
{
    if (i2c_smbus_read_i2c_block_data(stream, regAddr, size, data) != size)
    {
        perror("Failed to read block from I2C.");
        exit(1);
    }
}

