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

sailersensor::sailersensor(const Config& cfg) : lsmp(cfg)
{
  stime = cfg.lookup("main_sleep");
}

void sailersensor::run(void)
{
  // Start lsmpoller thread
  boost::thread lsmt = boost::thread(&lsmpoller::run, &lsmp);

  while(true)
  {
    int p = lsmp.get_pitch();
    int h = lsmp.get_heading();

    // TODO: More stuff
    cout << "Pitch: " << p << ", Heading: " << h << endl;

    usleep(stime);
  }  
}
