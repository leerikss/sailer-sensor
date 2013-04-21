#ifndef dao_h
#define dao_h

#include <sqlite3.h>
#include "structs.h"
#include <libconfig.h++>

class dao
{
public:
  dao(const libconfig::Config& c);
  ~dao();

  bool insertGps(const gps_struct& data);
  bool insertLsm(const lsm_struct& lsm);

private:
  void open(void);
  void close(void);
  bool query(const char* sql);
  const char* db_file;
  sqlite3* db;
};

#endif
