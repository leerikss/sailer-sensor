#include "lsm303.h"
#include <stdio.h>
#include <unistd.h>
#include <algorithm>

int main(void)
{
  const char *fileN = "/dev/i2c-1";
  lsm303 sensor(fileN);
  sensor.enable();

  lsm303::vector mag_min = {32.767, 32.767, 32.767}, mag_max = {-32.768, -32.768, -32.768};
  lsm303::vector acc_min = {32.767, 32.767, 32.767}, acc_max = {-32.768, -32.768, -32.768};

  while(1)
    {
      sensor.readAccelerationRaw();
      sensor.readMagnetometerRaw();

      mag_min.x = std::min(mag_min.x, sensor.m.x);
      mag_min.y = std::min(mag_min.y, sensor.m.y);
      mag_min.z = std::min(mag_min.z, sensor.m.z);
      mag_max.x = std::max(mag_max.x, sensor.m.x);
      mag_max.y = std::max(mag_max.y, sensor.m.y);
      mag_max.z = std::max(mag_max.z, sensor.m.z);

      acc_min.x = std::min(acc_min.x, sensor.a.x);
      acc_min.y = std::min(acc_min.y, sensor.a.y);
      acc_min.z = std::min(acc_min.z, sensor.a.z);
      acc_max.x = std::max(acc_max.x, sensor.a.x);
      acc_max.y = std::max(acc_max.y, sensor.a.y);
      acc_max.z = std::max(acc_max.z, sensor.a.z);

      printf("\e[27;1;31m Mag Max: X=%d Y=%d Z=%d :: Mag Min:\e[27;1;31m X=%d Y=%d Z=%d   \e[27;1;34m Acc Max: X=%d Y=%d Z=%d :: Acc Min:\e[27;1;34m X=%d Y=%d Z=%d\e[m\n",
	     (int16_t)mag_max.x,(int16_t)mag_max.y,(int16_t)mag_max.z,(int16_t)mag_min.x,(int16_t)mag_min.y,(int16_t)mag_min.z,
	     (int16_t)acc_max.x,(int16_t)acc_max.y,(int16_t)acc_max.z,(int16_t)acc_min.x,(int16_t)acc_min.y,(int16_t)acc_min.z);

      usleep(100);
    }

}

