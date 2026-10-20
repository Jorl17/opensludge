/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * SludgeGLApplication.cpp - Part of the SLUDGE Dev Kit (GTK+ version)
 *
 * Copyright (C) 2010 Tobias Hansen <tobias.han@gmx.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <glib-object.h>
#include <glib.h>

#include <epoxy/gl.h>

#include <errno.h>
#include <sys/types.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "SludgeGLApplication.h"
#include "draw2d.h"

// For each signal of the view, we call the method of the editor for it.

static void on_view_realize(GtkWidget *theWidget, gpointer app)
{
	((SludgeGLApplication *) app)->on_drawingarea1_realize(theWidget);
}

static void render_view(gpointer app)
{
	((SludgeGLApplication *) app)->renderView();
}

static gboolean on_view_scroll_event(GtkWidget *theWidget, GdkEventScroll *theEvent, gpointer app)
{
	return ((SludgeGLApplication *) app)->on_drawingarea1_scroll_event(theWidget, theEvent);
}

static gboolean on_view_button_press_event(GtkWidget *theWidget, GdkEventButton *theEvent, gpointer app)
{
	return ((SludgeGLApplication *) app)->on_drawingarea1_button_press_event(theWidget, theEvent);
}

static gboolean on_view_button_release_event(GtkWidget *theWidget, GdkEventButton *theEvent, gpointer app)
{
	return ((SludgeGLApplication *) app)->on_drawingarea1_button_release_event(theWidget, theEvent);
}

static gboolean on_view_motion_notify_event(GtkWidget *theWidget, GdkEventMotion *theEvent, gpointer app)
{
	return ((SludgeGLApplication *) app)->on_drawingarea1_motion_notify_event(theWidget, theEvent);
}

static gboolean on_view_leave_notify_event(GtkWidget *theWidget, GdkEventAny *theEvent, gpointer app)
{
	return ((SludgeGLApplication *) app)->on_drawingarea1_leave_notify_event(theWidget, theEvent);
}


SludgeGLApplication::SludgeGLApplication(const char * gladeFileName, const char * iconName, const char * configFile)
 : SludgeApplication(gladeFileName, iconName, configFile)
{
	view = NULL;
	theDrawingarea = NULL;
	if (!initSuccess) return;

	glInitialized = FALSE;
	viewSized = FALSE;
	zoomFitPending = FALSE;

	statusbarWidget = GTK_WIDGET (gtk_builder_get_object (theXml, "statusbar"));
	cursorxLabel = GTK_LABEL (gtk_builder_get_object (theXml, "cursorx_label"));
	cursoryLabel = GTK_LABEL (gtk_builder_get_object (theXml, "cursory_label"));
	picwLabel = GTK_LABEL (gtk_builder_get_object (theXml, "picw_label"));
	pichLabel = GTK_LABEL (gtk_builder_get_object (theXml, "pich_label"));
	zmulLabel = GTK_LABEL (gtk_builder_get_object (theXml, "zmul_label"));
	zoom100Button = NULL;
	zoomFitButton = NULL;

	awaitButton1Release = FALSE;
	awaitButton2Release = FALSE;

	x = y = w = h = 0;

	// We put the view into the viewbox of the window.
	GtkBox *viewBox = GTK_BOX (gtk_builder_get_object (theXml, "viewbox"));
	if (viewBox == NULL)
	{
		g_critical ("Failed to get the box for the view from the builder.\n");
		initSuccess = FALSE;
		return;
	}
	view = createEditorView (render_view, this);
	theDrawingarea = view->getWidget ();
	gtk_widget_add_events (theDrawingarea, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK
			| GDK_LEAVE_NOTIFY_MASK | GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_SCROLL_MASK);
	g_signal_connect (theDrawingarea, "realize", G_CALLBACK (on_view_realize), this);
	g_signal_connect (theDrawingarea, "scroll-event", G_CALLBACK (on_view_scroll_event), this);
	g_signal_connect (theDrawingarea, "button-press-event", G_CALLBACK (on_view_button_press_event), this);
	g_signal_connect (theDrawingarea, "button-release-event", G_CALLBACK (on_view_button_release_event), this);
	g_signal_connect (theDrawingarea, "motion-notify-event", G_CALLBACK (on_view_motion_notify_event), this);
	g_signal_connect (theDrawingarea, "leave-notify-event", G_CALLBACK (on_view_leave_notify_event), this);
	gtk_box_pack_start (viewBox, theDrawingarea, TRUE, TRUE, 0);
	gtk_widget_show (theDrawingarea);

	// Initialise the render mutex.
	g_mutex_init (&theRender_mutex);

	haveStatusbar = FALSE;
	zmul = 1.;
}

