#include "sailersensor.h"
#include <iostream>
#include <cstdlib>
#include <libconfig.h++>
#include <boost/thread.hpp>
#include "lsmpoller.h"
#include <unistd.h>

#define CONFIG_FILE "sailersensor.cfg"

using namespace libconfig;
using namespace std;

int main() 
{
  try
  {
    sailersensor ss;

    ss.init();
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

void sailersensor::init(void)
{
  // Read configs
  cfg.readFile(CONFIG_FILE);
  stime = cfg.lookup("main_sleep");

  // Init lsm303dlhc poller
  lsmp.init(cfg);

  // TODO: init gpspoller
}

void sailersensor::run(void)
{
  // Start lsmpoller thread
  boost::thread lsmt = boost::thread(&lsmpoller::run, &lsmp);

  while(true)
  {
    // TODO: More stuff
    int p = lsmp.get_pitch();
    int h = lsmp.get_heading();
    cout << "Pitch: " << p << ", Heading: " << h << endl;

    usleep(stime);
  }  
}
