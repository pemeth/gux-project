#include <stdint.h>

#include <gtk/gtk.h>

#include "bedit.h"
#include "bezier.h"
#include "utils.h"

#define M_PI 3.14159265358979323846

#define USAGE_HELP_TEXT "\
\nSHORTCUTS\
\n\t\"A\"\t\tAdd a new curve by defining its four control points.\
\n\t\"D\"\t\tDelete the currently selected curve.\
\n\t\"E\"\t\tToggle edit mode.\
\n\t\"Ctrl+N\"\tCreate a new blank canvas.\
\n\t\"Ctrl+Q\"\tQuit the application.\
\n\
\nCREATING NEW CURVES\
\n\tTo create a new Bezier curve use shortcut \"A\" and define four\
\n\tBezier control points by clicking on the canvas.\
\n\
\nEDIT MODE\
\n\tToggled by \"E\" to show/hide control points, enable curve editing\
\n\tand curve deletion. If control points are visible, edit mode is\
\n\ttoggled ON, if they are not, it is OFF.\
\n\
\nEDITING A CURVE\
\n\tClick and drag any control point. The control points must be\
\n\tvisible (toggle with \"E\" if they are not).\
\n\
\nSELECTING A CURVE\
\n\tTo select a curve, click on any of its control points. The selected \
\n\tcurve's control points are tinted red. To deselect a curve,\
\n\tclick anywhere on the canvas other than a control point.\
\n"

// TODO probably move these to a header

typedef struct RuntimeInfo
{
    GtkWidget *canvas;
    BezierCurveList list;
    Point click;
    GtkApplication *app; //!< Pointer to the application.
    uint8_t addingCurveStep;
    CurvePoints lastAddedCurve; //!< Curve that is being (or was last) constructed by the user
    Point *selectedPoint;
    BezierCurveNode *selectedCurve;
    uint8_t flagEditMode;
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

        // If a curve is selected, show its control points in color
        if (curr == data->selectedCurve)
            cairo_set_source_rgb(cr, 127, 0, 0);

        if (data->flagEditMode) {
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

        cairo_set_source_rgb(cr, 0, 0, 0);
        curr = curr->next;
    }

    return TRUE;
}

/* Update the runtime information that a curve is being added by the user. */
static void adding_curve(GtkWidget *self, RuntimeInfo *data)
{
    data->addingCurveStep = ADDING_CURVE_START;
}

static void delete_curve(GtkWidget *self, RuntimeInfo *data)
{
    if (!data->selectedCurve || data->flagEditMode == FALSE) {
        return;
    }

    remove_curve(&(data->list), data->selectedCurve);
    data->selectedCurve = NULL;
    data->selectedPoint = NULL;

    gtk_widget_queue_draw(data->canvas);
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

    if (data->flagEditMode) {
        is_click_on_bezier(&(data->list), data->click, 10, &(data->selectedPoint), &(data->selectedCurve));
    }

    return TRUE;
}

static gboolean canvas_button_move(GtkWidget* self, GdkEventMotion* event, RuntimeInfo *data)
{
    if ( !(event->state & GDK_BUTTON1_MASK) ) {
        return TRUE;
    }

    data->click.x = event->x;
    data->click.y = event->y;

    // If a point was clicked on, it is selected - move it around
    if (data->selectedPoint != NULL && data->flagEditMode) {
        data->selectedPoint->x = data->click.x;
        data->selectedPoint->y = data->click.y;
    }

    gtk_widget_queue_draw(self);

    return TRUE;
}

static void show_help_widnow(GtkWidget *self, gpointer *data)
{
    GtkWidget *helpWindow, *helpTextView, *box;
    GtkTextBuffer *helpTextBuffer;

    helpTextView = gtk_text_view_new();
    helpTextBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(helpTextView));

    gtk_text_buffer_set_text(helpTextBuffer, USAGE_HELP_TEXT, -1);

    helpWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_window_set_title(GTK_WINDOW(helpWindow), "Usage Help");
    gtk_window_set_default_size(GTK_WINDOW(helpWindow), 320, 200);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(helpWindow), box);

    // Make the helptext read only
    gtk_text_view_set_editable(GTK_TEXT_VIEW(helpTextView), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(helpTextView), FALSE);

    gtk_widget_set_margin_start(helpTextView, 5);
    gtk_widget_set_margin_end(helpTextView, 5);
    gtk_widget_set_margin_top(helpTextView, 5);
    gtk_widget_set_margin_bottom(helpTextView, 5);

    gtk_box_pack_start(GTK_BOX(box), helpTextView, FALSE, FALSE, 0);
    gtk_widget_show_all(helpWindow);
}

