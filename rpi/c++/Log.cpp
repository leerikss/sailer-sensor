#include "Log.h"
#include <iostream>
#include <syslog.h>

using namespace std;

void Log::open(void)
{
  openlog("sailersensord", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_USER);    
}

void Log::setLevel(int level)
{
  setlogmask (LOG_UPTO (level));
}

void Log::close(void)
{
  closelog();  
}

void Log::debug(const string& msg)
{
  syslog(LOG_DEBUG,"[debug] %s",msg.c_str());
}

void Log::info(const string& msg)
{
  syslog(LOG_INFO,"[info] %s",msg.c_str());
}

void Log::warn(const string& msg)
{
  syslog(LOG_WARNING,"[warn] %s",msg.c_str());
}

void Log::error(const string& msg)
{
  syslog(LOG_ERR,"[error] %s",msg.c_str());
}
