#include "gpspoller.h"
#include <libconfig.h++>
#include <iostream>
#include <unistd.h>
#include <gps.h>

using namespace libconfig;
using namespace std;

gpspoller::gpspoller(const Config& cfg)
{
  // Set vals from config
  s_time = cfg.lookup("gpspoller.sleep");
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
      // TODO: Do stuff
      printf("Latitude = %f, longitude = %f\n", gpsdata->fix.latitude, gpsdata->fix.longitude);
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
