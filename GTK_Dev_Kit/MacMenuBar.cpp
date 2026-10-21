/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * MacMenuBar.cpp - Part of the SLUDGE Dev Kit (GTK+ version)
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

#include <gtkosxapplication.h>

#include "MacMenuBar.h"

// Take an item out of its menu, and keep it for the app menu. When the menu
// then ends with separators, we remove them, and when it is empty, we remove
// the menu from the menu bar.
static void takeOutOfMenu(GtkWidget *item)
{
	GtkWidget *menu = gtk_widget_get_parent(item);
	g_object_ref(item);
	gtk_container_remove(GTK_CONTAINER(menu), item);

	GList *children = gtk_container_get_children(GTK_CONTAINER(menu));
	GList *last = g_list_last(children);
	while (last && GTK_IS_SEPARATOR_MENU_ITEM(last->data)) {
		gtk_container_remove(GTK_CONTAINER(menu), GTK_WIDGET(last->data));
		last = last->prev;
	}
	if (!last) {
		GtkWidget *menuItem = gtk_menu_get_attach_widget(GTK_MENU(menu));
		gtk_container_remove(GTK_CONTAINER(gtk_widget_get_parent(menuItem)), menuItem);
	}
	g_list_free(children);
}

static void activateItem(GtkMenuItem *appMenuItem, gpointer item)
{
	gtk_menu_item_activate(GTK_MENU_ITEM(item));
}

// Put appMenuItem into the app menu at index, and keep it for as long as the
// application runs.
static void insertIntoAppMenu(GtkosxApplication *app, GtkWidget *appMenuItem, gint index)
{
	g_object_ref_sink(appMenuItem);
	gtk_widget_show(appMenuItem);
	gtkosx_application_insert_app_menu_item(app, appMenuItem, index);
}

// Put a new item into the app menu at index, which activates item, our own
// item for the same command.
static void addToAppMenu(GtkosxApplication *app, const char *label, GtkWidget *item, gint index)
{
	GtkWidget *appMenuItem = gtk_menu_item_new_with_label(label);
	g_signal_connect(appMenuItem, "activate", G_CALLBACK(activateItem), item);
	insertIntoAppMenu(app, appMenuItem, index);
}

// Quit in the app menu, or Command+Q, asks to terminate the application.
// We refuse, and activate our own Quit item instead, which asks whether to
// save the changes before we quit.
static gboolean onBlockTermination(GtkosxApplication *app, gpointer quitItem)
{
	gtk_menu_item_activate(GTK_MENU_ITEM(quitItem));
	return TRUE;
}

void useMacMenuBar(GtkBuilder *builder)
{
	GtkosxApplication *app = (GtkosxApplication *) g_object_new(GTKOSX_TYPE_APPLICATION, NULL);
	GtkWidget *menuBar = GTK_WIDGET(gtk_builder_get_object(builder, "menubar1"));
	GtkWidget *quitItem = GTK_WIDGET(gtk_builder_get_object(builder, "menu_quit"));
	GtkWidget *aboutItem = GTK_WIDGET(gtk_builder_get_object(builder, "menu_about"));
	GObject *preferencesItem = gtk_builder_get_object(builder, "menu_preferences");
	if (!menuBar || !quitItem || !aboutItem) {
		g_critical("The window has no menu bar with Quit and About items.");
		return;
	}

	// macOS puts Quit, About and Preferences in the app menu, so we take
	// them out of our menus before we give the menus to macOS.
	takeOutOfMenu(quitItem);
	takeOutOfMenu(aboutItem);
	if (preferencesItem) takeOutOfMenu(GTK_WIDGET(preferencesItem));
	gtk_widget_hide(menuBar);
	gtkosx_application_set_menu_bar(app, GTK_MENU_SHELL(menuBar));

	// At index 0, gtkosx_application_insert_app_menu_item names the item
	// "About" and the name of the app.
	gint index = 0;
	addToAppMenu(app, "About", aboutItem, index++);
	insertIntoAppMenu(app, gtk_separator_menu_item_new(), index++);
	if (preferencesItem) {
		addToAppMenu(app, "Preferences…", GTK_WIDGET(preferencesItem), index++);
		insertIntoAppMenu(app, gtk_separator_menu_item_new(), index++);
	}

	g_signal_connect(app, "NSApplicationBlockTermination", G_CALLBACK(onBlockTermination), quitItem);
	gtkosx_application_ready(app);
}
