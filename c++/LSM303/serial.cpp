#include "lsm303.h"
#include <stdio.h>
#include <unistd.h>
#include <algorithm>

int main(void)
{
  const char *fileN = "/dev/i2c-1";
  lsm303 sensor(fileN);
  sensor.enable();

  while(1)
    {
      sensor.readAcceleration();
      // sensor.readAccelerationRaw();
      sensor.readMagnetometerRaw();

      printf("\e[27;1;31mAcc: X=%d :: Y=%d :: Z=%d \e[m \e[27;1;34mMag Heading: %d\e[m \n",
	     (int16_t)sensor.a.x, (int16_t)sensor.a.y, (int16_t)sensor.a.z, sensor.heading());

      usleep(100000);
    }

}

