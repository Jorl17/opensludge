/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * Common.cpp - Part of the SLUDGE Dev Kit (GTK+ version)
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gstdio.h>

#if defined(__APPLE__)
#include <CoreFoundation/CoreFoundation.h>
#endif

#include "Common.h"
#include "interface.h"

gint message_dialog( GtkMessageType type, const gchar *title, const gchar *message);

bool askAQuestion (const char * head, const char * msg)
{
	gint response;

	response = message_dialog(GTK_MESSAGE_QUESTION, head, msg);

	if (response == GTK_RESPONSE_YES) return true;
		else return false;
}

bool errorBox (const char * head, const char * msg)
{
	message_dialog(GTK_MESSAGE_ERROR, head, msg);

	return false;
}

gint
message_dialog(GtkMessageType type, const gchar *title, const gchar *message)
{
	GtkWidget *dialog;
	gint response;
	GtkButtonsType buttons;

	if (type == GTK_MESSAGE_QUESTION) buttons = GTK_BUTTONS_YES_NO;
	else buttons = GTK_BUTTONS_OK;
        
	dialog = gtk_message_dialog_new (NULL,
						GTK_DIALOG_MODAL,
						type,
						buttons,
						"%s", message);
                        
	gtk_window_set_title(GTK_WINDOW(dialog), title);
	response = gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);   
        
	return response;
}

// Add a pattern to the filter for the files with the given extension, such as
// "png", in upper or lower case. Windows ignores case in file names, and in
// the Windows file dialog, only * and ? are wildcards, so on Windows we add
// "*.png". On Linux, macOS and other systems, we add "*.[pP][nN][gG]".
void addExtensionPattern (GtkFileFilter * filter, const char * extension)
{
#ifdef __WIN32
	gchar * pattern = g_strconcat ("*.", extension, NULL);
#else
	GString * built = g_string_new ("*.");
	for (const char * c = extension; *c; c++) {
		g_string_append_printf (built, "[%c%c]", g_ascii_tolower (*c), g_ascii_toupper (*c));
	}
	gchar * pattern = g_string_free (built, FALSE);
#endif
	gtk_file_filter_add_pattern (filter, pattern);
	g_free (pattern);
}

void setFileChooserFilters(GtkFileChooser *theFileChooser, gboolean png, gboolean tga)
{
	GtkFileFilter *pngtgafilter, *pngfilter, *tgafilter;

	if (png && tga) {
		pngtgafilter = gtk_file_filter_new();
		gtk_file_filter_set_name(pngtgafilter, "PNG/TGA images");
		addExtensionPattern(pngtgafilter, "png");
		addExtensionPattern(pngtgafilter, "tga");
		gtk_file_chooser_add_filter(theFileChooser, pngtgafilter);
	}
	if (png) {
		pngfilter = gtk_file_filter_new();
		gtk_file_filter_set_name(pngfilter, "PNG images");
		addExtensionPattern(pngfilter, "png");
		gtk_file_chooser_add_filter(theFileChooser, pngfilter);
	}
	if (tga) {
		tgafilter = gtk_file_filter_new();
		gtk_file_filter_set_name(tgafilter, "TGA images");
		addExtensionPattern(tgafilter, "tga");
		gtk_file_chooser_add_filter(theFileChooser, tgafilter);
	}
	if (png && tga) {
		gtk_file_chooser_set_filter(theFileChooser, pngtgafilter);
	} else if (png) {
		gtk_file_chooser_set_filter(theFileChooser, pngfilter);
	} else if (tga) {
		gtk_file_chooser_set_filter(theFileChooser, tgafilter);
	}
}

const char * getTempDir ()
{
	return g_get_user_cache_dir();
}

char * joinTwoStrings (const char * a, const char * b) {
	char * nS = new char[strlen (a) + strlen (b) + 1];
	sprintf (nS, "%s%s", a, b);
	return nS;
}

bool fileExists(char * file) {
	FILE * tester;
	bool retval = false;
	tester = fopen (file, "rb");
	if (tester) {
		retval = true;
		fclose (tester);
	}
	return retval;
}

void flipBackslashes(char **string)
{
#ifdef __WIN32
	for (int i = 0; (*string)[i] != 0; i++)
	{
		if ((*string)[i] == '\\')
			(*string)[i] = '/';
	}
#endif
}

