#ifndef dao_h
#define dao_h

#include <sqlite3.h>

// Singleton class
class dao
{
public:
  static dao& getInstance()
  {
    static dao instance;
    return instance;
  }

  struct gps
  {
    double latitude,longitude,altitude;
  };

  struct lsm
  {
    int mag_x,mag_y,mag_z,acc_x,acc_y,acc_z;
  };

  void insertGps(void);

private:
  dao(){};
  dao(dao const&);
  void operator=(dao const&);

  void open(void);
  void init(void);
  void exec(const char* sql);
  void close(void);

  sqlite3 *db;
  bool inited;
};

#endif
