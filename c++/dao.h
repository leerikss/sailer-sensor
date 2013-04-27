#ifndef dao_h
#define dao_h

#include <sqlite3.h>
#include "structs.h"
#include <libconfig.h++>

#define SQL_CREATE_TABLES "CREATE TABLE IF NOT EXISTS gps ("	\
  "id INTEGER PRIMARY KEY,"					\
  "time INTEGER DEFAULT CURRENT_TIMESTAMP,"			\
  "sat_time INTEGER,"						\
  "latitude REAL,"						\
  "longitude REAL,"						\
  "altitude REAL,"						\
  "satellites INTEGER,"						\
  "epx REAL,"							\
  "epy REAL,"							\
  "dist REAL,"							\
  "head REAL,"							\
  "knots REAL);"						\
  "CREATE TABLE IF NOT EXISTS lsm303dlhc ("			\
  "id INTEGER PRIMARY KEY,"					\
  "time INTEGER DEFAULT CURRENT_TIMESTAMP,"			\
  "mag_x INTEGER,"						\
  "mag_y INTEGER,"						\
  "mag_z INTEGER,"						\
  "mag_h REAL,"							\
  "acc_x INTEGER,"						\
  "acc_y INTEGER,"						\
  "acc_z INTEGER,"						\
  "acc_p REAL);"

#define  SQL_INSERT_GPS  "INSERT INTO gps(sat_time,latitude,"	\
  "longitude,altitude,satellites,epx,epy,dist,head,knots) "	\
  "VALUES (%d,%f,%f,%f,%d,%f,%f,%f,%f,%f)";

#define SQL_INSERT_LSM    "INSERT INTO lsm303dlhc(mag_x,mag_y,mag_z,mag_h," \
  "acc_x,acc_y,acc_z,acc_p) VALUES (%d,%d,%d,%f,%d,%d,%d,%f)";

class dao
{
public:
  dao(const libconfig::Config& c);
  ~dao();

  bool insertGps(const gps& data);
  bool insertLsm(const lsm& l);

private:
  void open(void);
  void close(void);
  bool query(const char* sql);
  const char* db_file;
  sqlite3* db;
};

#endif
