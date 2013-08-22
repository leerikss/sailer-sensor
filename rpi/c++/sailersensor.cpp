#include "sailersensor.h"
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <libconfig.h++>
#include <pthread.h>
#include <unistd.h>
#include "lsmpoller.h"
#include "dao.h"
#include "structs.h"
#include "socketclient.h"
#include <iomanip>
#include <cmath>
#include "mathutil.h"
#include "log.h"

#define MSG_GPS_HEAD  "GH"
#define MSG_GPS_SPEED "GS"
#define MSG_MAG_HEAD  "MH"
#define MSG_ACC_PITCH "AP"

using namespace libconfig;
using namespace std;

int main(int argc,char *argv[]) 
{
  try
  {
    // Config passed as argument
    if(argc != 2)
    {
      cerr << "sailersensor requires one argument only defining the path to the config file.";
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
    cerr << "I/O error while reading config file";
    return(EXIT_FAILURE);
  }
  catch(const ParseException &pex)
  {
    cerr << "Config file parse error at " << pex.getFile() << ":" << pex.getLine();
    cerr << " - " << pex.getError();
    return(EXIT_FAILURE);
  }
  
  return EXIT_SUCCESS;
}

sailersensor::sailersensor(const Config& cfg) : logger(cfg), db(cfg), lsm_p(cfg), gps_p(cfg), sc(cfg)
{
  s_time = cfg.lookup("sailersensor.sleep");
  display_usb_ip = cfg.lookup("sailersensor.display_usb_ip");
  display_wlan_ip = cfg.lookup("sailersensor.display_wlan_ip");
  display_port = cfg.lookup("sailersensor.display_port");
  store_data = cfg.lookup("sailersensor.store_data");
}

void sailersensor::run(void)
{
  // Start lsmpoller thread
  pthread_t lsm_t;
  pthread_create(&lsm_t, NULL, &lsmpoller::startThread, &lsm_p);

  // Start gpspoller thread
  pthread_t gps_t;
  pthread_create(&gps_t, NULL, &gpspoller::startThread, &gps_p);

  // Cache previous values
  int p_gh, p_mh, p_ap;
  double p_gs;

  while(true)
  {
    try
    {
      logger.debug("sailersensor::run() Loop starts");

      stringstream msg;

      logger.debug("Getting GPS data...");

      // Handle GPS
      const gps& g = gps_p.getData();
      bool h = gps_p.isHalted();

      // Debug data
      ostringstream ss;
      ss << "GPS data: lat=" << fixed << setprecision(8) << g.lat;
      ss << ",lon=" << fixed << setprecision(8) << g.lon;
      ss << ",head=" << fixed << setprecision(0) << g.head;
      ss << ",knots=" << fixed << setprecision(1) << g.knots;
      logger.debug( ss.str() );
	
      // Store gps data into db
      if( store_data && !h )
      {
	logger.debug("Storing GPS data into DB...");
	db.insertGps(g);
      }

      // Halt message
      if(h)
      {
	logger.debug("GPS is halted. Sending GH & GS = -1");
	msg << MSG_GPS_HEAD << "-1 ";
	msg << MSG_GPS_SPEED << "-1 ";
      }
      // Build normal message
      else 
      {
	int gh = static_cast<int>(g.head);
	double gs = mathutil::round(g.knots,1);

	if( gh != p_gh )
	{
	  msg << MSG_GPS_HEAD << gh << " ";
	  p_gh = gh;
	}
	if( gs != p_gs )
	{
	  msg << MSG_GPS_SPEED << fixed << setprecision(1) << gs << " ";
	  p_gs = gs;
	}
      }

      logger.debug("Getting LSM303DLHC data...");

      // Handle lsm303dlhc
      const lsm& l = lsm_p.getData();
      int ap = abs( static_cast<int>(l.a.p) );
      int mh = static_cast<int>(l.m.h);

      // Debug data
      ostringstream ss2;
      ss2 << "LSM303DLHC data: mag_head=" << fixed << setprecision(0) << l.m.h;
      ss2 << ",acc_pitch=" << fixed << setprecision(0) << l.a.p;

      logger.debug( ss2.str() );

      // Store lsm data into db
      if( store_data && (ap != p_ap || mh != p_mh) )
      {
	logger.debug("Storing LSM303DLHC data into DB...");
	db.insertLsm(l);
      }

      // Build message
      if( mh != p_mh )
      {
	msg << MSG_MAG_HEAD << mh << " ";
	p_mh = mh;
      }
      if( ap != p_ap )
      {
	msg << MSG_ACC_PITCH << ap << " ";
	p_ap = ap;
      }

      // Send via USB
      logger.debug("Sending Message to Kindle via USB: " + msg.str() + "...");
      if(sc.conn(display_usb_ip,display_port) )
      {
	sc.send_data( msg.str() );
	sc.close();
      }

      // Send via Wlan
      logger.debug("Sending Message to Kindle via Wlan: " + msg.str() + "...");
      if(sc.conn(display_wlan_ip,display_port) )
      {
	sc.send_data( msg.str() );
	sc.close();
      }

      logger.debug("sailersensor::run() loop ends. Sleeping...");

    }
    catch( const std::exception & ex ) 
    {
      ostringstream ss;
      ss << "sailersensor::run(): exception " << ex.what();
      logger.error( ss.str() );
    }      
    catch(...)
    {
      logger.error( "sailersensor::run(): Unexpected Error" );
    }
    usleep(s_time);
  }  
}
