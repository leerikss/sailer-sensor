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

/*
    // TEST
    deque<point> ps;
    point p1, p2, p3;
    p1.x = 24.965609;
    p1.y = 60.187655;
    ps.push_back(p1);

    p2.x = 24.965439;
    p2.y = 60.187459;
    ps.push_back(p2);

    p3.x = 24.965495;
    p3.y = 60.187665;
    ps.push_back(p3);

    line l = mathutil::getLinReg(ps);
    printf("%f,%f,%f,%f\n",l.p1.x, l.p1.y, l.p2.x, l.p2.y);
*/

    // TEST
/*
  {"epx": 10.628, "epy": 13.002, "altitude": 34.304, "longitude": 24.967242, "satellites": 8, "time": 747594000, "latitude": 60.186609}, {"epx": 10.628, "epy": 13.002, "altitude": 32.806, "longitude": 24.967193, "satellites": 8, "time":\
  747594001, "latitude": 60.186615}, {"epx": 10.628, "epy": 13.002, "altitude": 31.55, "longitude": 24.96712, "satellites": 8, "time": 747594003, "latitude": 60.186616}, {"epx": 10.628, "epy": 13.002, "altitude": 29.812, "longitude": 24\
 .967095, "satellites": 8, "time": 747594004, "latitude": 60.186626}, {"epx": 10.628, "epy": 13.002, "altitude": 29.392, "longitude": 24.967062, "satellites": 8, "time": 747594005, "latitude": 60.186632}, {"epx": 10.628, "epy": 13.002, \
 "altitude": 28.522, "longitude": 24.967037, "satellites": 8, "time": 747594006, "latitude": 60.186646}
 */
/*
    double lat1=60.187655;
    double lon1=24.965609;
    double lat2=60.187459;
    double lon2=24.965439;
    double b = mathutil::getBearing(lat1,lon1,lat2,lon2);
    printf("Bearing: %f\n",b);
*/

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
