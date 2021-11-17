#ifndef BEZIERPOINT_H
#define BEZIERPOINT_H

#ifndef DEBUG
#include <gtk/gtk.h>
#endif

typedef struct Point
{
    #ifndef DEBUG
    gdouble x;
    gdouble y;
    #else
    double x;
    double y;
    #endif
} Point;

typedef struct BezierCurveNode
{
    Point start;
    Point c1;
    Point c2;
    Point end;
    struct BezierCurveNode *next;
} BezierCurveNode;

typedef struct BezierCurveList
{
    struct BezierCurveNode *root;
    struct BezierCurveNode *last;
} BezierCurveList;

void delete_list(BezierCurveList *list);
void init_list(BezierCurveList *list);
BezierCurveNode* add_node(
    BezierCurveList *list,
    Point start,
    Point c1,
    Point c2,
    Point end
);

#endif /* BEZIERPOINT_H */
