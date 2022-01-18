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
    size_t n_nodes;
} BezierCurveList;

void delete_list(BezierCurveList *list);
void remove_nth(BezierCurveList *list, size_t n);
void remove_last(BezierCurveList *list);
void remove_curve(BezierCurveList *list, BezierCurveNode *curve);
void init_list(BezierCurveList *list);
void print_list(BezierCurveList *list);
void print_node(BezierCurveNode *node);
BezierCurveNode* append_node(
    BezierCurveList *list,
    Point start,
    Point c1,
    Point c2,
    Point end
);

#endif /* BEZIERPOINT_H */
