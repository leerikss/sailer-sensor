#ifndef mathutil_h
#define mathutil_h

#include<math.h>

using namespace std;

#define EARTH_RADIUS_KM 6371
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class mathutil
{
public:
  static double getDistHaver(const double& lat1, const double& lon1,
				 const double& lat2, const double& lon2);
  static double toRadian(const double& v);
private:
};

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

double mathutil::toRadian(const double& v)
{
  return v * M_PI / 180;
}

#endif
