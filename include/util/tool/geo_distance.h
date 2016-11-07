/**
 * @Copyright 2010 Ganji Inc.
 * @file    src/ganji/util/tool/geo_distance.h
 * @namespace ganji::util::tool
 * @version 1.0
 * @author  zhangjiafa
 * @date    2011-08-04
 *
 * Calc geo distance
 *
 * Change Log:
 *
 */
#ifndef _GANJI_UTIL_TOOL_GEO_DISTANCE_H_
#define _GANJI_UTIL_TOOL_GEO_DISTANCE_H_
#include <math.h>

namespace ganji { namespace util { namespace tool {
namespace geo_distance {
  static const double kEarthRadius = 6378.137; //地球半径
  static const double kPI = 3.141592653;
  inline double Rad(double d) {
    return d * kPI / 180.0;
  }

  double GetDistance(double lat1, double lng1, double lat2, double lng2) {
    double rad_lat1 = Rad(lat1);
    double rad_lat2 = Rad(lat2);
    double a = rad_lat1 - rad_lat2;
    double b = Rad(lng1) - Rad(lng2);
    double s = 2 * asin(sqrt(pow(sin(a / 2), 2) + cos(rad_lat1) * cos(rad_lat2) * pow(sin(b / 2), 2)));
    s *= kEarthRadius;
    return round(s * 100000) / 100.0;
  }
}
} } }
#endif   ///< _GANJI_UTIL_TOOL_GEO_DISTANCE_H_
