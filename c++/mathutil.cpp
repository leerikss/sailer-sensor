#include "mathutil.h"
#include <math.h>
#include <stdio.h>
#include "structs.h"
#include <deque>

#define MS_TO_KNOT    1.94384449244;

using namespace std;

double mathutil::getDistHaver(const double& lat1, const double& lon1,
				 const double& lat2, const double& lon2)
{
  double dLat = toRadian( lat2 - lat1 );
  double dLon = toRadian( lon2 - lon1 );
  double dLat1 = toRadian( lat1 );
  double dLat2 = toRadian( lat2 );

  double a = sin(dLat/2) * sin(dLat/2) +
    cos(dLat1) * cos(dLat2) * 
    sin(dLon/2) * sin(dLon/2);

  double c = 2 * atan2(sqrt(a), sqrt(1-a));
  double d = EARTH_RADIUS_KM * c;
  
  return d;  
}

double mathutil::getBearing(const double& lat1, const double& lon1,
				 const double& lat2, const double& lon2)
{
  double dLat1 = toRadian(lat1);
  double dLat2 = toRadian(lat2);
  double dLon = toRadian(lon2-lon1);

  double y = sin(dLon) * cos(dLat2);
  double x = cos(dLat1)*sin(dLat2)-sin(dLat1)*cos(dLat2)*cos(dLon);

  double b = toBearing( atan2(y,x) );
  return b;
}

double mathutil::getHeading(deque<gps>& gs)
{
  line l = getLine(gs);
  return getBearing(l.p1.y, l.p1.x, l.p2.y, l.p2.x);
}

double mathutil::getSpeedInKnots(deque<gps>& gs)
{
  // Get distance in meters
  line l = getLine(gs);
  double meters = getDistHaver( l.p1.y, l.p1.x, l.p2.y, l.p2.x ) * 1000;
  // Get time
  int stime = gs.at(0).time;
  int etime = gs.at( gs.size() ).time;
  int time = etime - stime;
  // Get knots
  double ms = ( meters / (double)time ) * 1000.0;
  double kn =  ms * MS_TO_KNOT;
  return kn;
}

line mathutil::getLine(const deque<gps>& gs)
{
  bound b = getBounds(gs);
  double w = getDistHaver( b.min.y, b.min.x, b.min.y, b.max.x);
  double h = getDistHaver( b.min.y, b.min.x, b.max.y, b.min.x);

  // Build deque of points dependent on axis we're moving in
  deque<point> ps;
  for(int i=0;i<(int)gs.size(); i++)
  {
    gps g = gs.at(i);
    point p;

    if( w > h )
    {
      p.x = g.lon; p.y = g.lat;
    }
    else
    {
      p.x = g.lat; p.y = g.lon;
    }

    ps.push_back( p );
  }

  // Return line, flip other way around when heading in x-axis
  line l1 = getLinReg(ps);

  if( w > h )
  {
    line l2;
    l2.p1.x = l1.p1.y; l2.p1.y = l1.p1.x;
    l2.p2.x = l1.p2.y; l2.p2.y = l1.p2.x;
    return l2;
  }
  else
    return l1;
}

bound mathutil::getBounds(const deque<gps>& gs)
{
  bound b;
  b.min.x=181; b.max.x=-181;
  b.min.y=91; b.max.y=-91;
  for(int i=0; i<(int)gs.size(); i++)
  {
    gps g = gs.at(i);
    b.min.x = min(g.lon,b.min.x);
    b.min.y = min(g.lat,b.min.y);
    b.max.x = max(g.lon,b.max.x);
    b.max.y = max(g.lat,b.max.y);
  }
  return b;
}

line mathutil::getLinReg(const deque<point>& points)
{
  // Init variables
  double sum_x=0,sum_y=0,sum_xy=0,sum_xx=0,sum_yy=0,x=0,y=0;
  int count = 0; int len = points.size();

  // Calculate the sum for each of the parts necessary.
  for (int i = 0; i < len; i++) 
  {
    x = points.at(i).x;
    y = points.at(i).y;
    sum_x += x;
    sum_y += y;
    sum_xx += x*x;
    sum_xy += x*y;
    sum_yy += y*y;
    count++;
  }

  double b = (count * sum_xy - sum_x * sum_y) / (count * sum_xx - sum_x * sum_x);
  double a = (sum_y / count) - (b * sum_x) / count;

  // Build result
  line l;
  l.p1.x = points.at(0).x;
  l.p1.y = l.p1.x * b + a;
  l.p2.x = points.at(len-1).x;
  l.p2.y = l.p2.x * b + a;
  return l;
}

double mathutil::toRadian(const double& v)
{
  return v * M_PI /180;
}

double mathutil::toDegree(const double& v)
{
  return v * 180 / M_PI;
}

double mathutil::toBearing(const double& v)
{
  return fmod( ( toDegree(v) + 360 ), 360 );
}
