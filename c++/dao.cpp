#include "dao.h"

#include <sqlite3.h>
#include <iostream>
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
  "epy REAL);"							\
  "CREATE TABLE IF NOT EXISTS lsm303dlhc ("			\
  "id INTEGER PRIMARY KEY,"					\
  "time INTEGER DEFAULT CURRENT_TIMESTAMP,"			\
  "mag_x INTEGER,"						\
  "mag_y INTEGER,"						\
  "mag_z INTEGER,"						\
  "acc_x INTEGER,"						\
  "acc_y INTEGER,"						\
  "acc_z INTEGER);"

#define  SQL_INSERT_GPS  "INSERT INTO gps(sat_time,latitude," \
  "longitude,altitude,satellites,epx,epy) VALUES (%d,%f,%f,%f,%d,%f,%f)";

#define SQL_INSERT_LSM    "INSERT INTO lsm303dlhc(mag_x,mag_y,mag_z,"	\
  "acc_x,acc_y,acc_z) VALUES (%f,%f,%f,%f,%f,%f)";

using namespace std;

dao::dao(const libconfig::Config& cfg)
{
  db_file = cfg.lookup("sailersensor.db_file");
  // Create init tables
  const char* sql = SQL_CREATE_TABLES;
  query(sql);
}

dao::~dao()
{
  close();
}

void dao::open()
{
  if( sqlite3_open(db_file, &db) != SQLITE_OK )
    cerr << "dao::open(): Sqlite3 open failed" << endl;
}

void dao::close(void)
{
  if( sqlite3_close(db) != SQLITE_OK )
    cerr << "dao::close(): Sqlite3 close failed" << endl;
}


bool dao::insertGps(const gps& g)
{
  const char* sql = SQL_INSERT_GPS;

  return query( \
    sqlite3_mprintf(sql, g.time, g.lat, g.lon, g.alt, g.sat, g.epx, g.epy) );
}

bool dao::insertLsm(const lsm& l)
{
  const char* sql = SQL_INSERT_LSM;

  return query( \
    sqlite3_mprintf(sql,l.mag_x,l.mag_y,l.mag_z,l.acc_x,l.acc_y,l.acc_z));
}

bool dao::query(const char* sql)
{
  char* error;

  open();

  if( sqlite3_exec(db,sql,NULL,NULL,&error) )
  {
    cerr << "dao::quuery(): Error: " << sqlite3_errmsg(db) \
	 << " while executing '" << sql << "'" << endl;
    sqlite3_free(error);

    close();
    return false;
  }
  else
  {
    close();
    return true;
  }
}
