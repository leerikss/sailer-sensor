#include "mathutil.h"
#include <math.h>
#include <stdio.h>
#include "structs.h"
#include <deque>

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

double mathutil::toBearing(const double& v)
{
  return fmod( ( toDegree(v) + 360 ), 360 );
}

double mathutil::toRadian(const double& v)
{
  return v * M_PI /180;
}

double mathutil::toDegree(const double& v)
{
  return v * 180 / M_PI;
}

