#include <gtk/gtk.h>

#include "bezier.h"

typedef struct MouseClick
{
	gdouble x;
	gdouble y;
} MouseClick;

typedef struct RuntimeInfo
{
	BezierCurveList list;
	MouseClick click;
} RuntimeInfo;

static gboolean canvas_draw(GtkWidget *self, cairo_t *cr, RuntimeInfo *data)
{
	// Set background to white
	cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
	cairo_paint(cr);

	cairo_set_antialias(cr, CAIRO_ANTIALIAS_DEFAULT);

	// Reset color to black
	cairo_set_source_rgb(cr, 0, 0, 0);

	// Show a small square where the cursor is
	cairo_new_path(cr);
	cairo_move_to(cr, data->click.x - 5, data->click.y - 5);
	cairo_rel_line_to(cr, 10, 0);
	cairo_rel_line_to(cr, 0, 10);
	cairo_rel_line_to(cr, -10, 0);
	cairo_close_path(cr);
	cairo_fill(cr);
	cairo_stroke(cr);

	// A demo for modifying a bezier curve by dragging the control point
	cairo_new_path(cr);
	cairo_move_to(cr, 250, 200);
	cairo_curve_to(cr, 250, 250, data->click.x, data->click.y, 300, 300);
	cairo_stroke(cr);

	return TRUE;
}

static gboolean canvas_button_pressed(GtkWidget *self, GdkEventButton *event, RuntimeInfo *data)
{
	data->click.x = event->x;
	data->click.y = event->y;

	gtk_widget_queue_draw(self);

	return TRUE;
}

static void canvas_button_move(GtkWidget* self, GdkEventMotion* event, RuntimeInfo *data)
{
	if ( !(event->state & GDK_BUTTON1_MASK) ) {
		return;
	}

	data->click.x = event->x;
	data->click.y = event->y;

	gtk_widget_queue_draw(self);
}

static void activate(GtkApplication *app, RuntimeInfo *data)
{
	GtkWidget *window, *box, *canvas;

	window = gtk_application_window_new(app);

	gtk_window_set_default_size(GTK_WINDOW(window), 500, 450);
	gtk_window_set_title(GTK_WINDOW(window), "bedit");
	gtk_window_set_resizable(GTK_WINDOW(window), TRUE);

	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

	gtk_container_add(GTK_CONTAINER(window), box);

	canvas = gtk_drawing_area_new();

	gtk_widget_add_events(
		canvas,
		GDK_BUTTON1_MOTION_MASK |
        GDK_BUTTON_PRESS_MASK   |
        GDK_BUTTON_RELEASE_MASK
	);

	gtk_box_pack_start(GTK_BOX(box), canvas, TRUE, TRUE, 0);

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

    app = gtk_application_new("xnemet04.vut.fit.gux.bedit", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), &info);

    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
