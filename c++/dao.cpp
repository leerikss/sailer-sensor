#include "dao.h"

#include <sqlite3.h>
#include <iostream>

#define DB          "data/sailorlog.sqlite"

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

  exec(sql);
}

void dao::insertGps()
{
  open();
}

void dao::exec(const char* sql)
{
  char *none = 0; 
  sqlite3_exec(db,sql,NULL,NULL,&none);
}

// Class is singleton so this probably won't be needed
void dao::close(void)
{
  sqlite3_close(db);
}
