#include "lsmpoller.h"
#include <libconfig.h++>
#include <iostream>
#include <unistd.h>

using namespace std;

lsmpoller::lsmpoller() {}

void lsmpoller::init(libconfig::Config& cfg)
{
  // TODO
}

void lsmpoller::run(void)
{
  cout << "lmspoller.run() thread..." << endl;
  sleep(2);
}
