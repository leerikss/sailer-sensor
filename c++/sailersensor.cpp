#include "sailersensor.h"
#include <iostream>
#include <cstdlib>
#include <libconfig.h++>
#include <boost/thread.hpp> 
#include <unistd.h>
#include "lsmpoller.h"

#define CONFIG_FILE "sailersensor.cfg"

using namespace libconfig;
using namespace std;

int main() 
{
  try
  {
    // Read configs
    Config cfg;
    cfg.readFile(CONFIG_FILE);
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
  
  // return EXIT_SUCCESS;
}

sailersensor::sailersensor(const Config& cfg) : lsm_p(cfg), gps_p(cfg)
  
{
  s_time = cfg.lookup("sailersensor.sleep");
}

void sailersensor::run(void)
{
  // Start lsmpoller thread
  boost::thread lsm_t = boost::thread(&lsmpoller::run, &lsm_p);

  // Start gpspoller thread
  boost::thread gps_t = boost::thread(&gpspoller::run, &gps_p);

  while(true)
  {
    /*
    int p = lsm_p.get_pitch();
    int h = lsm_p.get_heading();
    cout << "Pitch: " << p << ", Heading: " << h << endl;
    */

    usleep(s_time);
  }  
}
