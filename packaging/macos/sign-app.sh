#!/bin/bash
# Sign a macOS application bundle.
#
# Usage: sign-app.sh <.app>
#
# When MACOS_SIGNING_IDENTITY is set, for example to "Developer ID
# Application: Name (TEAMID)", and that identity is in the keychain, we sign
# the app with it and the hardened runtime, as Apple requires to notarise.
# Otherwise we give the app an ad-hoc signature, without which an Apple
# Silicon Mac does not run it at all.

set -euo pipefail

app=$1
here=$(cd "$(dirname "$0")" && pwd)

sign () {
	if [ -n "${MACOS_SIGNING_IDENTITY:-}" ]; then
		codesign --force --timestamp --options runtime \
			--entitlements "$here/entitlements.plist" \
			--sign "$MACOS_SIGNING_IDENTITY" "$1"
	else
		codesign --force --sign - "$1"
	fi
}

# The signature of the app covers the signatures of the libraries and the
# other programs in the app, so we sign those first.
main=$(/usr/libexec/PlistBuddy -c "Print :CFBundleExecutable" "$app/Contents/Info.plist")
find "$app/Contents" -type f ! -path "$app/Contents/MacOS/$main" -print0 |
	while IFS= read -r -d '' file; do
		case "$(file -b "$file")" in *Mach-O*) sign "$file" ;; esac
	done
sign "$app"
codesign --verify --strict --verbose=2 "$app"
