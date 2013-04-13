#ifndef gpspoller_h
#define gpspoller_h

#include <libconfig.h++>
#include <gps.h>

using namespace std;

class gpspoller
{
 public:
  gpspoller(const libconfig::Config& c);

  void run(void);

  bool running;

 private:
  int s_time;
  gps_data_t *gpsdata;
};


#endif
