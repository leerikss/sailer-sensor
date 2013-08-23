#include "log.h"

#include <iostream>
#include <iomanip>  
#include <libconfig.h++>
#include <syslog.h>

using namespace libconfig;
using namespace std;

#define LEVEL_DEBUG 0
#define LEVEL_WARN  1
#define LEVEL_ERROR 2

log::log(const Config& cfg)
{    
  level = cfg.lookup("log.level");
  openlog("sailersensor", 0, LOG_USER);
}

void log::debug(const string& msg)
{
  if(level>LEVEL_DEBUG) 
    return;
  syslog(LOG_DEBUG,"[debug] %s",msg.c_str());
}

void log::warn(const string& msg)
{
  if(level>LEVEL_WARN) 
    return;
  syslog(LOG_WARNING,"[warn] %s",msg.c_str());
}


void log::error(const string& msg)
{
  if(level>LEVEL_ERROR) 
    return;
  syslog(LOG_ERR,"[error] %s",msg.c_str());
}

/*
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
*/
