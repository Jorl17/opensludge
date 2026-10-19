/*
 * Test for sh_cmd in GTK_Dev_Kit/Common.cpp, with which the project manager
 * starts the engine, an editor or another tool on a file of the project.
 *
 * We start sh with a path longer than 256 characters as its argument, as for
 * a game in a deeply nested folder. sh writes the argument that it received
 * into a file, and we check that it received the whole path.
 */

#include <stdio.h>
#include <string.h>
#include <string>

#include <glib.h>
#include <glib/gstdio.h>

#include "Common.h"

int main () {
	gchar * folder = g_dir_make_tmp ("sh_cmd_test-XXXXXX", NULL);
	if (! folder) {
		printf ("FAIL: we can't create a temporary folder\n");
		return 1;
	}
	std::string argument = std::string (folder) + "/";
	for (int i = 0; i < 20; i ++) argument += "a folder with a long name/";
	argument += "the game.slg";

	gchar * cmd = g_strdup ("sh -c 'printf %s \"$0\" > received.txt'");
	gchar * args = g_strdup (argument.c_str ());
	int started = sh_cmd (folder, cmd, args);

	gchar * receivedFile = g_build_filename (folder, "received.txt", NULL);
	gchar * received = NULL;
	for (int i = 0; started && ! received && i < 100; i ++) {
		g_usleep (50000);
		g_file_get_contents (receivedFile, &received, NULL, NULL);
	}

	int failed = 0;
	if (! started) {
		printf ("FAIL: sh_cmd didn't start sh with an argument of %zu characters\n", argument.size ());
		failed = 1;
	} else if (! received || argument != received) {
		printf ("FAIL: sh received \"%s\" instead of \"%s\"\n", received ? received : "", argument.c_str ());
		failed = 1;
	} else {
		printf ("ok   sh received the whole argument of %zu characters\n", argument.size ());
	}

	g_remove (receivedFile);
	g_rmdir (folder);
	g_free (received);
	g_free (receivedFile);
	g_free (args);
	g_free (cmd);
	g_free (folder);
	return failed;
}
