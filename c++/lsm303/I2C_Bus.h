#ifndef _I2C_Bus_h
#define _I2C_Bus_h

#include <stdint.h>


class I2CBus
{
public:
    I2CBus(const char * devName);
    ~I2CBus();

    void addrSet(uint8_t addrress);
    void writeByte(uint8_t regAddr, uint8_t byte);
    uint8_t readByte(uint8_t regAddr);
    void readBlock(uint8_t regAddr, uint8_t size, uint8_t * data);

private:
    int stream;
};

#endif
