#ifndef dao_h
#define dao_h

#include <sqlite3.h>
#include "structs.h"

// Singleton class
class dao
{
public:
  static dao& getInstance()
  {
    static dao instance;
    return instance;
  }

  bool insertGps(const gps_struct& data);
  bool insertLsm(const lsm_struct& lsm);

private:
  dao(){};
  dao(dao const&);
  void operator=(dao const&);

  void open(void);
  bool query(const char* sql);
  void close(void);

  sqlite3* db;
  bool inited;
};

#endif
