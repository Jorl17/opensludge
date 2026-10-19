#!/bin/sh
# Make the Windows zip of the Dev Kit: the tools, the engine to run the
# games with, and the parts of GTK that the tools load.
#
# Usage: make-devkit-zip.sh <install folder> <GTK prefix> <engine .exe> <version> <label> <output folder>
#
# The install folder is the one from make install, with the tools in bin and
# their data in share/sludge. The GTK prefix is the folder with the GTK DLLs
# in bin, for example /ucrt64 in MSYS2. We put the label, for example x64,
# in the name of the zip. Set OBJDUMP and STRIP to the tools for Windows
# executables when they are not objdump and strip, and RUN to the program
# that runs Windows executables when they cannot run directly, for example
# wine on Linux.

set -eu

install=$1 gtk=$2 engine=$3 version=$4 label=$5 output=$6
here=$(cd "$(dirname "$0")" && pwd)
stage=$(mktemp -d)
trap 'rm -rf "$stage"' EXIT
kit="$stage/OpenSLUDGE Dev Kit"
loaders=lib/gdk-pixbuf-2.0/2.10.0/loaders

mkdir -p "$kit/resources" "$output"
cp "$install"/bin/sludge-*.exe "$kit/"
cp "$engine" "$kit/OpenSLUDGE.exe"
for exe in "$kit"/*.exe; do ${STRIP:-strip} "$exe"; done
cp -r "$install"/share/sludge/. "$kit/resources/"
cp "$here/README-DevKit.txt" "$kit/README.txt"

# GLib starts other programs, such as the engine, through these helpers.
cp "$gtk"/bin/gspawn-win64-helper.exe "$gtk"/bin/gspawn-win64-helper-console.exe "$kit/"

# GTK looks for its loaders, settings schemas and icons in the lib and share
# folders next to its DLLs.
mkdir -p "$kit/$loaders" "$kit/share/glib-2.0/schemas" "$kit/share/icons"
cp "$gtk/$loaders"/*.dll "$kit/$loaders/"
cp "$gtk/bin/gdk-pixbuf-query-loaders.exe" "$kit/"
cp "$gtk"/share/glib-2.0/schemas/org.gtk.Settings.*.gschema.xml "$kit/share/glib-2.0/schemas/"
glib-compile-schemas "$kit/share/glib-2.0/schemas"
cp -r "$gtk/share/icons/Adwaita" "$gtk/share/icons/AdwaitaLegacy" "$gtk/share/icons/hicolor" "$kit/share/icons/"

# Copy the DLLs that the programs and the loaders import from the GTK prefix,
# and then the DLLs that those import, until we have them all.
while :; do
	missing=$(find "$kit" -name '*.exe' -o -name '*.dll' | while read -r file; do
		${OBJDUMP:-objdump} -p "$file" | sed -n 's/^\s*DLL Name: //p'
	done | sort -u | while read -r dll; do
		if [ -f "$gtk/bin/$dll" ] && [ ! -f "$kit/$dll" ]; then echo "$dll"; fi
	done)
	[ -z "$missing" ] && break
	for dll in $missing; do cp "$gtk/bin/$dll" "$kit/"; done
done

# gdk-pixbuf-query-loaders.exe writes the paths of the loaders relative to
# its own folder only when the loaders are in that folder, so we run it in
# the Dev Kit folder, with the DLLs that it needs, and remove it afterwards.
(cd "$kit" && ${RUN:-} ./gdk-pixbuf-query-loaders.exe "$loaders"/*.dll > "$loaders/../loaders.cache")
rm "$kit/gdk-pixbuf-query-loaders.exe"

output=$(cd "$output" && pwd)
zip="$output/OpenSLUDGE-DevKit-$version-windows-$label.zip"
# zip adds to an existing archive, so we remove the one from an earlier run.
rm -f "$zip"
(cd "$stage" && zip -q -r -9 "$zip" "OpenSLUDGE Dev Kit")
ls -la "$output"
