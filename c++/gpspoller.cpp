#include "gpspoller.h"
#include <libconfig.h++>
#include <iostream>
#include <unistd.h>
#include <gps.h>
// #include <libgpsmm.h>

using namespace libconfig;
using namespace std;

gpspoller::gpspoller(const Config& cfg)
{
  // Set vals from config
  s_time = cfg.lookup("gpspoller.sleep");
}

void gpspoller::open(gps_data_t* gpsdata)
{
  while( gps_open("localhost", DEFAULT_GPSD_PORT, gpsdata) < 0 )
  {
      cerr << "Unable to connect to device. Trying again..." << endl;
      usleep(s_time);
  }
  reg(gpsdata);
}

void gpspoller::reg(gps_data_t* gpsdata)
{
  gps_stream(gpsdata, WATCH_ENABLE, NULL);
}

void gpspoller::close(gps_data_t* gpsdata)
{
  gps_stream(gpsdata, WATCH_DISABLE, NULL);
  gps_close(gpsdata);
}

void gpspoller::run(void)
{
// gpsdata buffer
  gps_data_t gpsdata_t;
  gps_data_t* gpsdata = &gpsdata_t;

// Connect to device
  open(gpsdata);

  running = true;
  while(running)
  {
    //block to wair for data
    if (gps_waiting(gpsdata, (500)))
    {
      // Data ok if it is read, has a status, and latitude nor 
      // longitude is NaN
      if( ( gps_read(gpsdata) != -1 ) & ( gpsdata->status > 0 ) & 
	  !(gpsdata->fix.latitude  != gpsdata->fix.latitude) &
	  !(gpsdata->fix.longitude != gpsdata->fix.longitude) )
      {
	// TODO: Do stuff
	printf("Latitude = %f, longitude = %f\n", gpsdata->fix.latitude, gpsdata->fix.longitude);
      }
      else
	cerr << "No data retrieved." << endl;
    }
    // Sleep
    usleep(s_time);
  }

  // Close
  close(gpsdata);
}
