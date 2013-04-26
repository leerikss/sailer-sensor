#include "sailersensor.h"
#include <iostream>
#include <cstdlib>
#include <libconfig.h++>
#include <pthread.h>
#include <unistd.h>
#include "lsmpoller.h"
#include "dao.h"
#include "structs.h"
#include "socketclient.h"
#include "mathutil.h" // <-- Remove
#include <deque> // <-- Remove

#define CONFIG_FILE "/home/pi/WORKING/sailer-sensor/c++/sailersensor.cfg"

using namespace libconfig;
using namespace std;

int main() 
{
  try
  {
    // TODO: Get config from argument

    /*
    // Read configs
    Config cfg;
    cfg.readFile(CONFIG_FILE);
    sailersensor ss = sailersensor(cfg);
    // ss.run();
    */

    // TEST
    deque<gps> gs;
    gps g1,g2,g3,g4,g5,g6;
    g1.lon=24.967242; g1.lat=60.186609; g1.time=747594000;
    g2.lon=24.967193; g2.lat=60.186615; g2.time=747594001;
    g3.lon=24.96712; g3.lat=60.186616; g3.time=747594003;
    g4.lon=24.967095; g4.lat=60.186626; g4.time=747594004;
    g5.lon=24.967062; g5.lat=60.186632; g5.time=747594005;
    gs.push_back(g1);
    gs.push_back(g2);
    gs.push_back(g3);
    gs.push_back(g4);
    gs.push_back(g5);
    double h = mathutil::getHeading(gs);
    double s = mathutil::getSpeedInKnots(gs);
    printf("h=%f,s=%f\n",h,s);

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

sailersensor::sailersensor(const Config& cfg) : db(cfg), lsm_p(cfg), gps_p(cfg)
{
  s_time = cfg.lookup("sailersensor.sleep");
  display_ip = cfg.lookup("sailersensor.display_ip");
  display_port = cfg.lookup("sailersensor.display_port");
  store_data = cfg.lookup("sailersensor.store_data");
}

void sailersensor::run(void)
{
  // Start lsmpoller thread
  // pthread_t lsm_t;
  // pthread_create(&lsm_t, NULL, &lsmpoller::startThread, &lsm_p);

  // Start gpspoller thread
  pthread_t gps_t;
  pthread_create(&gps_t, NULL, &gpspoller::startThread, &gps_p);

  // Init socket
  // socketclient s;

  // Cache previous values
  double prevLat,prevLon;

  while(true)
  {
    try
    {
      // Store raw gps data into db
      if( store_data )
      {
	const gps& g = gps_p.getLatestPos();
	if( g.lat != prevLat && g.lon != prevLon )
	{
	  db.insertGps(g);
	  prevLat = g.lat;
	  prevLon = g.lon;
	}
      }

      /*
	printf("Sailersensor: Lat: %f, Lon: %f, Alt: %f, Time: %d\n", g.lat, \
	g.lon, g.alt, g.time );
      */

      /*
	int p = lsm_p.get_pitch();
	int h = lsm_p.get_heading();
	cout << "Sailersensor: Pitch: " << p << ", Heading: " << h << endl;
      */

      /*

      // Send to socket
      if(s.conn(display_ip,display_port) )
      {
	s.send_data("");
	s.close();
      }
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