SludgeGLApplication::~SludgeGLApplication()
{
	delete view;
	g_mutex_clear (&theRender_mutex);
}

void SludgeGLApplication::setZ(float newZ)
{
	z = newZ;
	if (z > 200.0) z = 200.0;
	if (z < -19.0) z = -19.0;
	zmul = (1.0+z/20);
	refreshStatusbarZmul();
}

void
SludgeGLApplication::activateZoomButtons(int picwidth, int picheight)
{
	picWidth = picwidth;
	picHeight = picheight;
	gtk_widget_set_sensitive (GTK_WIDGET(zoom100Button), TRUE);
	gtk_widget_set_sensitive (GTK_WIDGET(zoomFitButton), TRUE);
}

void
SludgeGLApplication::deactivateZoomButtons()
{
	if (zoom100Button)
		gtk_widget_set_sensitive (GTK_WIDGET(zoom100Button), FALSE);
	if (zoomFitButton)
		gtk_widget_set_sensitive (GTK_WIDGET(zoomFitButton), FALSE);
}

void SludgeGLApplication::showStatusbar(int picwidth, int picheight)
{
	picWidth = picwidth;
	picHeight = picheight;
	haveStatusbar = TRUE;
	char buf[100];
	sprintf(buf, "%i", picWidth);
	gtk_label_set_text (picwLabel, buf);
	sprintf(buf, "%i", picHeight);
	gtk_label_set_text (pichLabel, buf);
	refreshStatusbarCursor(0, 0);
	refreshStatusbarZmul();
	gtk_widget_show_all(statusbarWidget);
}

void SludgeGLApplication::refreshStatusbarCursor(int local_pointx, int local_pointy)
{
	if (!haveStatusbar) return;

	char buf[100];
	sprintf(buf, "%i", (int)((local_pointx+x)*zmul));
	gtk_label_set_text (cursorxLabel, buf);
	sprintf(buf, "%i", (int)((local_pointy-y)*zmul));
	gtk_label_set_text (cursoryLabel, buf);
}

void SludgeGLApplication::refreshStatusbarZmul()
{
	if (!haveStatusbar) return;

	char buf[100];
	sprintf(buf, "%i", (int)(100/zmul));
	gtk_label_set_text (zmulLabel, buf);
}

void SludgeGLApplication::hideStatusbar()
{
	gtk_widget_hide(statusbarWidget);
}

void SludgeGLApplication::reshape()
{
	int newW = gtk_widget_get_allocated_width (theDrawingarea);
	int newH = gtk_widget_get_allocated_height (theDrawingarea);
	if (! w) x = 0;
	else x -= (newW-w)/2;
	if (! h) y = 0;
	else y += (newH-h)/2;
	h = newH;
	w = newW;
}

/*
 * Make sure the OpenGL context exists and is current, and let the
 * application set up its OpenGL state. Nothing happens until the widget is
 * realized and its context is available; instead the work is deferred to the
 * first "render" signal.
 */
void SludgeGLApplication::initGL()
{
	if (glInitialized) return;
	if (!makeViewCurrent ()) return;

	if (!startDrawing2D ())
	{
		g_critical ("Couldn't set up the OpenGL drawing.\n");
		return;
	}

	prepareOpenGL();
	glInitialized = TRUE;
}

