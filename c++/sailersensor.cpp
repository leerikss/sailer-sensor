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

using namespace libconfig;
using namespace std;

int main(int argc,char *argv[]) 
{
  try
  {
    // Config passed as argument
    if(argc != 2)
    {
      cerr << "sailersensor requires one argument only defining the path to the config file." << endl;
      return(EXIT_FAILURE);
    }

    // Read configs
    Config cfg;
    cfg.readFile(argv[1]);
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

sailersensor::sailersensor(const Config& cfg) : db(cfg), lsm_p(cfg), gps_p(cfg)
{
  s_time = cfg.lookup("sailersensor.sleep");
  display_ip = cfg.lookup("sailersensor.display_ip");
  display_port = cfg.lookup("sailersensor.display_port");
  store_data = cfg.lookup("sailersensor.store_data");
}

void sailersensor::run(void)
{
  
  /*
  // Start lsmpoller thread
  pthread_t lsm_t;
  pthread_create(&lsm_t, NULL, &lsmpoller::startThread, &lsm_p);

  // Start gpspoller thread
  pthread_t gps_t;
  pthread_create(&gps_t, NULL, &gpspoller::startThread, &gps_p);

  // Init socket
  socketclient s;

  // Cache previous values
  double prevLat,prevLon;

  while(true)
  {
    try
    {
      ig( !gps_p.isHalted() && ( g.lat != prevLat && g.lon != prevLon ) )
      {
	const gps& g = gps_p.getData();

	// Store into db
	if( store_data )
	  db.insertGps(g);

	prevLat = g.lat;
	prevLon = g.lon;

	printf("Sailersensor: Lat: %f, Lon: %f, Alt: %f, Time: %d\n", g.lat, \
	g.lon, g.alt, g.time );
      }

      int p = lsm_p.get_pitch();
      int h = lsm_p.get_heading();

      cout << "Sailersensor: Pitch: " << p << ", Heading: " << h << endl;

      // Send to display
      if(s.conn(display_ip,display_port) )
      {
	s.send_data("");
	s.close();
      }
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
  */

}
