#ifndef UTILS_H
#define UTILS_H

#include <QtMath>

static const double  a = 6378245.0;
static const double  ee = 0.00669342162296594323;
static const double  x_pi = M_PI * 3000.0 / 180.0;

class Utils
{
public:

    // GPS原始数据度分转度
    static double dms2d(double value);

};

#endif // UTILS_H
