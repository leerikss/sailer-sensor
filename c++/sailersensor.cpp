#include "sailersensor.h"
#include <iostream>
#include <cstdlib>
#include <libconfig.h++>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include "lsmpoller.h"

#define CONFIG_FILE "sailersensor.cfg"

using namespace libconfig;
using namespace std;

int main() 
{
  // Init
  sailersensor ss;
  try
  {
    ss.init();
  }
  catch(const FileIOException &fioex)
  {
    std::cerr << "I/O error while reading file." << std::endl;
    return(EXIT_FAILURE);
  }
  catch(const ParseException &pex)
  {
    std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
	      << " - " << pex.getError() << std::endl;
    return(EXIT_FAILURE);
  }

  // Run  
  ss.run();
  
  return EXIT_SUCCESS;
}

void sailersensor::init(void)
{
  // Read config
  cfg.readFile(CONFIG_FILE);

  // Construct lsm303dlhc poller
  lsmp.init(cfg);
}

void sailersensor::run(void)
{
}
