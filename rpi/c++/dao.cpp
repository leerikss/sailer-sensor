#include "dao.h"

#include <iostream>
#include "Log.h"

using namespace std;

dao::dao(const libconfig::Config& cfg)
{
  db_file = cfg.lookup("config.sailersensor.db_file");

  // Init sql
  sqlCreateTables = SQL_CREATE_TABLES;
  sqlInsertGps = SQL_INSERT_GPS;
  sqlInsertLsm = SQL_INSERT_LSM;

  // Create init tables
  query(sqlCreateTables.c_str());
}

dao::~dao()
{
}

bool dao::open()
{
  if( sqlite3_open(db_file, &db) != SQLITE_OK )
  {
    Log::get().error( string("Failed to open db. ") + sqlite3_errmsg(db) );
    return false;
  }
  return true;
}

bool dao::close(void)
{
  if( sqlite3_close(db) != SQLITE_OK )
  {
    Log::get().error( string("Failed to close db. ") + sqlite3_errmsg(db) );
    return false;
  }
  return true;
}


bool dao::insertGps(const gps &g)
{
  const char *sql = sqlite3_mprintf(sqlInsertGps.c_str(), g.time, g.lat, g.lon, g.alt, \
				g.sat,g.epx, g.epy, g.dist, g.head, g.knots);
  return query( sql );
}

bool dao::insertLsm(const lsm &l)
{
  const char *sql = sqlite3_mprintf(sqlInsertLsm.c_str(), l.m.x, l.m.y, l.m.z, l.m.h, \
				l.a.x, l.a.y, l.a.z, l.a.p);
  return query( sql );
}

bool dao::query(const char *sql)
{
  char* error;
  
  if( !open() )
    return false;
  
  if( sqlite3_exec(db,sql,NULL,NULL,&error) )
  {
    Log::get().error( sqlite3_errmsg(db) + string(" while executing '")+sql+string("'") );
    sqlite3_free(error);

    close();
    return false;
  }

  close();
  return true;
}
