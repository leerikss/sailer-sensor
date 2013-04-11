#include "sailersensor.h"
#include <iostream>
#include <cstdlib>
#include <libconfig.h++>

#define CONFIG_FILE "sailersensor.cfg"

using namespace libconfig;

int main() 
{
  // Read the config file
  Config cfg;

  try { cfg.readFile(CONFIG_FILE);  }
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

  sailersensor ss = sailersensor(cfg);
  ss.run();

  return EXIT_SUCCESS;
}

sailersensor::sailersensor(const Config &cfg)
{
  
}

sailersensor::run()
{
  printf("Started");
}
