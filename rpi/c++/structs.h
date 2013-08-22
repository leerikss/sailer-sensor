#ifndef structs_h
#define structs_h

struct mag
{
  int x,y,z;
  double h;
};

struct acc
{
  int x,y,z;
  double p;
};

struct lsm
{
  mag m;
  acc a;
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
