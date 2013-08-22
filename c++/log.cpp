#include "log.h"

#include <iostream>
#include <iomanip>  
#include <libconfig.h++>

using namespace libconfig;
using namespace std;

#define LOG_DEBUG 0
#define LOG_WARN  1
#define LOG_ERROR 2

log::log(const Config& cfg)
{    
  level = cfg.lookup("log.level");
}

void log::debug(const string& msg)
{
  if(level>LOG_DEBUG) return;
  outTime();
  cout << "[DEBUG] " << msg << endl;
}

void log::warn(const string& msg)
{
  if(level>LOG_WARN) return;
  outTime();
  cout << "[WARN] " << msg << endl;
}


void log::error(const string& msg)
{
  if(level>LOG_ERROR) return;
  outTime();
  cout << "[ERROR] " << msg << endl;
}

void log::outTime(void)
{
    time_t now;
    struct tm *t;
    now = time(0);
    t = localtime(&now);
    cout << "[";
    cout << setfill('0') << setw(2) << t->tm_mday <<  ".";
    cout << setfill('0') << setw(2) << t->tm_mon+1 << ".";
    cout << (t->tm_year+1900) << " ";
    cout << setfill('0') << setw(2) << t->tm_hour << ":";
    cout << setfill('0') << setw(2) << t->tm_min << ":";
    cout << setfill('0') << setw(2) << t->tm_sec << "] ";
}
