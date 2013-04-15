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
  void open(gps_data_t* gpsdata);
  void close(gps_data_t* gpsdata);
  void reg(gps_data_t* gpsdata);

  int s_time;
};


#endif
