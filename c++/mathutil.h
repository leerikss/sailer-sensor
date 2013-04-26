#ifndef mathutil_h
#define mathutil_h

#include <deque>
#include "structs.h"

#define EARTH_RADIUS_KM 6371
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std;

class mathutil
{
public:
  static double getHeading(deque<gps>& gs);
  static double getSpeedInKnots(deque<gps>& gs);

  static double getDistHaver(const double& lat1, const double& lon1,
				 const double& lat2, const double& lon2);
  static double toRadian(const double& v);
  static double toDegree(const double& v);
  static double toBearing(const double& v);
  static double getBearing(const double& lat1, const double& lon1,
			   const double& lat2, const double& lon2);
  static line getLinReg (const deque<point>& points);
  static bound getBounds(const deque<gps>& gs);
  static line getLine(const deque<gps>& g);
private:
};

#endif
