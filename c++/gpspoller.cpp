#include "gpspoller.h"
#include <libconfig.h++>
#include <iostream>
#include <unistd.h>
#include <gps.h>
#include <deque>
#include "structs.h"

using namespace libconfig;
using namespace std;

gpspoller::gpspoller(const Config& cfg)
{
  // Set vals from config
  s_time = cfg.lookup("gpspoller.sleep");
  g_size = cfg.lookup("gpspoller.buffer_size");
}

void gpspoller::run(void)
{
  // gpsd buffer
  gps_data_t gpsdata_t;
  gps_data_t* gpsdata = &gpsdata_t;

  // Connect to the device
  open(gpsdata);

  running = true;
  while(running)
  {
    // Read data and loop without sleeping if valid data is retrieved
    // When sleeping between the readings, a continuous lag would occur,
    // so reading as fast as possible to get the freshest data
    if ( gps_waiting(gpsdata, (500) ) && 
	 ( gps_read(gpsdata) != -1 ) && 
	 ( gpsdata->status > 0 ) && 
	 !(gpsdata->fix.latitude  != gpsdata->fix.latitude) &&
	 !(gpsdata->fix.longitude != gpsdata->fix.longitude) )
    {
      // Add to deque
      gps_struct g;
      g.latitude = gpsdata->fix.latitude;
      g.longitude = gpsdata->fix.longitude;
      g.altitude = gpsdata->fix.altitude;
      g.time = (int)gpsdata->fix.time;
      add_deque(g_deque, g, g_size);
/*
      printf("Latitude: %f\tLongitude: %f\tTime: %d\n", gpsdata->fix.latitude, gpsdata->fix.longitude, (int)gpsdata->fix.time);
*/
    }
    // If gpsd has no new data, or data is invalid, sleep to spare cpu
    else
    {
      usleep(s_time);
    }
  }

  // Close
  close(gpsdata);
}

const gps_struct& gpspoller::getLatestPos(void)
{
  return g_deque.front();
}

const float gpspoller::getHeading(void)
{
  return 0;
  // TODO: Implement
  // TODO: Take standstill into account
}

const float gpspoller::getSpeedInKnots(void)
{
  return 0;
  // TODO: Implement
  // TODO: Take standstill into account
}

void gpspoller::open(gps_data_t* gpsdata)
{
  while( gps_open("localhost", DEFAULT_GPSD_PORT, gpsdata) < 0 )
  {
      cerr << "Unable to connect to device. Trying again..." << endl;
      usleep(s_time);
  }
  gps_stream(gpsdata, WATCH_ENABLE, NULL);
}

void gpspoller::close(gps_data_t* gpsdata)
{
  gps_stream(gpsdata, WATCH_DISABLE, NULL);
  gps_close(gpsdata);
}

void gpspoller::add_deque(deque<gps_struct>& d, gps_struct& v, unsigned int& s)
{
  // TODO: Filter out corrupt peak values
  if( isValidPoint(v) )
  {
    d.push_front(v);
    if( d.size() > s )
      d.pop_back();
  }
}

bool gpspoller::isValidPoint(gps_struct& g)
{
  // TODO: Implement; don't allow points outside realistic reach
  return true;
}
