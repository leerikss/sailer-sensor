#include "lsmpoller.h"
#include <libconfig.h++>
#include <iostream>

using namespace std;

lsmpoller::lsmpoller() {}

void lsmpoller::init(libconfig::Config& cfg)
{
  int z = cfg.lookup("magnetometer.max.z");
  cout << "max.z = " << z << endl;
}
