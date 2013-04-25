#include "gpspoller.h"
#include <libconfig.h++>
#include <iostream>
#include <unistd.h>
#include <gps.h>
#include <deque>
#include "structs.h"
#include "mathutil.h"

using namespace libconfig;
using namespace std;

gpspoller::gpspoller(const Config& cfg)
{
  // Set vals from config
  s_time = cfg.lookup("gpspoller.sleep");
  buff_size = cfg.lookup("gpspoller.buffer_size");
  buff_skip_dist_m = cfg.lookup("gpspoller.buffer_skip_dist_m");
  buff_skip_dist_max = cfg.lookup("gpspoller.buffer_skip_dist_max");
  buff_skip_min_sec = cfg.lookup("gpspoller.buffer_skip_min_sec");
  buff_skip_dist_count = 0;
}

gpspoller::~gpspoller()
{
  gps_data_t* gpsdata = &gpsdata_t;
  close(gpsdata);
}

void* gpspoller::run(void)
{
  // gpsd buffer
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
	 (int)gpsdata->fix.time != 0 &&
	 gpsdata->satellites_used != 0 &&
         !(gpsdata->fix.latitude  != gpsdata->fix.latitude) &&
         !(gpsdata->fix.longitude != gpsdata->fix.longitude) &&
         !(gpsdata->fix.altitude != gpsdata->fix.altitude) )
    {
      // Add to deque
      gps_struct g;
      g.lat = gpsdata->fix.latitude;
      g.lon = gpsdata->fix.longitude;
      g.alt = gpsdata->fix.altitude;
      g.time = (int)gpsdata->fix.time;
      g.sat = gpsdata->satellites_used;
      g.epx = gpsdata->fix.epx;
      g.epy = gpsdata->fix.epy;
      add_deque(g_deque, g, buff_size);
      
      // printf("Latitude: %f\tLongitude: %f\tTime: %d\n", gpsdata->fix.latitude, gpsdata->fix.longitude, (int)gpsdata->fix.time);

    }
    // If gpsd has no new data, or data is invalid, sleep to spare cpu
    else
    {
      usleep(s_time);
    }
  }

  // Close
  close(gpsdata);

  return 0;
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

void gpspoller::add_deque(deque<gps_struct>& d, gps_struct& g, 
			  unsigned int& s)
{
  // Add to deque if valid
  if( isValidPoint(d, g) )
  {
    d.push_front(g);
    if( d.size() > s )
      d.pop_back();
  }
}

bool gpspoller::isValidPoint(deque<gps_struct>& d, gps_struct& g)
{
  // Filter out corrupt latitude/longitude/time
  if(g.lat < -90 || g.lat > 90 || g.lon < -180 || g.lon > 180 )
    return false;
  if(g.time == 0 || g.sat == 0)
    return false;

  // If no previous records, skip rest
  if( d.size() == 0 )
    return true;

  // Get previous record
  gps_struct& g2 = d.front();

  // Distance between records
  double dist = mathutil::getDistHaver( g.lat, g.lon,
				     g2.lat, g2.lon) * 1000;

  // Time difference between records
  int secDiff = ( g.time - g2.time );

  // Skip records arriving too fast
  if( secDiff < buff_skip_min_sec )
    return false;

  // Record too far away
  if( dist >= buff_skip_dist_m )
  {
    buff_skip_dist_count++;
    if( buff_skip_dist_count > buff_skip_dist_max )
    {
      cerr << "Retrieved too many continuous distant points. "\
	"Treating point as valid." << endl;
      buff_skip_dist_count = 0;
      return true;
    }
    else
    {
      cerr << "Retrieved a too distant point, treating it as an error,"\
	" m = " << dist << endl;
      return false;
    }
  }
  // Remember to decrease nr of distant points
  else if( buff_skip_dist_count > 0 )
    buff_skip_dist_count--;

  return true;
}
