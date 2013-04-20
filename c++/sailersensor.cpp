#include "sailersensor.h"
#include <iostream>
#include <cstdlib>
#include <libconfig.h++>
#include <pthread.h>
#include <unistd.h>
#include "lsmpoller.h"
#include "dao.h"
#include "structs.h"

#define CONFIG_FILE "/home/pi/WORKING/sailer-sensor/c++/sailersensor.cfg"

using namespace libconfig;
using namespace std;

int main() 
{
  try
  {
    // Read configs
    Config cfg;
    cfg.readFile(CONFIG_FILE);
    sailersensor ss = sailersensor(cfg);
    ss.run();
  }
  catch(const FileIOException &fioex)
  {
    cerr << "I/O error while reading file." << endl;
    return(EXIT_FAILURE);
  }
  catch(const ParseException &pex)
  {
    cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
	 << " - " << pex.getError() << endl;
    return(EXIT_FAILURE);
  }
  
  // return EXIT_SUCCESS;
}

sailersensor::sailersensor(const Config& cfg) : lsm_p(cfg), gps_p(cfg)
  
{
  s_time = cfg.lookup("sailersensor.sleep");
}

void sailersensor::run(void)
{
  // Start lsmpoller thread
  pthread_t lsm_t;
  pthread_create(&lsm_t, NULL, &lsmpoller::startRun, &lsm_p);

  // Start gpspoller thread
  pthread_t gps_t;
  pthread_create(&gps_t, NULL, &gpspoller::startRun, &gps_p);

  while(true)
  {
    try
    {
      // TODO: Do this if conf allows
      const gps_struct& g = gps_p.getLatestPos();
      dao::getInstance().insertGps(g);

      /*
      printf("Sailersensor: Lat: %f, Lon: %f, Alt: %f, Time: %d\n", g.latitude, \
	   g.longitude, g.altitude, g.time );
      */

      /*
      int p = lsm_p.get_pitch();
      int h = lsm_p.get_heading();
      cout << "Sailersensor: Pitch: " << p << ", Heading: " << h << endl;
      */

    }
    catch( const std::exception & ex ) 
    {
      cerr << "sailersensor::run(): Error " << ex.what() << endl;
    }      
    catch(...)
    {
      cerr << "sailersensor::run(): Unexpected Error" << endl;
    }
    usleep(s_time);
  }  
}
