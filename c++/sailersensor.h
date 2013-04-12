#ifndef sailersensor_h
#define sailersensor_h
#include <libconfig.h++>
#include "lsmpoller.h"

// using namespace libconfig;

class sailersensor
{
 public:
  void init(void);
  void run(void);

 private:
  libconfig::Config cfg;
  lsmpoller lsmp;
};

#endif