/*
 * Ask for the OpenGL state to be set up again, for instance after a new image
 * was loaded. If the context is ready this happens immediately, otherwise it is
 * deferred to the next render.
 */
void SludgeGLApplication::refreshGL()
{
	glInitialized = FALSE;
	initGL();
	if (theDrawingarea != NULL) gtk_widget_queue_draw (theDrawingarea);
}

/*
 * Make the OpenGL context of the view current. GTK draws the window with its
 * own OpenGL context, so outside the "render" handler, we call this before we
 * create textures.
 */
gboolean SludgeGLApplication::makeViewCurrent()
{
	if (theDrawingarea == NULL) return FALSE;
	return view->makeCurrent ();
}

void SludgeGLApplication::setCoords()
{
	setViewArea (x, y, w, h, zmul);
}

// Callbacks:

void SludgeGLApplication::on_drawingarea1_realize(GtkWidget *theWidget)
{
	if (gtk_widget_get_window (theWidget) == NULL)
	{
		return;
	}

	// Set up OpenGL now if the context is already available.
	initGL();
}

/*
 * Draw the view, with the OpenGL context of the view current.
 */
void SludgeGLApplication::renderView()
{
	// First frame: set up the application's OpenGL state.
	initGL();

	/*
	 * Lock rendering mutex if it's not busy - otherwise exit.
	 * This prevents the renderer being spammed by drawing requests if the rendering takes longer than the feed/timer process.
	 */
	if (!g_mutex_trylock(&theRender_mutex))
		return;

	if (glInitialized)
	{
		viewSized = TRUE;
		reshape();
		if (zoomFitPending) on_zoom_fit_clicked ();

		// We set the viewport and the projection here, and not in reshape or
		// in the zoom and scroll handlers. GTK draws the rest of the window
		// with an OpenGL context of its own, and makes the context of this
		// view current only while it runs the render handler.
		// If we set them in another handler, we would change the context that
		// GTK draws the window with, and GTK would draw its buttons and labels
		// wrongly.
		// w and h are in logical pixels, but on a display with a scale factor
		// of 2, the view has twice as many pixels in each direction, so we
		// multiply the viewport by the scale factor.
		int scale = gtk_widget_get_scale_factor (theDrawingarea);
		glViewport (0, 0, w * scale, h * scale);
		setCoords();
		drawRect();
	}

	// Release the rendering mutex.
	g_mutex_unlock(&theRender_mutex);
}


/*
 * The timer-based process tells GtkGLArea to redraw, which results in another "render" signal.
 */
gboolean SludgeGLApplication::render_timer_event(gpointer theUser_data)
{
	GtkWidget *theWidget = GTK_WIDGET (theUser_data);

	/*
	 * Only send a redraw request if the render process is not busy.  This prevents long rendering processes
	 * from piling up.
	 */
	if (g_mutex_trylock(&theRender_mutex))
	{
		// Unlock the mutex before we issue the redraw.
		g_mutex_unlock(&theRender_mutex);
		gtk_widget_queue_draw (theWidget);
	}

	return TRUE;
}

gboolean SludgeGLApplication::on_drawingarea1_scroll_event(GtkWidget *theWidget, GdkEventScroll *theEvent)
{
	float deltaY;
	if (theEvent->direction == GDK_SCROLL_UP) {
		deltaY = -1.;
	}
	if (theEvent->direction == GDK_SCROLL_DOWN) {
		deltaY = 1.;
	}
	double x1, y1;
	int local_pointx, local_pointy;
	
	local_pointx = theEvent->x;
	local_pointy = theEvent->y;
	x1 = zmul*(local_pointx+x);
	y1 = -zmul*(local_pointy-y);

	setZ(z+deltaY);

	x = -(local_pointx)+x1/zmul;
	y = (local_pointy)+y1/zmul;

	render_timer_event(theDrawingarea);

	return FALSE;
}