static void toggle_show_control_points(GtkWidget* self, RuntimeInfo *data)
{
    data->flagEditMode = data->flagEditMode ? FALSE : TRUE;

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
    GtkWidget *menubar, *fileMenu, *editMenu, *helpMenu;
    /* Menu Items */
    GtkWidget   *quitMI, *addCurveMI, *toggleEditModeMI, *newCanvasMI,
                *usageMI, *deleteCurveMI;
    /* Top Level Menu Items */
    GtkWidget *fileTLMI, *editTLMI, *helpTLMI;

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
    helpMenu = gtk_menu_new();

    /* Menu Items */
    fileTLMI = gtk_menu_item_new_with_mnemonic("_File");
    editTLMI = gtk_menu_item_new_with_mnemonic("_Edit");
    helpTLMI = gtk_menu_item_new_with_mnemonic("_Help");
    quitMI = gtk_menu_item_new_with_mnemonic("_Quit");
    addCurveMI = gtk_menu_item_new_with_mnemonic("_Add Curve");
    toggleEditModeMI = gtk_menu_item_new_with_mnemonic("Toggle _Edit Mode");
    newCanvasMI = gtk_menu_item_new_with_mnemonic("_New Canvas");
    deleteCurveMI = gtk_menu_item_new_with_mnemonic("_Delete Curve");
    usageMI = gtk_menu_item_new_with_mnemonic("_Usage");

    /* Menu encapsulation */
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(fileTLMI), fileMenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), newCanvasMI);
    gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), gtk_separator_menu_item_new()); // TODO figure out if it is OK to use separators like this
    gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), quitMI);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), fileTLMI);

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(editTLMI), editMenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(editMenu), addCurveMI);
    gtk_menu_shell_append(GTK_MENU_SHELL(editMenu), deleteCurveMI);
    gtk_menu_shell_append(GTK_MENU_SHELL(editMenu), toggleEditModeMI);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), editTLMI);

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(helpTLMI), helpMenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(helpMenu), usageMI);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), helpTLMI);

    gtk_box_pack_start(GTK_BOX(box), menubar, FALSE, FALSE, 0);

    /* Menu Item accelerators */
    gtk_widget_add_accelerator(newCanvasMI, "activate", accel_group,
        GDK_KEY_n, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(quitMI, "activate", accel_group,
        GDK_KEY_q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(addCurveMI, "activate", accel_group,
        GDK_KEY_a, (GdkModifierType)0, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(deleteCurveMI, "activate", accel_group,
        GDK_KEY_d, (GdkModifierType)0, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(toggleEditModeMI, "activate", accel_group,
        GDK_KEY_e, (GdkModifierType)0, GTK_ACCEL_VISIBLE);

    /* Menu signals */
    g_signal_connect(G_OBJECT(newCanvasMI), "activate",
        G_CALLBACK(reset_canvas), data);
    g_signal_connect(G_OBJECT(quitMI), "activate",
        G_CALLBACK(quit_app), data);
    g_signal_connect(G_OBJECT(addCurveMI), "activate",
        G_CALLBACK(adding_curve), data);
    g_signal_connect(G_OBJECT(deleteCurveMI), "activate",
        G_CALLBACK(delete_curve), data);
    g_signal_connect(G_OBJECT(toggleEditModeMI), "activate",
        G_CALLBACK(toggle_show_control_points), data);
    g_signal_connect(G_OBJECT(usageMI), "activate",
        G_CALLBACK(show_help_widnow), NULL);

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
    info.flagEditMode = TRUE;

    app = gtk_application_new("xnemet04.vut.fit.gux.bedit", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), &info);

    // Save the pointer to the app - mainly for quitting through the File menu
    info.app = app;

    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    delete_list(&(info.list));

    return status;
}
