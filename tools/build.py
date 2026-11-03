#!/usr/bin/env python3
"""Select CMake build presets and build the install target (Python 3.9+)."""

import argparse
import json
from pathlib import Path
import re
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]


def read_build_presets(root):
    """Read included files and resolve build inheritance; CMake validates presets."""
    presets, visited = {}, set()

    def read(path):
        path = path.resolve()
        if path in visited:
            return
        visited.add(path)
        data = json.loads(path.read_text(encoding="utf-8-sig"))
        for include in data.get("include", []):
            read(path.parent / include)
        for preset in data.get("buildPresets", []):
            presets[preset["name"]] = preset

    read(root / "CMakePresets.json")
    if (root / "CMakeUserPresets.json").exists():
        read(root / "CMakeUserPresets.json")

    def resolve(name, chain=()):
        if name in chain:
            raise ValueError(f"Cyclic build preset inheritance: {name}")
        preset = presets[name]
        parents = preset.get("inherits", [])
        if isinstance(parents, str):
            parents = [parents]
        result = {}
        for parent in reversed(parents):
            result.update(resolve(parent, (*chain, name)))
        result.update(preset)
        return result

    return {name: resolve(name) for name in presets}


def listed_presets(cmake, kind):
    result = subprocess.run(
        [cmake, f"--list-presets={kind}"], cwd=ROOT,
        text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
    )
    if result.returncode:
        raise ValueError(result.stderr.strip() or result.stdout.strip())
    return re.findall(r'^\s*"([^"]+)"', result.stdout, re.MULTILINE)


def select_presets(tokens, available):
    selected = []
    for token in tokens:
        for item in re.split(r"[,\s]+", token.strip()):
            if not item:
                continue
            if item.lower() == "all":
                names = available
            elif item.isdigit() and 1 <= int(item) <= len(available):
                names = [available[int(item) - 1]]
            elif item in available:
                names = [item]
            else:
                raise ValueError(f"Unknown or unavailable preset: {item}")
            for name in names:
                if name not in selected:
                    selected.append(name)
    if not selected:
        raise ValueError("No presets selected.")
    return selected


def build_commands(cmake, name, preset, jobs, skip_configure, configure_args):
    commands = []
    if not skip_configure:
        commands.append([cmake, "--preset", preset["configurePreset"], *configure_args])
    commands.append([cmake, "--build", "--preset", name,
                     "--target", "install", "--parallel", str(jobs)])
    return commands


def run_builds(selected, presets, settings, dry_run):
    failed = []
    for name in selected:
        print(f"\n=== {presets[name].get('displayName', name).strip()} ===", flush=True)
        for command in build_commands(settings["cmake"], name, presets[name],
                                      settings["jobs"], settings["skip_configure"],
                                      settings["configure_args"]):
            print(subprocess.list2cmdline(command), flush=True)
            if not dry_run:
                result = subprocess.run(command, cwd=ROOT)
                if result.returncode:
                    failed.append(name)
                    print(f"FAILED: {name} (exit {result.returncode})", file=sys.stderr)
                    break
        if failed and not settings["keep_going"]:
            break
    if failed:
        print("Failed presets: " + ", ".join(failed), file=sys.stderr)
        return 1
    print("\n" + ("Preview complete." if dry_run else "Build and install complete."))
    return 0


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("presets", nargs="*", help="Build preset names or menu numbers; 'all' selects all")
    parser.add_argument("--list", action="store_true", help="List available presets and exit")
    parser.add_argument("--settings", type=Path, help="Load settings from JSON")
    parser.add_argument("--save-settings", type=Path, help="Save the selection and options to JSON")
    parser.add_argument("--jobs", type=int, help="Parallel jobs per build (default: 4)")
    parser.add_argument("--cmake", help="CMake executable path (default: cmake)")
    parser.add_argument("--skip-configure", action="store_true", default=None)
    parser.add_argument("--keep-going", action="store_true", default=None, help="Continue after a failed preset")
    parser.add_argument("--configure-arg", action="append", help="Extra configure argument, e.g. --configure-arg=-DFOO=ON")
    parser.add_argument("--dry-run", action="store_true", help="Print commands without configuring/building/installing")
    args = parser.parse_args(argv)
    settings = dict(cmake="cmake", jobs=4, skip_configure=False, keep_going=False,
                    configure_args=[], presets=[])
    if args.settings:
        loaded = json.loads(args.settings.read_text(encoding="utf-8-sig"))
        if not isinstance(loaded, dict) or set(loaded) - set(settings):
            raise ValueError("Settings must be a JSON object with supported option names.")
        settings.update(loaded)
    for key in ("cmake", "jobs", "skip_configure", "keep_going"):
        if getattr(args, key) is not None:
            settings[key] = getattr(args, key)
    if args.configure_arg is not None:
        settings["configure_args"] = args.configure_arg
    if type(settings["jobs"]) is not int or settings["jobs"] < 1:
        raise ValueError("jobs must be a positive integer.")
    if not isinstance(settings["cmake"], str) or not settings["cmake"]:
        raise ValueError("cmake must be an executable name or path.")
    for key in ("skip_configure", "keep_going"):
        if type(settings[key]) is not bool:
            raise ValueError(f"{key} must be a boolean.")
    for key in ("presets", "configure_args"):
        if not isinstance(settings[key], list) or not all(isinstance(x, str) for x in settings[key]):
            raise ValueError(f"{key} must be an array of strings.")

    presets = read_build_presets(ROOT)
    configured = set(listed_presets(settings["cmake"], "configure"))
    available = [name for name in listed_presets(settings["cmake"], "build")
                 if presets[name].get("configurePreset") in configured]
    if not available:
        raise ValueError("No available build presets on this host.")
    tokens = args.presets or settings["presets"]
    if args.list or not tokens:
        for number, name in enumerate(available, 1):
            print(f"{number:2}. {presets[name].get('displayName', name).strip()}\n    {name}")
    if args.list:
        return 0
    if not tokens:
        tokens = [input("Select presets (numbers/names separated by spaces or commas; all): ")]
    selected = select_presets(tokens, available)
    settings["presets"] = selected
    if args.save_settings:
        args.save_settings.write_text(json.dumps(settings, indent=2) + "\n", encoding="utf-8")
        print(f"Settings saved: {args.save_settings}")
    return run_builds(selected, presets, settings, args.dry_run)


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (OSError, ValueError, KeyError, EOFError) as exc:
        print(f"Error: {exc}", file=sys.stderr)
        sys.exit(1)
    except KeyboardInterrupt:
        print("\nCancelled.", file=sys.stderr)
        sys.exit(130)
