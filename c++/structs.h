#ifndef structs_h
#define structs_h

struct lsm
{
  int mag_x, mag_y, mag_z, mag_h,acc_x, acc_z, acc_y;
};

struct gps
{
  double lat,lon,alt,epx,epy,dist,head,knots;
  int time, sat;
};

struct point
{
  double x,y;
};

struct line
{
  point p1,p2;
};

struct bound
{
  point min,max;
};

#endif
