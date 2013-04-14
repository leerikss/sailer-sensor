#ifndef sailersensor_h
#define sailersensor_h
#include <libconfig.h++>
#include "lsmpoller.h"
#include "gpspoller.h"

using namespace libconfig;

class sailersensor
{
 public:
  sailersensor(const Config& cfg);

  void run(void);

 private:
  lsmpoller lsm_p;
  gpspoller gps_p;
  int s_time;
};

#endif
