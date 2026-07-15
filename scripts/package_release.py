#!/usr/bin/env python3

from __future__ import annotations

import argparse
import hashlib
import json
import pathlib
import shutil
import subprocess
from datetime import datetime, UTC

from version import generate_version


def sha256(path: pathlib.Path):

    h = hashlib.sha256()

    with open(path, "rb") as f:
        while chunk := f.read(8192):
            h.update(chunk)

    return h.hexdigest()


def git_commit():

    try:
        return subprocess.check_output(
            ["git", "rev-parse", "--short", "HEAD"],
            text=True,
        ).strip()

    except Exception:
        return "unknown"


def main():

    parser = argparse.ArgumentParser()

    parser.add_argument("--binary", required=True)
    parser.add_argument("--board", required=True)
    parser.add_argument("--build-type", required=True)

    args = parser.parse_args()

    firmware = pathlib.Path(args.binary)

    if not firmware.exists():
        raise FileNotFoundError(firmware)

    version = generate_version()

    release = pathlib.Path("release")
    release.mkdir(exist_ok=True)

    firmware_name = f"OBC-firmware-{version}.bin"

    destination = release / firmware_name

    shutil.copy2(firmware, destination)

    digest = sha256(destination)

    with open(release / "firmware.sha256", "w") as f:
        f.write(f"{digest}  {firmware_name}\n")

    manifest = {
        "version": version,
        "board": args.board,
        "build_type": args.build_type,
        "firmware": firmware_name,
        "git_commit": git_commit(),
        "timestamp_utc": datetime.now(UTC).isoformat(),
        "sha256": digest,
        "size_bytes": destination.stat().st_size,
    }

    with open(release / "manifest.json", "w") as f:
        json.dump(manifest, f, indent=4)

    print("Release package created.")
    print(release.resolve())


if __name__ == "__main__":
    main()
