#ifndef database_h
#define database_h

#include <sqlite3.h>

class database
{
public:
  database(void);

private:
  bool open(void);
  void close(void);

  sqlite3 *db;

};

#endif
