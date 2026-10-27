/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * GLAreaView.cpp - Part of the SLUDGE Dev Kit (GTK+ version)
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

#include <epoxy/gl.h>

#include "EditorView.h"

// On Linux and Windows, the view is a GtkGLArea, which creates and manages
// the OpenGL context, and swaps the buffers after every render.
class GLAreaView : public EditorView {
private:
	GtkWidget *area;
	void (*render)(gpointer data);
	gpointer data;

	static gboolean onRender(GtkGLArea *theArea, GdkGLContext *context, gpointer view)
	{
		GLAreaView *self = (GLAreaView *) view;
		self->render (self->data);
		return TRUE;
	}

public:
	GLAreaView(void (*render)(gpointer data), gpointer data)
	{
		this->render = render;
		this->data = data;
		area = gtk_gl_area_new ();
		// GtkGLArea turns on the depth test when the area has a depth
		// buffer, and we never clear one, so we draw without it. Without an
		// alpha channel, the clear colour of each view is opaque.
		gtk_gl_area_set_has_depth_buffer (GTK_GL_AREA (area), FALSE);
		gtk_gl_area_set_has_alpha (GTK_GL_AREA (area), FALSE);
		gtk_gl_area_set_auto_render (GTK_GL_AREA (area), TRUE);
		g_signal_connect (area, "render", G_CALLBACK (onRender), this);
	}

	GtkWidget *getWidget()
	{
		return area;
	}

	gboolean makeCurrent()
	{
		if (gtk_gl_area_get_context (GTK_GL_AREA (area)) == NULL) return FALSE;
		gtk_gl_area_make_current (GTK_GL_AREA (area));
		return gtk_gl_area_get_error (GTK_GL_AREA (area)) == NULL;
	}
};

EditorView *createEditorView(void (*render)(gpointer data), gpointer data)
{
	return new GLAreaView (render, data);
}
