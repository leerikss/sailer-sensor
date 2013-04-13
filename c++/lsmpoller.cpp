#include "lsmpoller.h"
#include <libconfig.h++>
#include <iostream>
#include <unistd.h>
#include <deque>
#include "lsm303dlhc/lsm303dlhc.h"

#define  DEV "/dev/i2c-1";

using namespace std;

lsmpoller::lsmpoller() {}

void lsmpoller::init(libconfig::Config& cfg)
{
  // Set vals from config
  stime = cfg.lookup("lsm_p_sleep");
  p_size = cfg.lookup("accelerometer.buffer_size");
  h_size = cfg.lookup("magnetometer.buffer_size");

  // TODO: FIx this, not working

  // Init device
  const char *fileN = DEV;
  sensor = lsm303dlhc(fileN, cfg);
}

void lsmpoller::run(void)
{
  running = true;

  sensor.enable();

  while(running)
  {
    // Read pitch & add to buffer
    int p = sensor.pitch();
    add_deque(p_deque, p, p_size);

    // Read heading & add to buffer
    int h = sensor.heading();
    add_deque(h_deque, h, h_size);

    // Sleep
    usleep(stime);
  }
}

int lsmpoller::get_pitch(void)
{
  return get_avg(p_deque);
}

int lsmpoller::get_heading(void)
{
  return get_avg(h_deque);
}

float lsmpoller::get_avg(deque<int>& dq)
{
  float avg = 0.0;
  for(int unsigned i=0; i<dq.size(); i++)
  {
    // cout << dq[i] << "+";
    avg += dq[i];
  }
  if( avg > 0.0 )
  {
    // cout << "= > avg: " << avg/dq.size() << endl;
    return avg/dq.size();
  }
  else 
    return 0;
}

void lsmpoller::add_deque(deque<int>& dq, int& v, unsigned int& s)
{
  dq.push_front(v);
  if( dq.size() > s )
    dq.pop_back();
}
