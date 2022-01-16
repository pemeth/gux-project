#include <math.h>
#include <stdint.h>

#include <gtk/gtk.h>

#include "bedit.h"
#include "bezier.h"
#include "utils.h"

// TODO probably move these to a header

typedef struct RuntimeInfo
{
    GtkWidget *canvas;
    BezierCurveList list;
    Point click;
    GtkApplication *app; //!< Pointer to the application.
    uint8_t addingCurveStep;
    CurvePoints lastAddedCurve; //!< Curve that is being (or was last) constructed by the user
    Point *clickedPoint;
    BezierCurveNode *clickedBezier;
    uint8_t flagShowControlPoints;
} RuntimeInfo;

static gboolean canvas_draw(GtkWidget *self, cairo_t *cr, RuntimeInfo *data)
{
    // Set background to white
    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    cairo_paint(cr);

    cairo_set_antialias(cr, CAIRO_ANTIALIAS_DEFAULT);

    // Reset color to black
    cairo_set_source_rgb(cr, 0, 0, 0);

    if (data->addingCurveStep != NOT_ADDING_CURVE) {
        show_selected_points(cr, data->addingCurveStep, data->lastAddedCurve);
    }

    // Render all saved curves
    BezierCurveNode *curr = data->list.root;
    double cpRadius = 5.0;
    double curveLineWidth = cairo_get_line_width(cr);
    double tangentLineWidth = 1.0;
    double dashes[] = {2.0};
    while (curr) {
        // The curve
        cairo_new_path(cr);
        cairo_move_to(cr, curr->start.x, curr->start.y);
        cairo_curve_to(
            cr,
            curr->c1.x, curr->c1.y,
            curr->c2.x, curr->c2.y,
            curr->end.x, curr->end.y
        );
        cairo_stroke(cr);

        if (data->flagShowControlPoints) {
            // The tangents to c1 and c2
            cairo_set_dash(cr, dashes, 1, 0.0);
            cairo_set_line_width(cr, tangentLineWidth);

            cairo_new_path(cr);
            cairo_move_to(cr, curr->start.x, curr->start.y);
            cairo_line_to(cr, curr->c1.x, curr->c1.y);
            cairo_stroke(cr);
            cairo_new_path(cr);
            cairo_move_to(cr, curr->end.x, curr->end.y);
            cairo_line_to(cr, curr->c2.x, curr->c2.y);
            cairo_stroke(cr);

            // The endpoints to the tangents
            cairo_arc(cr, curr->c1.x, curr->c1.y, cpRadius, 0.0, 2 * M_PI);
            cairo_stroke(cr);
            cairo_arc(cr, curr->c2.x, curr->c2.y, cpRadius, 0.0, 2 * M_PI);
            cairo_stroke(cr);

            // The start and end of the bezier curve
            cairo_new_path(cr);
            cairo_move_to(cr, curr->start.x - cpRadius, curr->start.y - cpRadius);
            cairo_rel_line_to(cr, cpRadius * 2, 0);
            cairo_rel_line_to(cr, 0, cpRadius * 2);
            cairo_rel_line_to(cr, -cpRadius * 2, 0);
            cairo_close_path(cr);

            cairo_move_to(cr, curr->end.x - cpRadius, curr->end.y - cpRadius);
            cairo_rel_line_to(cr, cpRadius * 2, 0);
            cairo_rel_line_to(cr, 0, cpRadius * 2);
            cairo_rel_line_to(cr, -cpRadius * 2, 0);
            cairo_close_path(cr);

            cairo_stroke(cr);

            // Reset lines
            cairo_set_dash(cr, dashes, 0, 0.0);
            cairo_set_line_width(cr, curveLineWidth);
        }

        curr = curr->next;
    }

    // A demo for modifying a bezier curve by dragging the control point
    /* cairo_new_path(cr);
    cairo_move_to(cr, 250, 200);
    cairo_curve_to(cr, 250, 250, data->click.x, data->click.y, 300, 300);
    cairo_stroke(cr); */

    // Show tangent line of the control point
    /* cairo_new_path(cr);
    cairo_move_to(cr, 300, 300);
    cairo_line_to(cr, data->click.x, data->click.y);
    double dashes[] = {2.0};
    cairo_set_dash(cr, dashes, 1, 0.0);
    cairo_set_line_width(cr, 1.0);
    cairo_stroke(cr); */

    return TRUE;
}

