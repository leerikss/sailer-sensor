#ifndef gpspoller_h
#define gpspoller_h

#include <libconfig.h++>
#include <gps.h>
#include <deque>
#include "structs.h"

using namespace std;

class gpspoller
{
public:
  gpspoller(const libconfig::Config& c);
  ~gpspoller();

  // Thread methods
  static void* startThread(void* context)
  {
    return ((gpspoller*)context)->run();
  }
  void* run(void);

  const gps& getLatestPos(void);
  const float getHeading(void);
  const float getSpeedInKnots(void);
  
  bool running;

private:
  void open(gps_data_t* gpsdata);
  void close(gps_data_t* gpsdata);
  void add_deque(deque<gps>& d, gps& g, unsigned int& s);
  bool isValidPoint(deque<gps>& d, gps& g);

  gps_data_t gpsdata_t;
  deque<gps> g_deque;
  int buff_skip_dist_count;

  int s_time;
  unsigned int buff_size;
  int buff_skip_dist_m;
  int buff_skip_dist_max;
  int buff_skip_min_sec;
};


#endif
