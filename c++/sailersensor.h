#ifndef sailersensor_h
#define sailersensor_h
#include <libconfig.h++>
#include "lsmpoller.h"

using namespace libconfig;

class sailersensor
{
 public:
  sailersensor(const Config& cfg);

  void run(void);

 private:
  lsmpoller lsmp;
  int stime;
};

#endif
