#include "dao.h"

#include <sqlite3.h>
#include <iostream>
#include "structs.h"

#define DB          "data/sailorlog.sqlite"

using namespace std;

void dao::open(void)
{
  if(!inited)
  {
    sqlite3_open(DB,&db);
    init();
    inited = true;
  }
}

void dao::init(void)
{
  // Create tables
  const char* sql = "CREATE TABLE IF NOT EXISTS gps ("	\
    "id INTEGER PRIMARY KEY,"				\
    "time DATETIME DEFAULT CURRENT_TIMESTAMP,"		\
    "latitude real,"					\
    "longitude real,"					\
    "altitude real);"					\
    "CREATE TABLE IF NOT EXISTS lsm303dlhc ("		\
    "id INTEGER PRIMARY KEY,"				\
    "time DATETIME DEFAULT CURRENT_TIMESTAMP,"		\
    "mag_x INTEGER,"					\
    "mag_y INTEGER,"					\
    "mag_z INTEGER,"					\
    "acc_x INTEGER,"					\
    "acc_y INTEGER,"					\
    "acc_z INTEGER);";

  query(sql);
}

bool dao::insertGps(const gps_struct& g)
{
  open();

  const char* sql = sqlite3_mprintf("INSERT INTO gps(latitude,longitude,altitude) " \
				    "VALUES (%q,%q,%q)", g.latitude,g.longitude,g.altitude);

  return query(sql);
}

bool dao::insertLsm(const lsm_struct& l)
{
  open();

  const char* sql = sqlite3_mprintf("INSERT INTO gps(mag_x,mag_y,mag_z,acc_x,acc_y,acc_z) " \
				    "VALUES (%q,%q,%q,%q,%q,%q)", \ 
				    l.mag_x,l.mag_y,l.mag_z,l.acc_x,l.acc_y,l.acc_z);

  return query(sql);
}

bool dao::query(const char* sql)
{
  char* error;
  if( sqlite3_exec(db,sql,NULL,NULL,&error) )
  {
    cerr << "Error executing " << sqlite3_errmsg(db) << endl;
    return false;
  }
  return true;
}

// Class is singleton so this probably won't be needed
void dao::close(void)
{
  sqlite3_close(db);
}
