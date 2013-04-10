#include <iostream>
#include <cstdlib>
#include <libconfig.h++>
#include <string.h>
#include <iostream>
#include "lsm303dlhc.h"

#define CONFIG_FILE "lsm303dlhc.cfg"

using namespace std;
using namespace libconfig;

void usage(void)
{
  printf("Usage:\n");
  printf("--raw: Outputs the raw data retrieved from the device\n");
  printf("--cal: Outputs maximum levels needed for calibration (turn around the device for maximum values)\n");
  printf("--pih: Outputs accelerometer pitch (deg) and magnetometer heading (tilt compensated)\n");
}

void raw(lsm303dlhc &sensor)
{
  printf("\e[27;1;34mAcc = {x=%d, y=%d, z=%d}    \e[27;1;31m Mag = {x=%d, y=%d, z=%d}\e[m\n",
	 (int16_t)sensor.m.x, (int16_t)sensor.m.y, (int16_t)sensor.m.z, 
	 (int16_t)sensor.a.x, (int16_t)sensor.a.y, (int16_t)sensor.a.z );
}

void cal(lsm303dlhc &sensor, lsm303dlhc::vector &mag_min, lsm303dlhc::vector &mag_max, 
	       lsm303dlhc::vector &acc_min, lsm303dlhc::vector &acc_max)
{
      mag_min.x = min(mag_min.x, sensor.m.x);
      mag_min.y = min(mag_min.y, sensor.m.y);
      mag_min.z = min(mag_min.z, sensor.m.z);
      mag_max.x = max(mag_max.x, sensor.m.x);
      mag_max.y = max(mag_max.y, sensor.m.y);
      mag_max.z = max(mag_max.z, sensor.m.z);

      acc_min.x = min(acc_min.x, sensor.a.x);
      acc_min.y = min(acc_min.y, sensor.a.y);
      acc_min.z = min(acc_min.z, sensor.a.z);
      acc_max.x = max(acc_max.x, sensor.a.x);
      acc_max.y = max(acc_max.y, sensor.a.y);
      acc_max.z = max(acc_max.z, sensor.a.z);

      printf("\e[27;1;31mMag max = {x=%d, y=%d z=%d}    Mag min = {x=%d, y=%d, z=%d}    \e[27;1;34mAcc max = {x=%d, y=%d, z=%d}    Acc min = {x=%d, y=%d, z=%d}\e[m\n",
	     (int16_t)mag_max.x,(int16_t)mag_max.y,(int16_t)mag_max.z,(int16_t)mag_min.x,(int16_t)mag_min.y,(int16_t)mag_min.z,
	     (int16_t)acc_max.x,(int16_t)acc_max.y,(int16_t)acc_max.z,(int16_t)acc_min.x,(int16_t)acc_min.y,(int16_t)acc_min.z);

}

void pih(lsm303dlhc &sensor)
{
  printf("\e[27;1;31mAcc Roll: %d°\e[m    \e[27;1;34mMag Heading: %d°\e[m\n",
	 (int)sensor.a_pitch, (int)sensor.heading() );
}

int main(int argc,char *argv[])
{
  // Parse arguments
  char arg='?'; int i;
  for(i=0; i < argc ; i++)
  {
    if(!strcmp(argv[i], "--raw") )
      arg='r';
    if(!strcmp(argv[i], "--cal") )
      arg='c';
    if(!strcmp(argv[i], "--pih") )
      arg='p';
  }
  if(arg == '?')
  {
    usage();
    return(EXIT_FAILURE);
  }

  // Read the config file
  Config cfg;
  try { cfg.readFile(CONFIG_FILE);  }
  catch(const FileIOException &fioex)
  {
    std::cerr << "I/O error while reading file." << std::endl;
    return(EXIT_FAILURE);
  }
  catch(const ParseException &pex)
  {
    std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
	      << " - " << pex.getError() << std::endl;
    return(EXIT_FAILURE);
  }

  // Init & enable the lsm303dlhc sensor
  const char *fileN = "/dev/i2c-1";
  const Setting &settings = cfg.getRoot();
  lsm303dlhc sensor = lsm303dlhc(fileN, settings);
  sensor.enable();

  // Calibration max/min vectors
  lsm303dlhc::vector mag_min = {32767.0, 32767.0, 32767.0}, mag_max = {-32768.0, -32768.0, -32768.0};
  lsm303dlhc::vector acc_min = {32767.0, 32767.0, 32767.0}, acc_max = {-32768.0, -32768.0, -32768.0};

  // Loop sleep time
  int ms;
  cfg.lookupValue("sleep",ms);

  // Main loop
  while(1)
  {
    sensor.readMagRaw();
    switch(arg)
    {
      case 'r':
	sensor.readAccRaw();
	sensor.readMagRaw();
	raw(sensor);
	break;

      case 'c':
	sensor.readAccRaw();
	sensor.readMagRaw();
        cal(sensor,mag_min,mag_max,acc_min,acc_max);
	break;

      case 'p':
	sensor.readAccPitch();
	sensor.readMagRaw();
	pih(sensor);
	break;


    }

    // TODO: From configs
    usleep(ms);
  }

}
