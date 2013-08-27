#include "lsmpoller.h"
#include <libconfig.h++>
#include <iostream>
#include <unistd.h>
#include <deque>
#include "structs.h"
#include "lsm303dlhc/lsm303dlhc.h"
#include "Log.h"

using namespace libconfig;
using namespace std;

const char *fileN = "/dev/i2c-1";

lsmpoller::lsmpoller(const Config& cfg) : lsm303(fileN, cfg)
{
  // Set vals from config
  s_time = cfg.lookup("lsmpoller.sleep");
  a_size = cfg.lookup("accelerometer.buffer_size");
  m_size = cfg.lookup("magnetometer.buffer_size");

  Log::get().debug("lsmpoller constructed");
}

lsmpoller::~lsmpoller()
{
  Log::get().debug("lsmpoller desctructor called");
  // lsm303dlhc *p = &lsm303;
  // delete p;
}

void* lsmpoller::run(void)
{
  lsm303.enable();

  running = true;

  while(running)
  {
    // Read accelerometer
    acc a;
    lsm303.readAccRaw();
    a.x = lsm303.a.x;
    a.y = lsm303.a.y;
    a.z = lsm303.a.z;
    a.p = lsm303.pitch();
    add_a_deque(a);

    // Read magnetometer
    mag m;
    lsm303.readMagRaw();
    m.x = lsm303.m.x;
    m.y = lsm303.m.y;
    m.z = lsm303.m.z;
    m.h = lsm303.heading();
    add_m_deque(m);

    // Sleep
    usleep(s_time);
  }

  Log::get().debug("lsmpoller run() stopped.");
  return 0;
}

void lsmpoller::stop()
{
  running = false;
  Log::get().debug("lsmpoller stopping...");
}

const lsm lsmpoller::getData(void)
{
  lsm l;

  if( a_deque.empty() || m_deque.empty() )
    return l;

  acc a = a_deque.back();
  a.p = getPitch();

  mag m = m_deque.back();
  m.h = getHeading();

  l.a = a;
  l.m = m;
  return l;
}

double lsmpoller::getPitch(void)
{
  double avg = 0.0;
  unsigned int len = a_deque.size();
  for(unsigned int i=0; i<len; i++)
  {
    acc a = a_deque.at(i);
    avg += a.p;
  }
  if(avg == 0)
    return 0;
  return avg/len;
}

double lsmpoller::getHeading(void)
{
  double avg = 0.0;
  unsigned int len = m_deque.size();
  for(unsigned int i=0; i<len; i++)
  {
    mag m = m_deque.at(i);
    avg += m.h;
  }
  if(avg == 0)
    return 0;
  return avg/len;
}

void lsmpoller::add_m_deque(mag& m)
{
  m_deque.push_back(m);
  if( m_deque.size() > m_size )
    m_deque.pop_front();
}

void lsmpoller::add_a_deque(acc& a)
{
  a_deque.push_back(a);
  if( a_deque.size() > a_size )
    a_deque.pop_front();
}
