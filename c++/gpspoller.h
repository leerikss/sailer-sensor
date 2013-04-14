#ifndef gpspoller_h
#define gpspoller_h

#include <libconfig.h++>
#include <deque>

using namespace std;

class gpspoller
{
 public:
  gpspoller(const libconfig::Config& c);

  void run(void);

  bool running;

 private:
  int s_time;
};

#endif