gboolean SludgeGLApplication::on_drawingarea1_button_press_event
(GtkWidget *theWidget, GdkEventButton *theEvent)
{
	if (theEvent->button == 1) {
		button1Press (theEvent->x, theEvent->y);
	}
	else
	{
		mouseLoc2x = theEvent->x;
		mouseLoc2y = theEvent->y;
		awaitButton2Release = TRUE;	
	}

    return FALSE;
}

gboolean SludgeGLApplication::on_drawingarea1_button_release_event
(GtkWidget *theWidget, GdkEventButton *theEvent)
{
	if ( !((awaitButton1Release && theEvent->button == 1) || awaitButton2Release) ) return FALSE;

	if (theEvent->button == 1) 
	{
		button1Release (theEvent->x, theEvent->y);
	}
	else
	{
		awaitButton2Release = FALSE;
	}

	render_timer_event(theDrawingarea);

    return FALSE;
}

gboolean SludgeGLApplication::on_drawingarea1_motion_notify_event
(GtkWidget *theWidget, GdkEventMotion *theEvent)
{
	refreshStatusbarCursor(theEvent->x, theEvent->y);

	if ( (awaitButton1Release && theEvent->state & GDK_BUTTON1_MASK) || awaitButton2Release )
	{
		int local_pointx, local_pointy;
		local_pointx = theEvent->x;
		local_pointy = theEvent->y;

		if (theEvent->state & GDK_BUTTON1_MASK)
		{
			button1Motion (local_pointx, local_pointy);
		}
		else if (awaitButton2Release)
		{
			int x1 = x;
			int y1 = y;
			x = x1 + (mouseLoc2x - local_pointx);
			y = y1 + (local_pointy - mouseLoc2y);

			mouseLoc2x = theEvent->x;
			mouseLoc2y = theEvent->y;
		}
		render_timer_event(theDrawingarea);
	}

	gdk_event_request_motions (theEvent);

    return FALSE;
}

gboolean SludgeGLApplication::on_drawingarea1_leave_notify_event
(GtkWidget *theWidget, GdkEventAny *theEvent)
{
	awaitButton1Release = FALSE;
	awaitButton2Release = FALSE;

	drawingareaLeft ();

    return FALSE;
}

void SludgeGLApplication::on_zoom_100_realize (GtkToolButton *theButton)
{
	gtk_widget_set_sensitive (GTK_WIDGET(theButton), FALSE);
	zoom100Button = theButton;
}

void SludgeGLApplication::on_zoom_fit_realize (GtkToolButton *theButton)
{
	gtk_widget_set_sensitive (GTK_WIDGET(theButton), FALSE);
	zoomFitButton = theButton;
}

void SludgeGLApplication::on_zoom_100_clicked ()
{
	setZ(0.);
	x = -(w - picWidth / zmul) / 2.;
	y = (h - picHeight / zmul) / 2.;
	reshape();
}

// Fit the picture into the view. GTK gives the view its size only just
// before we first draw it, so if we fitted the picture before that, for
// example right after opening a file, we would fit it into a size of 0. In
// that case, we set zoomFitPending instead, and fit the picture in the render
// handler once the view has its size.
void SludgeGLApplication::on_zoom_fit_clicked ()
{
	if (!viewSized) {
		zoomFitPending = TRUE;
		return;
	}
	zoomFitPending = FALSE;
	fitZoom ();
}

void SludgeGLApplication::fitZoom ()
{
	float zmulx, zmuly;
	zmulx = (float) picWidth / w;
	zmuly = (float) picHeight / h;

	if (zmulx > zmuly) {
		zmul = zmulx;
		x = 0;
		y = (h - picHeight / zmul) / 2.;
	} else {
		zmul = zmuly;
		x = -(w - picWidth / zmul) / 2.;
		y = 0;
	}
	z =	20. * (zmul - 1.);

	refreshStatusbarZmul();
	reshape();
}