/* Update the runtime information that a curve is being added by the user. */
static void adding_curve(GtkWidget *self, RuntimeInfo *data)
{
    data->addingCurveStep = ADDING_CURVE_START;
}

static gboolean canvas_button_released(GtkWidget *self, GdkEventButton *event, RuntimeInfo *data)
{
    data->click.x = event->x;
    data->click.y = event->y;

    if (data->addingCurveStep == ADDING_CURVE_START) {
        // Adding curve starting point
        data->lastAddedCurve.start = (Point){.x = event->x, .y = event->y};
        data->addingCurveStep = data->addingCurveStep << 1;
    } else if (data->addingCurveStep == ADDING_CURVE_CP1) {
        // Adding first curve control point
        data->lastAddedCurve.c1 = (Point){.x = event->x, .y = event->y};
        data->addingCurveStep = data->addingCurveStep << 1;
    } else if (data->addingCurveStep == ADDING_CURVE_CP2) {
        // Adding second curve control point
        data->lastAddedCurve.c2 = (Point){.x = event->x, .y = event->y};
        data->addingCurveStep = data->addingCurveStep << 1;
    } else if (data->addingCurveStep == ADDING_CURVE_END) {
        // Adding curve endpoint
        data->lastAddedCurve.end = (Point){.x = event->x, .y = event->y};

        // Curve is fully defined - save it
        append_node(
            &(data->list),
            data->lastAddedCurve.start,
            data->lastAddedCurve.c1,
            data->lastAddedCurve.c2,
            data->lastAddedCurve.end
        );

        data->addingCurveStep = data->addingCurveStep << 1; // NOT_ADDING_CURVE
    } else {
        /* TODO error */
    }

    gtk_widget_queue_draw(self);

    return TRUE;
}

static void reset_canvas(GtkWidget* self, RuntimeInfo *data)
{
    delete_list(&(data->list));

    gtk_widget_queue_draw(data->canvas);
}

static gboolean canvas_button_pressed(GtkWidget *self, GdkEventButton *event, RuntimeInfo *data)
{
    data->click.x = event->x;
    data->click.y = event->y;

    // TODO make it so that points are moveable only if a modifier flag is turned on
    is_click_on_bezier(&(data->list), data->click, 10, &(data->clickedPoint), &(data->clickedBezier));

    return TRUE;
}

static gboolean canvas_button_move(GtkWidget* self, GdkEventMotion* event, RuntimeInfo *data)
{
    if ( !(event->state & GDK_BUTTON1_MASK) ) {
        return TRUE;
    }

    data->click.x = event->x;
    data->click.y = event->y;

    // If a point was clicked on it is selected - move it around
    if (data->clickedPoint != NULL) {
        data->clickedPoint->x = data->click.x;
        data->clickedPoint->y = data->click.y;
    }

    gtk_widget_queue_draw(self);

    return TRUE;
}

static void toggle_show_control_points(GtkWidget* self, RuntimeInfo *data)
{
    data->flagShowControlPoints = data->flagShowControlPoints ? FALSE : TRUE;

    gtk_widget_queue_draw(data->canvas);
}

/* Quit the app gracefully. */
static void quit_app(GtkWidget* self, RuntimeInfo *data)
{
    delete_list(&(data->list));
    g_application_quit(G_APPLICATION(data->app));
}

