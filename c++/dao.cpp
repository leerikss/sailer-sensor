#include "dao.h"

#include <sqlite3.h>
#include <iostream>
#include "structs.h"

#define DB               "data/sailorlog.sqlite"

#define  SQL_INSERT_GPS  "INSERT INTO gps(latitude,longitude,altitude) " \
  "VALUES (%f,%f,%f)";

#define SQL_INSERT_LSM    "INSERT INTO lsm303dlhc(mag_x,mag_y,mag_z,"	\
  "acc_x,acc_y,acc_z) VALUES (%f,%f,%f,%f,%f,%f)";

#define SQL_CREATE_TABLES "CREATE TABLE IF NOT EXISTS gps ("	\
  "id INTEGER PRIMARY KEY,"					\
  "time DATETIME DEFAULT CURRENT_TIMESTAMP,"			\
  "latitude real,"						\
  "longitude real,"						\
  "altitude real);"						\
  "CREATE TABLE IF NOT EXISTS lsm303dlhc ("			\
  "id INTEGER PRIMARY KEY,"					\
  "time DATETIME DEFAULT CURRENT_TIMESTAMP,"			\
  "mag_x INTEGER,"						\
  "mag_y INTEGER,"						\
  "mag_z INTEGER,"						\
  "acc_x INTEGER,"						\
  "acc_y INTEGER,"						\
  "acc_z INTEGER);"

using namespace std;

void dao::open(void)
{
  if(!inited)
  {
    // Open up connection
    sqlite3_open(DB,&db);
    // Create tables
    const char* sql = SQL_CREATE_TABLES;
    query(sql);
    // Mark as inited
    inited = true;
  }
}

bool dao::insertGps(const gps_struct& g)
{
  open();
  const char* sql = SQL_INSERT_GPS;
  return query( \
    sqlite3_mprintf(sql, g.latitude,g.longitude,g.altitude) );
}

bool dao::insertLsm(const lsm_struct& l)
{
  open();
  const char* sql = SQL_INSERT_LSM;
  return query( \
    sqlite3_mprintf(sql,l.mag_x,l.mag_y,l.mag_z,l.acc_x,l.acc_y,l.acc_z));
}

bool dao::query(const char* sql)
{
  char* error;
  if( sqlite3_exec(db,sql,NULL,NULL,&error) )
  {
    cerr << "dao::quuery(): Error: " << sqlite3_errmsg(db) \
	 << " while executing '" << sql << "'" << endl;
    sqlite3_free(error);
    return false;
  }
  return true;
}

// Class is singleton so this probably won't be needed
void dao::close(void)
{
  sqlite3_close(db);
}
