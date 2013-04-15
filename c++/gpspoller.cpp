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
}

void gpspoller::reg(gps_data_t* gpsdata)
{
  gps_stream(gpsdata, WATCH_ENABLE | WATCH_JSON, NULL);
}

void gpspoller::close(gps_data_t* gpsdata)
{
  gps_close(gpsdata);
}

void gpspoller::run(void)
{
// gpsdata buffer
  gps_data_t gpsdata_t;
  gps_data_t* gpsdata = &gpsdata_t;

// Connect to device
  open(gpsdata);

  // Register for updates
  reg(gpsdata);

  running = true;
  while(running)
  {
//block for up to .5 seconds
    if (gps_waiting(gpsdata, 500))
    {
      if( gps_read(gpsdata) != -1 && gpsdata->status>0 )
	printf("Latitude = %f, longitud = %f\n", gpsdata_t.fix.latitude, gpsdata_t.fix.longitude);
    }
// If waiting timeouted, register again
    else
      reg(gpsdata);

    // Sleep
    usleep(s_time);
  }
}
