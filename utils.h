#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

#include <gtk/gtk.h>

#include "bedit.h"
#include "bezier.h"

/**
 * Check whether a click is in the vicinity of a start/end/control point.
 *
 * @param list list of bezier curves.
 * @param p the point to be checked.
 * @param m maximum margin around the real point, where the click is valid.
 * @returns Pointer to the curve, for which the point was found, NULL otherwise.
 */
BezierCurveNode* is_click_on_bezier(BezierCurveList *list, Point p, double m);

/**
 * When the user is adding a new bezier line, show the points he already defined.
 *
 * @param cr pointer to the relevant cairo rendering device.
 * @param step the current step in defining the bezier curve.
 * @param points points defining the bezier curve.
 */
void show_selected_points(cairo_t *cr, uint8_t step, CurvePoints points);

#endif /* UTILS_H */