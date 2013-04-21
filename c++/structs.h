#ifndef structs_h
#define structs_h

struct lsm_struct
{
  int mag_x, mag_y, mag_z, acc_x, acc_z, acc_y;
};

struct gps_struct
{
  double lat,lon,alt,epx,epy;
  int time,sat;
};

#endif
