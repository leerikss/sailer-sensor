#include <iostream>
#include <cstdlib>
#include <libconfig.h++>
#include <string.h>
#include "lsm303dlhc.h"
#include <unistd.h>

using namespace libconfig;
using namespace std;

void usage(void)
{
  printf("Usage: lsmout.bin --[raw|cal|acc] path/tp/config_file.cfg\n");
  printf("--raw: Outputs the raw data retrieved from the device\n");
  printf("--cal: Outputs maximum levels needed for calibration (turn around the device for maximum values)\n");
  printf("--pih: Outputs accelerometer pitch (deg) and magnetometer heading (tilt compensated)\n");
  printf("--acc: Output current accelerometer values without looping\n");
}

void raw(lsm303dlhc &sensor)
{
  printf("\e[27;1;31mAcc: { x: %d, y: %d, z: %d }    \e[27;1;34m Mag: { x: %d, y: %d, z: %d }\e[m\n",
	 sensor.a.x, sensor.a.y, sensor.a.z,
	 sensor.m.x, sensor.m.y, sensor.m.z); 
}

void acc(lsm303dlhc &sensor)
{
  printf("{ x: %d, y: %d, z: %d }\n", sensor.a.x, sensor.a.y, sensor.a.z); 
}

void cal(lsm303dlhc &sensor, lsm303dlhc::ivector &mag_min, lsm303dlhc::ivector &mag_max, 
	       lsm303dlhc::ivector &acc_min, lsm303dlhc::ivector &acc_max)
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

      printf("\e[27;1;31mMag max: { x: %d, y: %d, z: %d }    Mag min: {x: %d, y: %d, z: %d;};    \
\e[27;1;34mAcc max: { x: %d, y: %d, z: %d }    Acc min: { x: %d, y: %d, z: %d }\e[m\n",
	     mag_max.x,mag_max.y,mag_max.z,mag_min.x,mag_min.y,mag_min.z,
	     acc_max.x,acc_max.y,acc_max.z,acc_min.x,acc_min.y,acc_min.z);

}

void pih(lsm303dlhc &sensor)
{
  printf("\e[27;1;31mAcc Pitch: %d°\e[m    \e[27;1;34mMag Heading: %d°\e[m\n",
	 sensor.pitch(), sensor.heading() );
}

int main(int argc,char *argv[])
{
  // Parse arguments
  char arg='?'; 
  bool loop=true;

  if( sizeof(argv) < 3)
  {
    usage();
    return(EXIT_FAILURE);
  }

  if(!strcmp(argv[1], "--raw") )
    arg='r';
  if(!strcmp(argv[1], "--cal") )
    arg='c';
  if(!strcmp(argv[1], "--pih") )
    arg='p';
  if(!strcmp(argv[1], "--acc") )
    arg='a';

  if(arg == '?')
  {
    usage();
    return(EXIT_FAILURE);
  }

  // Read the config file
  Config cfg;
  try { cfg.readFile( argv[2] );  }
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
  lsm303dlhc sensor = lsm303dlhc(fileN, cfg);
  sensor.enable();

  // Calibration max/min vectors
  lsm303dlhc::ivector mag_min = {32767, 32767, 32767}, mag_max = {-32768, -32768, -32768};
  lsm303dlhc::ivector acc_min = {32767, 32767, 32767}, acc_max = {-32768, -32768, -32768};

  // Loop sleep time
  int ms;
  cfg.lookupValue("config.sleep",ms);

  // Main loop
  while(loop)
  {
    sensor.readAccRaw();
    sensor.readMagRaw();

    switch(arg)
    {
      case 'r':
	raw(sensor);
	break;

      case 'c':
        cal(sensor,mag_min,mag_max,acc_min,acc_max);
	break;

      case 'p':
	pih(sensor);
	break;

      case 'a':
	acc(sensor);
	loop=false;
	break;
    }
    if(loop)
      usleep(ms);
  }
}
