#ifndef lsmpoller_h
#define lsmpoller_h

#include <libconfig.h++>
#include <deque>
#include "lsm303dlhc/lsm303dlhc.h"
#include "structs.h"
#include "log.h"

using namespace libconfig;
using namespace std;

class lsmpoller
{
public:
  lsmpoller(const Config& c);

  void* run(void);
  static void* startThread(void* context)
  {
    return ((lsmpoller*)context)->run();
  }

  const lsm getData(void);

  bool running;

private:
  double getPitch(void);
  double getHeading(void);
  void add_m_deque(mag& m);
  void add_a_deque(acc& a);

  log logger;
  lsm303dlhc lsm303;
  int s_time;
  deque<mag> m_deque;
  unsigned int m_size;
  deque<acc> a_deque;
  unsigned int a_size;

};

#endif
