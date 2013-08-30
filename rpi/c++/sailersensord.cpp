#include "sailersensord.h"
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <libconfig.h++>
#include <pthread.h>
#include <unistd.h>
#include "lsmpoller.h"
#include "gpspoller.h"
#include "dao.h"
#include "structs.h"
#include "socketclient.h"
#include <iomanip>
#include <cmath>
#include "mathutil.h"
#include "Log.h"
#include <sys/stat.h>

#define MSG_GPS_HEAD  "GH"
#define MSG_GPS_SPEED "GS"
#define MSG_MAG_HEAD  "MH"
#define MSG_ACC_PITCH "AP"

using namespace libconfig;
using namespace std;

// Kill deamon function
static bool running = false;
static void halt(int signum)
{
  Log::get().info("SIGTERM retrieved, stopping sailersensord...");
  running = false;
}

int main(int argc,char *argv[]) 
{
  Log::get().open();

  // Config passed as argument
  if(argc != 2)
  {
    Log::get().error("sailersensor requires one argument only defining the path to the config file.\n");
    return(EXIT_FAILURE);
  }

  Log::get().info("sailersensord starting.");

  Config cfg;
  try
  {
    // Read configs
    Log::get().info("Reading configs..");
    cfg.readFile(argv[1]);
  }
  catch(const FileIOException &fioex)
  {
    Log::get().error("I/O error while reading config file\n");
    return(EXIT_FAILURE);
  }
  catch(const ParseException &pex)
  {
    string msg = "Config file parse error at ";
    msg += pex.getFile();
    msg += ":";
    msg += pex.getLine();
    msg += " - ";
    msg += pex.getError();
    msg += "\n";
    Log::get().error(msg);
    return EXIT_FAILURE;
  }

  // Set log level from config file
  Log::get().setLevel( cfg.lookup("log.level") );

  // Start process
  Log::get().info("Configs ok. Starting daemon...");

  pid_t pid, sid;

  // Fork the Parent Process
  pid = fork();
  if (pid < 0) { exit(EXIT_FAILURE); }

  //We got a good pid, Close the Parent Process
  if (pid > 0) { exit(EXIT_SUCCESS); }

  //Change File Mask
  umask(0);

  //Create a new Signature Id for our child
  sid = setsid();
  if (sid < 0) { exit(EXIT_FAILURE); }

  //Change Directory
  //If we cant find the directory we exit with failure.
  if ((chdir("/")) < 0) { exit(EXIT_FAILURE); }
  
  //Close Standard File Descriptors
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);

  // Construct sailersensor
  sailersensor ss = sailersensor(cfg);

  // Install our signal handler.  This responds to
  // kill [pid] from the command line
  signal(SIGTERM, halt);

  // Ignore signal  when terminal session is closed.  This keeps our
  // daemon alive even when user closed terminal session
  signal(SIGHUP, SIG_IGN);

  // Start running
  ss.run();

  // Cleanup
  //sailersensor *ssp = &ss;
  //delete ssp;
  Log::get().close();

  Log::get().info("sailersensrod ends.");  

  return EXIT_SUCCESS;
}

sailersensor::~sailersensor()
{
  Log::get().debug("sailersensor desctructor called");

  // lsmpoller *lsmp = &lsm_p;
  // delete lsmp;

  //gpspoller *gpsp = &gps_p;
  //delete gpsp;  
}

sailersensor::sailersensor(const Config& cfg) : db(cfg), lsm_p(cfg), gps_p(cfg), sc(cfg)
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
  int p_gh=-1, p_mh=-1, p_ap=-1;
  double p_gs=-1;

  stringstream msg;
  ostringstream ss;

  running = true;
  while(running)
  {
    try
    {
      Log::get().debug("sailersensor::run() Loop starts");
      
      msg.str("");
      Log::get().debug("Getting GPS data...");

      // Handle GPS
      const gps g = gps_p.getData();
      bool h = gps_p.isHalted();

      // Debug data
      ss.str("");
      ss << "GPS data: lat=" << fixed << setprecision(8) << g.lat;
      ss << ",lon=" << fixed << setprecision(8) << g.lon;
      ss << ",head=" << fixed << setprecision(0) << g.head;
      ss << ",knots=" << fixed << setprecision(1) << g.knots;
      Log::get().debug( ss.str() );
      ss.clear();

      // Store gps data into db
      if( store_data && !h )
      {
	Log::get().debug("Storing GPS data into DB...");
	db.insertGps(g);
      }

      // Halt message
      if(h)
      {
	Log::get().debug("GPS is halted. Sending GH & GS = -1");
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

      Log::get().debug("Getting LSM303DLHC data...");

      // Handle lsm303dlhc
      const lsm l = lsm_p.getData();
      int ap = abs( static_cast<int>(l.a.p) );
      int mh = static_cast<int>(l.m.h);

      // Debug data
      ss.str("");
      ss << "LSM303DLHC data: mag_head=" << fixed << setprecision(0) << l.m.h;
      ss << ",acc_pitch=" << fixed << setprecision(0) << l.a.p;
      Log::get().debug( ss.str() );
      ss.clear();

      // Store lsm data into db
      if( store_data && (ap != p_ap || mh != p_mh) )
      {
	Log::get().debug("Storing LSM303DLHC data into DB...");
	db.insertLsm(l);
      }

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
      Log::get().debug("Message = " + msg.str() );

      // Send via USB
      Log::get().debug("Sending Message to Kindle via USB");
      if(sc.conn(display_usb_ip,display_port) )
      {
	sc.submit( msg.str() );
	sc.disconn();
      }

      // Send via Wlan
      Log::get().debug("Sending Message to Kindle via Wlan");
      if(sc.conn(display_wlan_ip,display_port) )
      {
	sc.submit( msg.str() );
	sc.disconn();
      }

      // Clear message
      msg.clear();

      Log::get().debug("sailersensor::run() loop ends. Sleeping...");
    }
    catch( const std::exception & ex ) 
    {
      ss.clear();
      ss.str("");
      ss << "sailersensor::run(): exception " << ex.what();
      Log::get().error( ss.str() );
    }      
    catch(...)
    {
      Log::get().error( "sailersensor::run(): Unexpected Error" );
    }
    usleep(s_time);
  }  

  // Closing gracefully
  lsm_p.stop();
  gps_p.stop();
  
  // Wait for pollers to stop
  sleep(2);

  Log::get().info("sailersensor run() stopped.");
}
