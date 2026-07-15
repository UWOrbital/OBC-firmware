#!/usr/bin/env python3

from __future__ import annotations

import hashlib
import json
import pathlib
import sys

release = pathlib.Path("release")

manifest = release / "manifest.json"

sha = release / "firmware.sha256"

if not manifest.exists():
    sys.exit("manifest.json missing")

if not sha.exists():
    sys.exit("firmware.sha256 missing")

firmware = next(release.glob("*.bin"), None)

if firmware is None:
    sys.exit("Firmware binary missing")

with open(manifest) as f:
    data = json.load(f)

digest = hashlib.sha256(firmware.read_bytes()).hexdigest()

if digest != data["sha256"]:
    sys.exit("Manifest SHA mismatch")

with open(sha) as f:
    expected = f.read().split()[0]

if expected != digest:
    sys.exit("SHA256 file mismatch")

print("Release package verified successfully.")
