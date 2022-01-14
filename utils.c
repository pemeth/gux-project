#include "utils.h"

/**
 * Check if the `tested` point is around `center` within `margin`.
 */
uint8_t is_point_in_margin(Point center, Point tested, double margin)
{
    Point v1, v2;

    // Vertices defining the rectangle
    v1.x = center.x - margin; v1.y = center.y - margin;
    v2.x = center.x + margin; v2.y = center.y + margin;

    // v1----------
    // |          |
    // |          |
    // |          |
    // ----------v2

    double D1, D2, D3, D4;
    D1 = tested.x - v1.x;
    D2 = tested.y - v1.y;
    D3 = v2.x - tested.x;
    D4 = v2.y - tested.y;

    if ((D1 >= 0) && (D2 >= 0) && (D3 >= 0) && (D4 >= 0))
        return TRUE;

    return FALSE;
}

BezierCurveNode* is_click_on_bezier(BezierCurveList *list, Point p, double m)
{
    BezierCurveNode *curr = list->root;

    while (curr) {
        if (is_point_in_margin(curr->start, p, m) ||
            is_point_in_margin(curr->c1, p, m) ||
            is_point_in_margin(curr->c2, p, m) ||
            is_point_in_margin(curr->end, p, m)) {
            // Found
            return curr;
        }

        curr = curr->next;
    }

    // Not found
    return NULL;
}


void show_selected_points(cairo_t *cr, uint8_t step, CurvePoints points)
{
    // Add blue starting point
    cairo_set_source_rgb(cr, 0, 0, 255);

    cairo_new_path(cr);
    cairo_move_to(cr, points.start.x - 5, points.start.y - 5);
    cairo_rel_line_to(cr, 10, 10);
    cairo_move_to(cr, points.start.x - 5, points.start.y + 5);
    cairo_rel_line_to(cr, 10, -10);
    cairo_stroke(cr);

    if (step == ADDING_CURVE_CP1) {
        cairo_set_source_rgb(cr, 0, 0, 0);
        return;
    }

    // Add red control point 1
    cairo_set_source_rgb(cr, 255, 0, 0);

    cairo_new_path(cr);
    cairo_move_to(cr, points.c1.x - 5, points.c1.y - 5);
    cairo_rel_line_to(cr, 10, 10);
    cairo_move_to(cr, points.c1.x - 5, points.c1.y + 5);
    cairo_rel_line_to(cr, 10, -10);
    cairo_stroke(cr);

    if (step == ADDING_CURVE_CP2) {
        cairo_set_source_rgb(cr, 0, 0, 0);
        return;
    }

    // Add green control point 2
    cairo_set_source_rgb(cr, 0, 255, 0);

    cairo_new_path(cr);
    cairo_move_to(cr, points.c2.x - 5, points.c2.y - 5);
    cairo_rel_line_to(cr, 10, 10);
    cairo_move_to(cr, points.c2.x - 5, points.c2.y + 5);
    cairo_rel_line_to(cr, 10, -10);
    cairo_stroke(cr);

    cairo_set_source_rgb(cr, 0, 0, 0);
    return;
}