#!/bin/bash
# Make a macOS application bundle that contains the SLUDGE engine or a tool
# of the Dev Kit.
#
# Usage: make-app.sh <executable> <output .app> <app name> <bundle id>
#                    <icon .icns> <minimum macOS version> [<game data file> [<extra file>...]]
#
# APP_CATEGORY is the App Store category of the app, and APP_COPYRIGHT is
# its copyright notice. When they are not set, we use the ones of the
# engine.
#
# With a game data file, we start that game when the player opens the app.
# We copy the data file and any extra files, such as the readme and licence
# of the game, into the Resources folder of the app. Without a game data
# file, we show a file chooser, in which the player picks a game.

set -euo pipefail

executable=$1 app=$2 name=$3 bundle_id=$4 icon=$5 min_macos=$6
shift 6

here=$(cd "$(dirname "$0")" && pwd)
# The version in Info.plist must be numbers and dots, so we leave out a
# suffix such as -beta.1.
version=$("$here/../version.sh")
version=${version%%-*}
copyright=${APP_COPYRIGHT:-"SLUDGE engine © Tim Furnish, Rikard Peterson, Tobias Hansen and contributors"}
category=${APP_CATEGORY:-public.app-category.adventure-games}
executable_name=$(basename "$executable")

rm -rf "$app"
mkdir -p "$app/Contents/MacOS" "$app/Contents/Resources"

cp "$executable" "$app/Contents/MacOS/$executable_name"
chmod 755 "$app/Contents/MacOS/$executable_name"
cp "$icon" "$app/Contents/Resources/AppIcon.icns"

sed -e "s|@BUNDLE_ID@|$bundle_id|g" \
    -e "s|@APP_NAME@|$name|g" \
    -e "s|@VERSION@|$version|g" \
    -e "s|@MIN_MACOS@|$min_macos|g" \
    -e "s|@COPYRIGHT@|$copyright|g" \
    -e "s|@EXECUTABLE@|$executable_name|g" \
    -e "s|@CATEGORY@|$category|g" \
    "$here/Info.plist.in" > "$app/Contents/Info.plist"
printf 'APPL????' > "$app/Contents/PkgInfo"

if [ $# -gt 0 ]; then
	cp "$1" "$app/Contents/Resources/gamedata"
	shift
	for extra in "$@"; do
		cp -R "$extra" "$app/Contents/Resources/"
	done
fi

echo "Created $app"
