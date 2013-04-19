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

  void run(void);
  const gps_struct& getLatestPos(void);

  bool running;

 private:
  void open(gps_data_t* gpsdata);
  void close(gps_data_t* gpsdata);
  void add_deque(deque<gps_struct>& d, gps_struct& v, unsigned int& s);

  int s_time;
  deque<gps_struct> g_deque;
  unsigned int g_size;
};


#endif
