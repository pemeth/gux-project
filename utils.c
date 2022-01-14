#include "utils.h"

uint8_t is_point_on_bezier(BezierCurveList *list, Point p, double m)
{
    /* BezierCurveNode *curr = list->root;

    while (curr) {
        // TODO finish this (also, i'm only checking for less than or equal, need to check grater than or equal)
        if ((curr->c1.x - m <= p.x && curr->c1.y - m <= p.y) ||
            (curr->c2.x - m <= p.x && curr->c2.y - m <= p.y))

        curr = curr->next;
    } */

    return FALSE;
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