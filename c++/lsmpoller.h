#ifndef lsmpoller_h
#define lsmpoller_h

#include <libconfig.h++>
#include <deque>
#include "lsm303dlhc/lsm303dlhc.h"

using namespace libconfig;
using namespace std;

class lsmpoller
{
 public:
  lsmpoller(const Config& c);

  void* run(void);
  static void* startRun(void* context)
  {
    return ((lsmpoller*)context)->run();
  }

  int get_pitch(void);
  int get_heading(void);

  bool running;

 private:
  void add_deque(deque<int>& q, int& v, unsigned int& s);
  float get_avg(deque<int>& q);

  lsm303dlhc lsm303;
  int s_time;
  deque<int> p_deque;
  unsigned int p_size;
  deque<int> h_deque;
  unsigned int h_size;

};

#endif
