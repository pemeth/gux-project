#ifndef BEDIT_H
#define BEDIT_H

#include "bezier.h"

#define ADDING_CURVE_START 1
#define ADDING_CURVE_CP1   2
#define ADDING_CURVE_CP2   4
#define ADDING_CURVE_END   8
#define NOT_ADDING_CURVE   16

typedef struct CurvePoints
{
    Point start;
    Point c1;
    Point c2;
    Point end;
} CurvePoints;

#endif /* BEDIT_H */