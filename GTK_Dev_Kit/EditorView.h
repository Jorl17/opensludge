/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * EditorView.h - Part of the SLUDGE Dev Kit (GTK+ version)
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

// The OpenGL view of an editor: the widget that shows it, and its OpenGL
// context. We build the Dev Kit with one implementation of it, in
// GLAreaView.cpp on Linux and Windows, and in OffscreenView.cpp on macOS.
class EditorView {
public:
	virtual ~EditorView() {}
	// Return the widget to put into the window.
	virtual GtkWidget *getWidget() = 0;
	// Make the OpenGL context of the view current. Return FALSE when the
	// view has no context yet, or when we couldn't make it current. In
	// OffscreenView, we create the context the first time.
	virtual gboolean makeCurrent() = 0;
};

// Create the view. Each time the view needs drawing, we make its context
// current and call render with data.
EditorView *createEditorView(void (*render)(gpointer data), gpointer data);
