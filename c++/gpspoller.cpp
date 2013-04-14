#include "gpspoller.h"
#include <libconfig.h++>
#include <iostream>
#include <unistd.h>
#include <gps.h>
#include <libgpsmm.h>

using namespace libconfig;
using namespace std;

gpspoller::gpspoller(const Config& cfg)
{
  // Set vals from config
  s_time = cfg.lookup("gpspoller.sleep");
}

void gpspoller::run(void)
{
  gpsmm gps_rec("localhost", DEFAULT_GPSD_PORT);

  if (gps_rec.stream(WATCH_ENABLE|WATCH_JSON) == NULL) {
    cerr << "No GPSD running.\n";
    return 1;
  }

  for (;;) {
    struct gps_data_t* newdata;

    if (!gps_rec.waiting(50000000))
      continue;

    if ((newdata = gps_rec.read()) == NULL) {
      cerr << "Read error.\n";
      return 1;
    } else {
      PROCESS(newdata);
    }
  }


/*
// Init gpsd client
gps_open("localhost", DEFAULT_GPSD_PORT, gpsdata);

//register for updates
gps_stream(gpsdata, WATCH_ENABLE | WATCH_JSON, NULL);

running = true;

while(running)
{
//block for up to .5 seconds
if (gps_waiting(gpsdata, 500))
{
//dunno when this would happen but its an error
if(gps_read(gpsdata)==-1)
{
*/
/*
  fprintf(stderr,"GPSd Error\n");
  gps_stream(gpsdata, WATCH_DISABLE, NULL);
  gps_close(gpsdata);
  return(-1);
  break;
*/
/*
  }
  else
  {
  //status>0 means you have data
  if(gpsdata->status>0)
	{
	  //sometimes if your GPS doesnt have a fix, it sends you data anyways
	  //the values for the fix are NaN. this is a clever way to check for NaN.
	  if(gpsdata->fix.longitude!=gpsdata->fix.longitude || gpsdata->fix.altitude!=gpsdata->fix.altitude)
	  {
*/
	  /*
	    fprintf(stderr,"Could not get a GPS fix.\n");
	    gps_stream(gpsdata, WATCH_DISABLE, NULL);
	    gps_close(gpsdata);
	    return(-1);
	  */
/*
	  }
	  //otherwise you have a legitimate fix!
	  else
	    fprintf(stderr,"\n");
	}
	//if you don't have any data yet, keep waiting for it.
	else
	  fprintf(stderr,".");
      }
    }
    //apparently gps_stream disables itself after a few seconds.. in this case, gps_waiting returns false.
    //we want to re-register for updates and keep looping! we dont have a fix yet.
    else
      gps_stream(gpsdata, WATCH_ENABLE | WATCH_JSON, NULL);
*/
    
    // Sleep
    usleep(s_time);
  }
}
