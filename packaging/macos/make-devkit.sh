#!/bin/bash
# Make the macOS Dev Kit: an app for each tool, with the parts of GTK that
# the tool loads, and OpenSLUDGE.app, to run the games with.
#
# Usage: make-devkit.sh <install folder> <GTK prefix> <OpenSLUDGE.app> <minimum macOS version> <output folder>
#
# The install folder is the one from make install, with the tools in bin and
# their data in share/sludge. The GTK prefix is the folder with GTK and the
# libraries that it depends on, for example /opt/homebrew. We copy those
# libraries into each app with dylibbundler, and sign the apps with
# sign-app.sh. We make the Dev Kit in the folder "OpenSLUDGE Dev Kit" in the
# output folder.

set -euo pipefail

install=$1 gtk=$2 engine=$3 min_macos=$4 output=$5
here=$(cd "$(dirname "$0")" && pwd)
kit="$output/OpenSLUDGE Dev Kit"
loaders=lib/gdk-pixbuf-2.0/2.10.0/loaders

# Usage: make_tool_app <executable> <app name> <icon> [<other executable>...]
make_tool_app () {
	local executable=$1 name=$2 icon=$3
	shift 3
	local app="$kit/$name.app"
	local resources="$app/Contents/Resources"

	APP_CATEGORY=public.app-category.developer-tools \
	APP_COPYRIGHT="SLUDGE Dev Kit © Tim Furnish, Rikard Peterson, Tobias Hansen and contributors" \
		"$here/make-app.sh" "$install/bin/$executable" "$app" "$name" \
		"org.opensludge.devkit.${executable#sludge-}" "$here/../../images/$icon.icns" "$min_macos"
	for other in "$@"; do cp "$install/bin/$other" "$app/Contents/MacOS/"; done
	cp -R "$install/share/sludge/." "$resources/"

	# In setUpProgramFiles, in GTK_Dev_Kit/Common.cpp, we point GTK to the
	# loaders, settings schemas and icons in the Resources folder.
	mkdir -p "$resources/$loaders" "$resources/share/glib-2.0/schemas" "$resources/share/icons"
	cp "$gtk/$loaders"/*.so "$resources/$loaders/"
	cp "$gtk"/share/glib-2.0/schemas/org.gtk.Settings.*.gschema.xml "$resources/share/glib-2.0/schemas/"
	glib-compile-schemas "$resources/share/glib-2.0/schemas"
	for theme in Adwaita AdwaitaLegacy hicolor; do
		# In Homebrew, the themes are symbolic links into the folders of the
		# packages, so we copy the files that they point to.
		if [ -d "$gtk/share/icons/$theme" ]; then cp -RL "$gtk/share/icons/$theme" "$resources/share/icons/"; fi
	done

	# We list the loaders in the GTK prefix, where they find their libraries.
	# In the list, we replace the folder of the loaders with the one in the
	# app, relative to the executable, which dyld resolves when GLib loads
	# them.
	"$gtk/bin/gdk-pixbuf-query-loaders" "$gtk/$loaders"/*.so |
		sed "s|$gtk/$loaders/|@executable_path/../Resources/$loaders/|" > "$resources/$loaders/../loaders.cache"

	# dylibbundler changes the install names of the libraries that it copies,
	# so we change the ones of the loaders ourselves.
	for loader in "$resources/$loaders"/*.so; do
		install_name_tool -id "@executable_path/../Resources/$loaders/$(basename "$loader")" "$loader"
	done
	local programs=()
	for file in "$app/Contents/MacOS"/* "$resources/$loaders"/*.so; do programs+=(-x "$file"); done
	dylibbundler -of -b -cd "${programs[@]}" -s "$gtk/lib" \
		-d "$app/Contents/Frameworks" -p @executable_path/../Frameworks/

	# dylibbundler can add the same rpath to a file twice, and dyld refuses to
	# load a file with a duplicate rpath, so we remove the extra ones.
	find "$app/Contents" -type f -print0 | while IFS= read -r -d '' file; do
		case "$(file -b "$file")" in *Mach-O*) ;; *) continue ;; esac
		otool -l "$file" | awk '/cmd LC_RPATH/ { getline; getline; print $2 }' | sort | uniq -d |
			while read -r rpath; do
				while [ "$(otool -l "$file" | grep -cF "path $rpath (")" -gt 1 ]; do
					install_name_tool -delete_rpath "$rpath" "$file"
				done
			done
	done

	"$here/sign-app.sh" "$app"
}

rm -rf "$kit"
mkdir -p "$kit"
# We run the compiler from the command line, and put it in the app of the
# project manager, which contains the libraries that the compiler uses.
make_tool_app sludge-projectmanager "SLUDGE Project Manager" ProjIcon sludge-compiler
make_tool_app sludge-floormaker "SLUDGE Floor Maker" floorIcon
make_tool_app sludge-spritebankeditor "SLUDGE Sprite Bank Editor" spriteIcon
make_tool_app sludge-zbuffermaker "SLUDGE Z-Buffer Maker" zIcon
make_tool_app sludge-translationeditor "SLUDGE Translation Editor" flags
cp -R "$engine" "$kit/"
cp "$here/README-DevKit.txt" "$kit/README.txt"
ls -la "$kit"
