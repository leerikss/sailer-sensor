#include "dao.h"

#include <sqlite3.h>
#include <iostream>
#include "structs.h"
#include <libconfig.h++>

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
    sqlite3_mprintf(sql, g.time, g.lat, g.lon, g.alt, g.sat, \
		    g.epx, g.epy, g.dist, g.head, g.knots) );
}

bool dao::insertLsm(const lsm& l)
{
  const char* sql = SQL_INSERT_LSM;

  return query( \
    sqlite3_mprintf(sql, l.m.x, l.m.y, l.m.z, l.m.h,\
		    l.a.x, l.a.y, l.a.z, l.a.p));
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
