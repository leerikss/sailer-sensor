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

  const gps_struct& getLatestPos(void);
  const float getHeading(void);
  const float getSpeedInKnots(void);
  
  bool running;

private:
  void open(gps_data_t* gpsdata);
  void close(gps_data_t* gpsdata);
  void add_deque(deque<gps_struct>& d, gps_struct& v, unsigned int& s);
  bool isValidPoint(gps_struct& g);

  gps_data_t gpsdata_t;
  int s_time;
  deque<gps_struct> g_deque;
  unsigned int g_size;
};


#endif
