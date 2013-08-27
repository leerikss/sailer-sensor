#include <iostream>
#include <syslog.h>

using namespace std;

class Log
{

public:
  static Log& get()
  {
    static Log instance; // Guaranteed to be destroyed.
    // Instantiated on first use.
    return instance;
  }

  void open(void);
  void setLevel(int level);
  void close(void);
  void debug(const string& msg);
  void info(const string& msg);
  void warn(const string& msg);
  void error(const string& msg);

private:
  Log() {};
  Log(Log const&);
  void operator=(Log const&);

};
