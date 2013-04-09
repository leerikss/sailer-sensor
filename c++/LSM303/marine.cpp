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
      sensor.readAccRow();
      sensor.readMagRaw();

      printf("\e[27;1;31mAcc Roll: %d°\e[m    \e[27;1;34mMag Heading: %d°\e[m\n",
	     (int)sensor.a_roll, (int)sensor.heading() );

      usleep(100000);
    }

}

