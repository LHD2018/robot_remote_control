#include "utils.h"

double Utils::dms2d(double value){
    if(value < 1000) return value;
    int d1 = value /100;
    double d = (value - d1*100) / 60.0 + d1;

    return d;
}
