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
      sensor.readMagRaw();
      sensor.readAccRaw();

      printf("\e[27;1;31m Mag: X = %d, Y = %d, Z = %d\e[27;1;34m    Acc: X = %d, Y = %d, Z = %d\e[m\n",
	     (int16_t)sensor.m.x, (int16_t)sensor.m.y, (int16_t)sensor.m.z, 
	     (int16_t)sensor.a.x, (int16_t)sensor.a.y, (int16_t)sensor.a.z );

      usleep(100000);
    }

}

