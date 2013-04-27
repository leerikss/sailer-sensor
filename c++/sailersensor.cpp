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
  
  return EXIT_SUCCESS;
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
  pthread_t lsm_t;
  pthread_create(&lsm_t, NULL, &lsmpoller::startThread, &lsm_p);

  // Start gpspoller thread
  pthread_t gps_t;
  pthread_create(&gps_t, NULL, &gpspoller::startThread, &gps_p);

  // Init socket
  socketclient s;

  // Cache previous values
  int p_gh, p_mh, p_ap;
  double p_gs;

  while(true)
  {
    try
    {
      stringstream msg;

      // Handle GPS
      const gps& g = gps_p.getData();
      bool h = gps_p.isHalted();

      // Store gps data into db
      if( store_data && !h )
	db.insertGps(g);
      
      // Halt message
      if(h)
      {
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

      // Handle lsm303dlhc
      const lsm& l = lsm_p.getData();
      int ap = abs( static_cast<int>(l.a.p) );
      int mh = static_cast<int>(l.m.h);

      // Store lsm data into db
      if( store_data && (ap != p_ap || mh != p_mh) )
	db.insertLsm(l);

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

      // Send to display
      if(s.conn(display_ip,display_port) )
      {
	s.send_data( msg.str() );
	s.close();
      }

      // Debug
      // cout << "Message: " << msg.str() << endl;
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
