#!/usr/bin/env python3
"""Generate local CMake presets from `plcncli get sdks` (Python 3.9+)."""

import hashlib
import json
import os
from pathlib import Path
import re
import subprocess
import sys


OWNER = "ptusa_main/plcnext-presets/1.0"
ROOT = Path(__file__).resolve().parents[1]


def generate_presets(sdks, windows):
    configure, build = [], []
    seen = set()
    for sdk in sdks:
        root = Path(sdk["path"]).resolve()
        if not (root / "toolchain.cmake").is_file():
            raise ValueError(f"SDK toolchain not found: {root / 'toolchain.cmake'}")
        make = None
        if windows:
            candidates = sorted((root / "sysroots").glob("*/usr/bin/make.exe"))
            if len(candidates) != 1:
                raise ValueError(f"Expected one SDK make.exe in {root}, found {len(candidates)}")
            make = candidates[0]
        for target in sdk["targets"]:
            device, version = target["name"], target["longVersion"]
            # Include the path so parallel SDK installations never share a cache.
            identity = f"{root.as_posix()}|{device}|{version}"
            if identity in seen:
                continue
            seen.add(identity)
            suffix = hashlib.sha256(identity.encode()).hexdigest()[:10]
            slug = re.sub(r"[^A-Za-z0-9_-]+", "-", f"{device}-{target['version']}")
            name = f"local-plcnext-{slug}-{suffix}"
            label = f"Local {device} {version}"
            environment = {
                "PLCNEXT_SDK_ROOT": root.as_posix(),
                "ARP_DEVICE": device,
                "ARP_DEVICE_VERSION": version,
                "ARP_DEVICE_SHORT_VERSION": target["shortVersion"],
            }
            cache = {"CMAKE_BUILD_TYPE": "Release"}
            if make:
                cache["CMAKE_MAKE_PROGRAM"] = make.as_posix()
                environment["ARP_SDK_PACKAGE_NAME"] = make.relative_to(root / "sysroots").parts[0]
            configure.append({
                "name": name,
                "displayName": label,
                "description": f"Installed SDK: {root.as_posix()}",
                "inherits": "windows-AXCF-default" if windows else "linux-AXCF-default",
                "binaryDir": "${sourceDir}/bin/build/${presetName}",
                "environment": environment,
                "cacheVariables": cache,
                "vendor": {OWNER: {}},
            })
            build.append({
                "name": name,
                "displayName": label,
                "configurePreset": name,
                "vendor": {OWNER: {}},
            })
    return configure, build


def update_document(document, configure, build):
    document.setdefault("version", 4)
    for key, generated in (("configurePresets", configure), ("buildPresets", build)):
        retained = [p for p in document.get(key, []) if OWNER not in p.get("vendor", {})]
        collisions = {p["name"] for p in retained} & {p["name"] for p in generated}
        if collisions:
            raise ValueError(f"Existing user preset names conflict: {sorted(collisions)}")
        document[key] = retained + generated
    return document


def main():
    if sys.platform not in ("win32", "linux"):
        raise ValueError("Only Windows and Linux hosts are supported")
    result = subprocess.run(["plcncli", "get", "sdks"], capture_output=True, text=True,
                            encoding="utf-8-sig", errors="replace", check=True)
    sdks = json.loads(result.stdout)["sdks"]
    configure, build = generate_presets(sdks, sys.platform == "win32")
    output = ROOT / "CMakeUserPresets.json"
    document = json.loads(output.read_text(encoding="utf-8-sig")) if output.exists() else {}
    document = update_document(document, configure, build)
    temporary = output.with_suffix(".json.tmp")
    try:
        temporary.write_text(json.dumps(document, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
        os.replace(temporary, output)
    finally:
        temporary.unlink(missing_ok=True)
    print(f"Updated {output}: {len(configure)} installed SDK target(s).")
    for preset in configure:
        print(f"  {preset['displayName']}\n    cmake --preset {preset['name']}")
        print(f"    cmake --build --preset {preset['name']}")


if __name__ == "__main__":
    try:
        main()
    except (OSError, ValueError, KeyError, TypeError, subprocess.CalledProcessError) as error:
        print(f"Error: {error}", file=sys.stderr)
        if isinstance(error, subprocess.CalledProcessError):
            print(error.stderr or error.stdout, file=sys.stderr)
        sys.exit(1)
