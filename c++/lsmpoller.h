#ifndef lsmpoller_h
#define lsmpoller_h
#include <libconfig.h++>
// #include "sailersensor.h"

class lsmpoller
{
 public:
  lsmpoller();
  void init(libconfig::Config& c);
};

#endif
