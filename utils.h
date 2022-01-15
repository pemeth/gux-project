#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

#include <gtk/gtk.h>

#include "bedit.h"
#include "bezier.h"

/**
 * Check whether a click is in the vicinity of a start/end/control point.
 * The point and its respective BezierCurveNode is returned through the last
 * two arguments. If the point is not found, these are set to NULL.
 *
 * @param list list of bezier curves.
 * @param p the point to be checked.
 * @param m maximum margin around the real point, where the click is valid.
 * @param foundPoint a double pointer,
 *  through which the found point will be returned. NULL if point is not found.
 * @param foundBezierCurve a double pointer, through which the Bezier curve,
 *  to which the found point belongs, is returned. NULL if point is not found.
 */
void is_click_on_bezier(BezierCurveList *list, Point p, double m, Point **foundPoint, BezierCurveNode **foundBezierCurve);

/**
 * When the user is adding a new bezier line, show the points he already defined.
 *
 * @param cr pointer to the relevant cairo rendering device.
 * @param step the current step in defining the bezier curve.
 * @param points points defining the bezier curve.
 */
void show_selected_points(cairo_t *cr, uint8_t step, CurvePoints points);

#endif /* UTILS_H */