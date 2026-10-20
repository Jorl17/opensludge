#!/usr/bin/env python3
# Print the stack of the thread that crashed, from the newest report that
# macOS wrote for a program, and the reason that macOS gives for the crash.
#
# Usage: print-crash-report.py <program name>
#
# macOS writes the report in ~/Library/Logs/DiagnosticReports a few seconds
# after the crash, so we wait up to 60 seconds for it.

import glob
import json
import os
import sys
import time

name = sys.argv[1]
pattern = os.path.expanduser(f"~/Library/Logs/DiagnosticReports/{name}*.ips")
for _ in range(60):
    reports = sorted(glob.glob(pattern), key=os.path.getmtime)
    if reports:
        break
    time.sleep(1)
else:
    print(f"There is no crash report for {name}")
    sys.exit(0)

with open(reports[-1]) as report:
    report.readline()  # The first line is a header of its own.
    body = json.load(report)
images = body.get("usedImages", [])
print(f"Crash report {reports[-1]}")
print("Exception:", json.dumps(body.get("exception")))
print("Reason:", json.dumps(body.get("asi")))
for thread in body.get("threads", []):
    if not thread.get("triggered"):
        continue
    for frame in thread.get("frames", []):
        index = frame.get("imageIndex", -1)
        image = images[index].get("name", "?") if 0 <= index < len(images) else "?"
        print(f"  {image}  {frame.get('symbol', hex(frame.get('imageOffset', 0)))}"
              f"  +{frame.get('symbolLocation', 0)}  {frame.get('sourceFile', '')}:{frame.get('sourceLine', '')}")