// Return the path of a file in sludge-devkit, the folder in the configuration
// folder of the user where we keep the settings of the Dev Kit, and create
// that folder if it does not exist yet. Free the path with g_free.
char * getSettingsFilePath (const char * fileName)
{
	gchar * folder = g_build_filename (g_get_user_config_dir (), "sludge-devkit", NULL);
	g_mkdir_with_parents (folder, 0777);
	gchar * path = g_build_filename (folder, fileName, NULL);
	g_free (folder);
	return path;
}

// The folder with the .glade files and the icons of the tools, with a slash
// at the end.
static const char * dataFolder = DATADIR;

const char * getDataFolder ()
{
	return dataFolder;
}

#if defined(__APPLE__)
// Return the path of the Resources folder of the app that we run in, or NULL
// when we don't run in an app, for example after make install. Free it with
// g_free.
static gchar * getAppResourcesFolder ()
{
	CFBundleRef bundle = CFBundleGetMainBundle ();
	// Outside an app, the main bundle is the folder of the program, and it has
	// no identifier.
	if (! bundle || ! CFBundleGetIdentifier (bundle)) return NULL;
	CFURLRef url = CFBundleCopyResourcesDirectoryURL (bundle);
	if (! url) return NULL;
	CFURLRef absoluteUrl = CFURLCopyAbsoluteURL (url);
	char path[PATH_MAX];
	Boolean found = CFURLGetFileSystemRepresentation (absoluteUrl, true, (UInt8 *) path, sizeof (path));
	CFRelease (absoluteUrl);
	CFRelease (url);
	return found ? g_strdup (path) : NULL;
}
#endif

void setUpProgramFiles (const char * programFullPath)
{
#if defined(__WIN32)
	// DATADIR is the resources folder next to the programs, relative to the
	// working folder, so we change to the folder of the program.
	char programPath[1000];
	int lastSlash = 0;
	sprintf(programPath, "%s", programFullPath);
	for (int j = 0; programPath[j] != 0; j++) {
		if (programPath[j] == '\\') {
			programPath[j] = '/';
			lastSlash = j;
		}
	}
	programPath[lastSlash + 1] = NULL;
	g_chdir(programPath);
#elif defined(__APPLE__)
	// In an app, the data of the tools and the files that GTK loads at run
	// time, such as the icons and the image loaders, are in the Resources
	// folder.
	gchar * resources = getAppResourcesFolder ();
	if (! resources) return;
	dataFolder = g_strconcat (resources, "/", NULL);
	gchar * share = g_build_filename (resources, "share", NULL);
	gchar * loaders = g_build_filename (resources, "lib", "gdk-pixbuf-2.0", "2.10.0", "loaders.cache", NULL);
	g_setenv ("XDG_DATA_DIRS", share, TRUE);
	g_setenv ("GDK_PIXBUF_MODULE_FILE", loaders, TRUE);
	g_free (loaders);
	g_free (share);
	g_free (resources);
#elif defined(__unix__)
	// DATADIR is the folder where make install puts the data.
#else
#error "No folder for the data of the tools on this platform"
#endif
}

void replaceInvalidCharacters(char *string, int *retval)
{
	const gchar *end;
	if (!g_utf8_validate(string, -1, &end)) {
		for (int i = 0; string[i] != 0; i++) {
			if (string[i] == end[0])
				string[i] = '_';
		}
		replaceInvalidCharacters(string, retval);
		*retval = 0;
	} else {
		return;
	}
}

// Start cmd, a program with its options, in the folder path, with args, when
// it is not NULL, as one more argument. Return TRUE when the program started.
int sh_cmd (gchar * path, const gchar * cmd, gchar * args)
{
	char   **argv = NULL;
	int      argc;
	int      rc;

	if (cmd == NULL)
		return FALSE;

	if (cmd[0] == '\0')
		return FALSE;

	if (path != NULL)
		g_chdir (path);

	if (!g_shell_parse_argv (cmd, &argc, &argv, NULL))
		return FALSE;

	if (args != NULL) {
		#ifdef __WIN32
		for (int i = 0; args[i] != 0; i++)
		{
			if (args[i] == '/')
				args[i] = '\\';
		}
		#endif

		argv = (char **) g_realloc (argv, (argc + 2) * sizeof (char *));
		argv[argc] = g_strdup (args);
		argv[argc + 1] = NULL;
	}

	rc = g_spawn_async (path, argv, NULL,
		      G_SPAWN_SEARCH_PATH,
		      NULL, NULL, NULL, NULL);

	g_strfreev (argv);

	return rc;
}
