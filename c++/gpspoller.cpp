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
  halt_m = cfg.lookup("gpspoller.halt_m");
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
      gps g;
      g.lat = gpsdata->fix.latitude;
      g.lon = gpsdata->fix.longitude;
      g.alt = gpsdata->fix.altitude;
      g.time = (int)gpsdata->fix.time;
      g.sat = gpsdata->satellites_used;
      g.epx = gpsdata->fix.epx;
      g.epy = gpsdata->fix.epy;
      add_deque(g);
      
/*
      printf("Latitude: %f\tLongitude: %f\tTime: %d\n", gpsdata->fix.latitude, \
      gpsdata->fix.longitude, (int)gpsdata->fix.time);
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

  return 0;
}

const gps gpspoller::getData(void)
{
  gps g;
  if( g_deque.empty() )
    return g;
  
  g = g_deque.back();
  setBounds();

  line l = getLine();
  
  // Set heading
  double h = mathutil::getBearing(l.p1.x, l.p1.y, l.p2.x, l.p2.y);
  g.head = h;

  // Set distance in meters
  double m = mathutil::getDistHaver( l.p1.x, l.p1.y, l.p2.x, l.p2.y ) * 1000;
  g.dist = m;

  // Get speed
  int st = g_deque.at(0).time;
  int et = g_deque.at( g_deque.size() - 1).time;
  int t = et - st;
  double ms = ( m / t );
  double kn =  ms * MS_TO_KNOT;
  g.knots = kn;

  return g;
}

const bool gpspoller::isHalted(void)
{
  // No data yet = halted
  if( g_deque.size() == 0)
    return true;

  setBounds();
  double m = mathutil::getDistHaver( bnd.min.x, bnd.min.y, bnd.max.x, bnd.max.y ) * 1000;  
  if(m <= (double)halt_m)
    return true;
  return false;
}

void gpspoller::setBounds(void)
{
  bnd.min.x=181; bnd.max.x=-181;
  bnd.min.y=91; bnd.max.y=-91;
  for(unsigned int i=0; i<g_deque.size(); i++)
  {
    gps g = g_deque.at(i);
    bnd.min.x = min(g.lon,bnd.min.x);
    bnd.min.y = min(g.lat,bnd.min.y);
    bnd.max.x = max(g.lon,bnd.max.x);
    bnd.max.y = max(g.lat,bnd.max.y);
  }
}

// TODO: Find out a more elegant way of retrieving the line without 
// flipping the linear regression arguments based on direction
line gpspoller::getLine(void)
{
  double w = mathutil::getDistHaver( bnd.min.y, bnd.min.x, bnd.min.y, bnd.max.x);
  double h = mathutil::getDistHaver( bnd.min.y, bnd.min.x, bnd.max.y, bnd.min.x);

  // Build deque of points dependent on axis we're moving in
  deque<point> ps;
  for(int unsigned i=0;i<g_deque.size(); i++)
  {
    gps g = g_deque.at(i);

    point p;

    if( w > h )
    {
      p.x = g.lon; 
      p.y = g.lat;
    }
    else
    {
      p.x = g.lat; 
      p.y = g.lon;
    }

    ps.push_back( p );
  }

  // Return line, flip other way around when heading in x-axis
  line l1 = mathutil::getLinReg(ps);

  if( w > h )
  {
    line l2;
    l2.p1.x = l1.p1.y; 
    l2.p1.y = l1.p1.x;
    l2.p2.x = l1.p2.y; 
    l2.p2.y = l1.p2.x;
    return l2;
  }
  else
    return l1;
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

void gpspoller::add_deque(gps& g)
{
  // Add to deque if valid
  if( isValidPoint(g) )
  {
    g_deque.push_back(g);
    if( g_deque.size() > buff_size )
      g_deque.pop_front();
  }
}

bool gpspoller::isValidPoint(gps& g)
{
  // Filter out corrupt latitude/longitude/time
  if(g.lat < -90 || g.lat > 90 || g.lon < -180 || g.lon > 180 )
    return false;
  if(g.time == 0 || g.sat == 0)
    return false;

  // If no previous records, skip rest
  if( g_deque.size() == 0 )
    return true;

  // Get previous record
  gps& g2 = g_deque.back();

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