static void activate(GtkApplication *app, RuntimeInfo *data)
{
    GtkWidget *window, *box, *canvas;
    GtkWidget *menubar, *fileMenu, *editMenu;
    /* Menu Items */
    GtkWidget *quitMI, *addPointMI, *showControlPointsMI, *newCanvasMI;
    /* Top Level Menu Items */
    GtkWidget *fileTLMI, *editTLMI;

    GtkAccelGroup *accel_group;

    window = gtk_application_window_new(app);

    gtk_window_set_default_size(GTK_WINDOW(window), 500, 450);
    gtk_window_set_title(GTK_WINDOW(window), "bedit");
    gtk_window_set_resizable(GTK_WINDOW(window), TRUE);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), box);

    /********* MENUS *********/
    /*************************/
    menubar = gtk_menu_bar_new();

    accel_group = gtk_accel_group_new ();
    gtk_window_add_accel_group (GTK_WINDOW (window), accel_group);

    /* Menus */
    fileMenu = gtk_menu_new();
    editMenu = gtk_menu_new();

    /* Menu Items */
    fileTLMI = gtk_menu_item_new_with_label("File");
    editTLMI = gtk_menu_item_new_with_label("Edit");
    quitMI = gtk_menu_item_new_with_label("Quit");
    addPointMI = gtk_menu_item_new_with_label("Add curve");
    showControlPointsMI = gtk_menu_item_new_with_label("Show control points");
    newCanvasMI = gtk_menu_item_new_with_label("New Canvas");
    // TODO add new menu with help

    /* Menu encapsulation */
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(fileTLMI), fileMenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), newCanvasMI);
    gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), gtk_separator_menu_item_new()); // TODO figure out if it is OK to use separators like this
    gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), quitMI);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), fileTLMI);

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(editTLMI), editMenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(editMenu), addPointMI);
    gtk_menu_shell_append(GTK_MENU_SHELL(editMenu), showControlPointsMI);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), editTLMI);

    gtk_box_pack_start(GTK_BOX(box), menubar, FALSE, FALSE, 0);

    /* Menu Item accelerators */
    gtk_widget_add_accelerator(newCanvasMI, "activate", accel_group,
        GDK_KEY_n, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(quitMI, "activate", accel_group,
        GDK_KEY_q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(addPointMI, "activate", accel_group,
        GDK_KEY_a, (GdkModifierType)0, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(showControlPointsMI, "activate", accel_group,
        GDK_KEY_s, (GdkModifierType)0, GTK_ACCEL_VISIBLE);

    /* Menu signals */
    g_signal_connect(G_OBJECT(newCanvasMI), "activate",
        G_CALLBACK(reset_canvas), data);
    g_signal_connect(G_OBJECT(quitMI), "activate",
        G_CALLBACK(quit_app), data);
    g_signal_connect(G_OBJECT(addPointMI), "activate",
        G_CALLBACK(adding_curve), data);
    g_signal_connect(G_OBJECT(showControlPointsMI), "activate",
        G_CALLBACK(toggle_show_control_points), data);

    /********* CANVAS ********/
    /*************************/
    canvas = gtk_drawing_area_new();
    data->canvas = canvas;

    gtk_widget_add_events(
        canvas,
        GDK_BUTTON1_MOTION_MASK |
        GDK_BUTTON_PRESS_MASK   |
        GDK_BUTTON_RELEASE_MASK
    );

    gtk_box_pack_start(GTK_BOX(box), canvas, TRUE, TRUE, 0);

    g_signal_connect(G_OBJECT(canvas), "button-release-event",
        G_CALLBACK(canvas_button_released), data);
    g_signal_connect(G_OBJECT(canvas), "button-press-event",
        G_CALLBACK(canvas_button_pressed), data);
    g_signal_connect(G_OBJECT(canvas), "motion-notify-event",
        G_CALLBACK(canvas_button_move), data);
    g_signal_connect(G_OBJECT(canvas), "draw",
        G_CALLBACK(canvas_draw), data);

    gtk_widget_show_all(window);
}


int main(int argc, char *argv[])
{
    GtkApplication *app;

    RuntimeInfo info;
    init_list(&(info.list));
    info.addingCurveStep = NOT_ADDING_CURVE;
    info.flagShowControlPoints = TRUE;

    app = gtk_application_new("xnemet04.vut.fit.gux.bedit", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), &info);

    // Save the pointer to the app - mainly for quitting through the File menu
    info.app = app;

    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    delete_list(&(info.list));

    return status;
}
