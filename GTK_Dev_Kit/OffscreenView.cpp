/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * OffscreenView.cpp - Part of the SLUDGE Dev Kit (GTK+ version)
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
#include <OpenGL/OpenGL.h>

#include "EditorView.h"

// On macOS, the view is a GtkDrawingArea. In a window with a GtkGLArea,
// GTK 3 on macOS paints the whole window with OpenGL, and macOS never shows
// that painting, so the window stays empty. Instead, we draw the view with
// our own OpenGL context into a framebuffer, read the pixels back and paint
// them with cairo.
class OffscreenView : public EditorView {
private:
	GtkWidget *area;
	void (*render)(gpointer data);
	gpointer data;
	CGLContextObj context;
	GLuint framebuffer, renderbuffer;
	cairo_surface_t *pixels;

	// Bind a framebuffer of width × height pixels, to draw the view into.
	gboolean bindFramebuffer(int width, int height)
	{
		if (! framebuffer) {
			glGenFramebuffers (1, &framebuffer);
			glGenRenderbuffers (1, &renderbuffer);
		}
		glBindFramebuffer (GL_FRAMEBUFFER, framebuffer);
		if (! pixels || cairo_image_surface_get_width (pixels) != width
				|| cairo_image_surface_get_height (pixels) != height) {
			// Without an alpha channel in the framebuffer, we read every
			// pixel as opaque, also where the view is cleared with a
			// transparent colour.
			glBindRenderbuffer (GL_RENDERBUFFER, renderbuffer);
			glRenderbufferStorage (GL_RENDERBUFFER, GL_RGB8, width, height);
			glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderbuffer);
			if (pixels) cairo_surface_destroy (pixels);
			pixels = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, height);
		}
		return glCheckFramebufferStatus (GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
	}

	// Read the framebuffer and paint it on cr. scale is the number of pixels
	// in the framebuffer for each pixel of cr in each direction.
	void paint(cairo_t *cr, int scale)
	{
		int width = cairo_image_surface_get_width (pixels);
		int height = cairo_image_surface_get_height (pixels);

		// In memory, a pixel of a cairo ARGB32 surface is B, G, R and A on a
		// little-endian Mac, which is GL_BGRA with GL_UNSIGNED_INT_8_8_8_8_REV.
		cairo_surface_flush (pixels);
		glPixelStorei (GL_PACK_ROW_LENGTH, cairo_image_surface_get_stride (pixels) / 4);
		glReadPixels (0, 0, width, height, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV,
				cairo_image_surface_get_data (pixels));
		glPixelStorei (GL_PACK_ROW_LENGTH, 0);
		cairo_surface_mark_dirty (pixels);

		// The first row in OpenGL is the bottom row of the picture, so we
		// paint the rows upside down.
		cairo_save (cr);
		cairo_scale (cr, 1.0 / scale, -1.0 / scale);
		cairo_translate (cr, 0, -height);
		cairo_set_source_surface (cr, pixels, 0, 0);
		cairo_paint (cr);
		cairo_restore (cr);
	}

	static gboolean onDraw(GtkWidget *theWidget, cairo_t *cr, gpointer view)
	{
		OffscreenView *self = (OffscreenView *) view;
		int scale = gtk_widget_get_scale_factor (theWidget);
		int width = gtk_widget_get_allocated_width (theWidget) * scale;
		int height = gtk_widget_get_allocated_height (theWidget) * scale;
		if (width <= 0 || height <= 0) return FALSE;
		if (! self->makeCurrent () || ! self->bindFramebuffer (width, height)) return FALSE;
		self->render (self->data);
		self->paint (cr, scale);
		return TRUE;
	}

public:
	OffscreenView(void (*render)(gpointer data), gpointer data)
	{
		this->render = render;
		this->data = data;
		context = NULL;
		framebuffer = renderbuffer = 0;
		pixels = NULL;
		area = gtk_drawing_area_new ();
		g_signal_connect (area, "draw", G_CALLBACK (onDraw), this);
	}

	~OffscreenView()
	{
		if (pixels) cairo_surface_destroy (pixels);
		if (context) CGLDestroyContext (context);
	}

	GtkWidget *getWidget()
	{
		return area;
	}

	gboolean makeCurrent()
	{
		if (! context) {
			// The same kind of context as the one of a GtkGLArea on the other
			// systems, without a window to draw into.
			CGLPixelFormatAttribute attributes[] = {
				kCGLPFAOpenGLProfile, (CGLPixelFormatAttribute) kCGLOGLPVersion_3_2_Core,
				kCGLPFAAllowOfflineRenderers,
				(CGLPixelFormatAttribute) 0
			};
			CGLPixelFormatObj format;
			GLint formatCount;
			if (CGLChoosePixelFormat (attributes, &format, &formatCount) != kCGLNoError || ! format) {
				g_critical ("Couldn't find an OpenGL 3.2 pixel format.");
				return FALSE;
			}
			CGLError error = CGLCreateContext (format, NULL, &context);
			CGLDestroyPixelFormat (format);
			if (error != kCGLNoError) {
				g_critical ("Couldn't create an OpenGL context: %s", CGLErrorString (error));
				context = NULL;
				return FALSE;
			}
		}
		return CGLSetCurrentContext (context) == kCGLNoError;
	}
};

EditorView *createEditorView(void (*render)(gpointer data), gpointer data)
{
	return new OffscreenView (render, data);
}
