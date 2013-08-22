#ifndef log_h
#define log_h

#include <libconfig.h++>

#define DEBUG 1
#define LEVEL_ERROR 2

using namespace libconfig;
using namespace std;

class log
{
public:
  log(const Config& c);

  void debug(const string& msg);
  void warn(const string& msg);
  void error(const string& msg);

private:
  void outTime(void);

  int level;
};

#endif
