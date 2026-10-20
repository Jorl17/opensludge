#!/bin/bash
# Import a Developer ID certificate into a new keychain, and print the name
# of its signing identity, for MACOS_SIGNING_IDENTITY in sign-app.sh.
#
# Usage: set-up-signing.sh <keychain folder>
#
# MACOS_CERTIFICATE_P12_BASE64 is the certificate, as a .p12 file in base64,
# and MACOS_CERTIFICATE_PASSWORD is its password.

set -euo pipefail

folder=$1
keychain=$folder/signing.keychain-db
keychain_password=$(openssl rand -hex 16)
echo "$MACOS_CERTIFICATE_P12_BASE64" | base64 --decode > "$folder/certificate.p12"
security create-keychain -p "$keychain_password" "$keychain"
security set-keychain-settings -lut 21600 "$keychain"
security unlock-keychain -p "$keychain_password" "$keychain"
security import "$folder/certificate.p12" -P "$MACOS_CERTIFICATE_PASSWORD" \
	-A -t cert -f pkcs12 -k "$keychain" > /dev/null
rm "$folder/certificate.p12"
security set-key-partition-list -S apple-tool:,apple: -k "$keychain_password" "$keychain" > /dev/null
security list-keychains -d user -s "$keychain" $(security list-keychains -d user | tr -d '"')
identity=$(security find-identity -v -p codesigning "$keychain" | sed -n -E 's/.*"(Developer ID Application: .*)"/\1/p' | head -n 1)
if [ -z "$identity" ]; then
	echo "There is no valid Developer ID Application identity in the certificate" >&2
	exit 1
fi
echo "$identity"
