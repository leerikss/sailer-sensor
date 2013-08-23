#ifndef sailersensor_h
#define sailersensor_h

#include <libconfig.h++>
#include "lsmpoller.h"
#include "gpspoller.h"
#include "structs.h"
#include "dao.h"
#include "socketclient.h"

using namespace libconfig;

class sailersensor
{
public:
  sailersensor(const Config& cfg);
  void run(void);

private:
  dao db;
  lsmpoller lsm_p;
  gpspoller gps_p;
  socketclient sc;

  // Config vals
  int s_time;
  const char* display_usb_ip;
  const char* display_wlan_ip;
  int display_port;
  bool store_data;  
};

#endif
