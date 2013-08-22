#ifndef gpspoller_h
#define gpspoller_h

#include <libconfig.h++>
#include <gps.h>
#include <deque>
#include "structs.h"
#include "log.h"

#define MS_TO_KNOT    1.94384449244;

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
  // Public access
  const gps getData(void);
  const bool isHalted(void);
  
  bool running;

private:
  double getHeading(deque<gps>& gs);
  double getSpeedInKnots(deque<gps>& gs);
  void setBounds(void);
  line getLine(void);
  void open(gps_data_t* gpsdata);
  void close(gps_data_t* gpsdata);
  void add_deque(gps& g);
  bool isValidPoint(gps& g);

  log logger;
  gps_data_t gpsdata_t;
  deque<gps> g_deque;
  int buff_skip_dist_count;

  int s_time;
  bound bnd;

  // Config vals
  int halt_m;
  unsigned int buff_size;
  int buff_skip_dist_m;
  int buff_skip_dist_max;
  int buff_skip_min_sec;
};


#endif
