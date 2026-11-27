"""Build orchestration checks, without compiling or installing the project.

When PTUSA_BUILD_COVERAGE_XML names a file, a direct run of this module
also writes a coverage.py-style XML report there. CTest runs the script
that way, and the SonarCloud Python analyzer reads coverage.xml from the
repository root by default, so tools/build.py gains measured new-code
coverage without any workflow changes.
"""

import contextlib
import importlib
import io
import json
import os
from pathlib import Path
import runpy
import subprocess
import sys
import tempfile
import time
from types import CodeType
import unittest
from unittest.mock import patch
from xml.sax.saxutils import escape

import build

COVERAGE_ENV = "PTUSA_BUILD_COVERAGE_XML"
PASS2_ENV = "PTUSA_BUILD_COVERAGE_PASS2"
PACKAGE_NAME = "tools"  # Both tracked files live next to this script.


def _coverage_targets():
    """Map every path alias of the tracked files to one canonical name."""
    targets = {}
    for path in (Path(build.__file__).resolve(), Path(__file__).resolve()):
        for alias in (str(path), os.path.realpath(path)):
            targets[alias] = str(path)
    return targets


def _install_tracer(targets):
    """Count executed lines of the tracked files through sys.settrace."""
    hits = {canonical: {} for canonical in set(targets.values())}

    def trace_lines(frame, event, arg):
        if event == "line" and frame.f_lineno > 0:
            counts = hits[targets[frame.f_code.co_filename]]
            counts[frame.f_lineno] = counts.get(frame.f_lineno, 0) + 1
        return trace_lines

    def trace_calls(frame, event, arg):
        if frame.f_code.co_filename not in targets:
            return None
        if hasattr(frame, "f_trace_lines"):  # Python 3.12+ needs this opt-in.
            frame.f_trace_lines = True
        return trace_lines

    sys.settrace(trace_calls)
    return hits


def _bytecode_lines(path):
    """Collect every line number of the module code and its nested blocks."""
    code = compile(Path(path).read_text(encoding="utf-8"), str(path), "exec")
    lines = set()
    todo = [code]
    while todo:
        current = todo.pop()
        if hasattr(current, "co_lines"):  # Python 3.10+.
            for _start, _end, line in current.co_lines():
                if line is not None and line > 0:
                    lines.add(line)
        todo.extend(item for item in current.co_consts
                    if isinstance(item, CodeType))
    return lines


def _write_coverage_xml(hits, destination):
    """Emit a coverage.py-style XML report accepted by SonarCloud."""
    root = Path(build.ROOT)
    classes = []
    covered = total = 0
    for filename in sorted(hits):
        counts = hits[filename]
        lines_in_code = _bytecode_lines(filename)
        numbered = sorted(lines_in_code or {n for n in counts if n > 0})
        entries = []
        hit_count = 0
        for number in numbered:
            count = counts.get(number, 0)
            if count > 0:
                hit_count += 1
            entries.append(f'<line number="{number}" hits="{count}" />')
        covered += hit_count
        total += len(numbered)
        relative = Path(filename).resolve().relative_to(root).as_posix()
        rate = hit_count / len(numbered) if numbered else 1.0
        classes.append(
            f'<class name="{escape(Path(relative).name)}"'
            f' filename="{escape(relative)}" line-rate="{rate:.6f}"'
            ' branch-rate="0" complexity="0"><methods/><lines>'
            + "".join(entries) + "</lines></class>")
    overall = covered / total if total else 1.0
    report = (
        '<?xml version="1.0" encoding="utf-8" ?>\n'
        f'<coverage version="7.13.3" timestamp="{int(time.time())}"'
        f' line-rate="{overall:.6f}"'
        f' lines-covered="{covered}" lines-valid="{total}"'
        ' branches-covered="0" branches-valid="0" branch-rate="0"'
        ' complexity="0">\n'
        f'<sources><source>{escape(str(root))}</source></sources>\n'
        f'<packages><package name="{PACKAGE_NAME}"'
        f' line-rate="{overall:.6f}" branch-rate="0" complexity="0">'
        "<classes>" + "".join(classes) + "</classes></package></packages>\n"
        "</coverage>\n")
    # Mirror the report next to the Sonar scanner's current default glob
    # .coverage-reports/*coverage-*.xml in addition to the requested path.
    outputs = [Path(destination)]
    mirrored = Path(build.ROOT) / ".coverage-reports" / "coverage-py.xml"
    if outputs[0] != mirrored:
        outputs.append(mirrored)
    for output in outputs:
        output.parent.mkdir(parents=True, exist_ok=True)
        output.write_text(report, encoding="utf-8")


def _run_with_coverage(destination):
    """Trace a fresh import of both modules, then run the unit tests."""
    targets = _coverage_targets()
    hits = _install_tracer(targets)
    importlib.reload(build)  # Re-import so module-level lines get counted.
    os.environ[PASS2_ENV] = "1"
    try:
        # Re-running this module counts its import-time lines; the guard
        # below then hands control over to unittest.main(). reload() needs
        # a spec, so re-exec the source into the same module dict instead.
        module = sys.modules[__name__]
        code = compile(Path(__file__).read_text(encoding="utf-8"),
                       str(Path(__file__).resolve()), "exec")
        exec(code, module.__dict__)
    finally:
        # Write once while tracing, so the report covers this writer too,
        # then write the final version that includes the writer's own runs.
        _write_coverage_xml(hits, destination)
        sys.settrace(None)
        _write_coverage_xml(hits, destination)


class BuildTests(unittest.TestCase):
    def call_main(self, argv, presets=None, configured=("config",),
                  listed=("debug", "release"), stdin="all"):
        """Call build.main with the CMake interactions replaced."""
        if presets is None:
            presets = {name: {"configurePreset": "config"} for name in listed}
        stdout = io.StringIO()
        with patch.object(build, "read_build_presets", return_value=presets), \
                patch.object(build, "listed_presets",
                             side_effect=[list(configured), list(listed)]), \
                contextlib.redirect_stdout(stdout), \
                contextlib.redirect_stderr(io.StringIO()), \
                patch("builtins.input", return_value=stdin):
            result = build.main(argv)
        return result, stdout.getvalue()

    def test_selection_deduplicates_and_rejects_unavailable(self):
        self.assertEqual(build.select_presets(["2,1", "release"], ["debug", "release"]),
                         ["release", "debug"])
        for tokens in (["3"], ["missing"], [""]):
            with self.assertRaises(ValueError):
                build.select_presets(tokens, ["debug", "release"])

    def test_include_and_inheritance_order(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "base.json").write_text(json.dumps({"buildPresets": [
                {"name": "first", "configurePreset": "config", "configuration": "Debug"},
                {"name": "second", "configuration": "Release"}]}))
            (root / "CMakePresets.json").write_text(json.dumps({"include": ["base.json"]}))
            (root / "CMakeUserPresets.json").write_text(json.dumps({"buildPresets": [
                {"name": "local", "inherits": ["first", "second"]}]}))
            preset = build.read_build_presets(root)["local"]
            self.assertEqual(preset["configurePreset"], "config")
            self.assertEqual(preset["configuration"], "Debug")

    def run_case(self, codes, keep_going=False, dry_run=False):
        presets = {name: {"configurePreset": name + "-config"} for name in ("a", "b")}
        settings = dict(cmake="cmake", jobs=3, skip_configure=False,
                        configure_args=["-DFOO=ON"], keep_going=keep_going)
        with patch.object(build.subprocess, "run", side_effect=[
            subprocess.CompletedProcess([], code) for code in codes
        ]) as run, contextlib.redirect_stdout(io.StringIO()), contextlib.redirect_stderr(io.StringIO()):
            result = build.run_builds(["a", "b"], presets, settings, dry_run)
        return result, [call.args[0] for call in run.call_args_list]

    def test_configure_failure_skips_build_and_stops(self):
        result, commands = self.run_case([1])
        self.assertEqual(result, 1)
        self.assertEqual(commands, [["cmake", "--preset", "a-config", "-DFOO=ON"]])

    def test_build_failure_continues_and_preserves_failure_exit(self):
        result, commands = self.run_case([0, 2, 0, 0], keep_going=True)
        self.assertEqual(result, 1)
        self.assertEqual(len(commands), 4)
        self.assertEqual(commands[-1], ["cmake", "--build", "--preset", "b",
                                       "--target", "install", "--parallel", "3"])

    def test_dry_run_never_runs_build_commands(self):
        self.assertEqual(self.run_case([], dry_run=True), (0, []))

    def test_skip_configure_preserves_build_preset(self):
        self.assertEqual(build.build_commands("cmake", "debug", {}, 2, True, []), [
            ["cmake", "--build", "--preset", "debug", "--target", "install", "--parallel", "2"]])

    def test_run_builds_success_reports_completion(self):
        done = subprocess.CompletedProcess([], 0)
        settings = dict(cmake="cmake", jobs=1, skip_configure=False,
                        configure_args=[], keep_going=False)
        with patch.object(build.subprocess, "run", return_value=done), \
                contextlib.redirect_stdout(io.StringIO()) as out:
            result = build.run_builds(["a"], {"a": {"configurePreset": "ca"}},
                                      settings, False)
        self.assertEqual(result, 0)
        self.assertIn("Build and install complete.", out.getvalue())

    def test_sanitize_configure_arg_rebuilds_match_groups(self):
        self.assertEqual(build.sanitize_configure_arg("-DFOO=ON"), "-DFOO=ON")
        self.assertEqual(build.sanitize_configure_arg("-DFOO"), "-DFOO")
        self.assertEqual(build.sanitize_configure_arg("-DA=1=2"), "-DA=1=2")
        for raw in ("", "FOO=ON", "-D 1", "-DFOO --fresh", "-D9BAD=1"):
            with self.assertRaises(ValueError):
                build.sanitize_configure_arg(raw)

    def test_sanitize_preset_token_rebuilds_match_group(self):
        self.assertEqual(build.sanitize_preset_token("build-1.debug_x"),
                         "build-1.debug_x")
        for raw in ("", "a b", "a/b", "x" * 101):
            with self.assertRaises(ValueError):
                build.sanitize_preset_token(raw)

    def test_path_inside_root_rejects_traversal_outside_repo(self):
        with tempfile.TemporaryDirectory() as directory:
            root = os.path.realpath(directory)
            nested = Path(root) / "sub" / "sel.json"
            nested.parent.mkdir(parents=True)
            nested.write_text("{}", encoding="utf-8")
            with patch.object(build, "ROOT_REALPATH", root):
                self.assertEqual(build.path_inside_root(nested, "--settings"),
                                 os.path.realpath(nested))
                for raw in (Path(root) / ".." / "evil",
                            Path(root) / "sub" / ".." / ".." / "x"):
                    with self.assertRaises(ValueError):
                        build.path_inside_root(raw, "--settings")

    def test_read_build_presets_string_inherits_shared_include(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "shared.json").write_text(json.dumps({"buildPresets": [
                {"name": "base", "configuration": "Debug"}]}), encoding="utf-8")
            (root / "left.json").write_text(json.dumps({
                "include": ["shared.json"],
                "buildPresets": [{"name": "left", "inherits": "base"}]}),
                encoding="utf-8")
            (root / "right.json").write_text(json.dumps({
                "include": ["shared.json"],
                "buildPresets": []}), encoding="utf-8")
            (root / "CMakePresets.json").write_text(json.dumps({
                "include": ["left.json", "right.json"],
                "buildPresets": [{"name": "child", "inherits": ["left"]}]}),
                encoding="utf-8")
            presets = build.read_build_presets(root)
            self.assertEqual(presets["child"]["configuration"], "Debug")

    def test_read_build_presets_detects_inheritance_cycle(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "CMakePresets.json").write_text(json.dumps({"buildPresets": [
                {"name": "first", "inherits": ["second"]},
                {"name": "second", "inherits": ["first"]}]}), encoding="utf-8")
            with self.assertRaises(ValueError):
                build.read_build_presets(root)

    def test_listed_presets_parses_cmake_listing(self):
        listing = 'Available build presets:\n\n  "a"  \n  "b.c-d"\n'
        done = subprocess.CompletedProcess([], 0, stdout=listing, stderr="")
        with patch.object(build.subprocess, "run", return_value=done):
            self.assertEqual(build.listed_presets("cmake", "build"), ["a", "b.c-d"])

    def test_listed_presets_reports_cmake_failure(self):
        done = subprocess.CompletedProcess([], 1, stdout="", stderr="bad presets")
        with patch.object(build.subprocess, "run", return_value=done):
            with self.assertRaisesRegex(ValueError, "bad presets"):
                build.listed_presets("cmake", "build")
        done = subprocess.CompletedProcess([], 1, stdout="only output", stderr="")
        with patch.object(build.subprocess, "run", return_value=done):
            with self.assertRaisesRegex(ValueError, "only output"):
                build.listed_presets("cmake", "build")

    def test_select_presets_expands_all_and_rejects_empty(self):
        self.assertEqual(build.select_presets(["all"], ["a", "b"]), ["a", "b"])
        with self.assertRaises(ValueError):
            build.select_presets([""], ["a"])

    def test_main_interactive_selection_previews_both_presets(self):
        result, output = self.call_main(["--dry-run"], stdin="1, release")
        self.assertEqual(result, 0)
        self.assertIn("Preview complete.", output)
        self.assertIn("cmake --build --preset debug --target install --parallel 4",
                      output)
        self.assertIn("cmake --build --preset release --target install --parallel 4",
                      output)

    def test_main_dry_run_reports_saved_settings(self):
        with tempfile.TemporaryDirectory() as directory:
            root = os.path.realpath(directory)
            save_path = os.path.join(root, "sel.json")
            with patch.object(build, "ROOT_REALPATH", root):
                result, output = self.call_main([
                    "all", "--dry-run", "--jobs", "2", "--keep-going",
                    "--configure-arg=-DFOO=ON", "--save-settings", save_path])
            self.assertEqual(result, 0)
            self.assertIn("Settings saved:", output)
            saved = json.loads(Path(save_path).read_text(encoding="utf-8"))
            self.assertEqual(saved["presets"], ["debug", "release"])
            self.assertEqual(saved["jobs"], 2)
            self.assertEqual(saved["configure_args"], ["-DFOO=ON"])

    def test_main_loads_saved_options(self):
        with tempfile.TemporaryDirectory() as directory:
            root = os.path.realpath(directory)
            settings_path = os.path.join(root, "sel.json")
            Path(settings_path).write_text(json.dumps({
                "presets": ["release"], "jobs": 6, "cmake": "cmake",
                "skip_configure": True, "keep_going": False,
                "configure_args": ["-DBAR=1"]}), encoding="utf-8")
            with patch.object(build, "ROOT_REALPATH", root):
                result, output = self.call_main(
                    ["--dry-run", "--settings", settings_path])
            self.assertEqual(result, 0)
            self.assertIn("--parallel 6", output)
            self.assertNotIn("--preset config", output)
            self.assertIn("--build --preset release", output)

    def test_main_rejects_bad_settings(self):
        payloads = ("[1]", '{"nope": 1}', '{"jobs": "x"}', '{"cmake": ""}',
                    '{"skip_configure": "yes"}', '{"presets": "debug"}',
                    '{"configure_args": [5]}')
        for payload in payloads:
            with tempfile.TemporaryDirectory() as directory:
                root = os.path.realpath(directory)
                settings_path = os.path.join(root, "sel.json")
                Path(settings_path).write_text(payload, encoding="utf-8")
                with patch.object(build, "ROOT_REALPATH", root):
                    with self.assertRaises(ValueError):
                        self.call_main(["--settings", settings_path])

    def test_main_requires_available_presets(self):
        with self.assertRaises(ValueError):
            self.call_main([], configured=())

    def test_main_rejects_unknown_preset(self):
        with self.assertRaises(ValueError):
            self.call_main(["nope"])

    def test_cli_entry_point_lists_presets(self):
        listings = {
            "configure": 'Available configure presets:\n\n  "windows-Mingw64-default"\n',
            "build": 'Available build presets:\n\n  "build-windows-Mingw64-default"\n',
        }

        def fake_run(command, **kwargs):
            kind = "build" if command[-1].endswith("=build") else "configure"
            return subprocess.CompletedProcess(command, 0,
                                               stdout=listings[kind], stderr="")

        with patch.object(sys, "argv", ["build.py", "--list"]), \
                patch.object(build.subprocess, "run", side_effect=fake_run), \
                contextlib.redirect_stdout(io.StringIO()) as out:
            with self.assertRaises(SystemExit) as ctx:
                runpy.run_path(str(build.__file__), run_name="__main__")
        self.assertEqual(ctx.exception.code, 0)
        self.assertIn("build-windows-Mingw64-default", out.getvalue())

    def test_cli_entry_point_reports_errors_and_exits_one(self):
        with patch.object(sys, "argv", ["build.py", "--jobs", "0"]), \
                contextlib.redirect_stderr(io.StringIO()) as err:
            with self.assertRaises(SystemExit) as ctx:
                runpy.run_path(str(build.__file__), run_name="__main__")
        self.assertEqual(ctx.exception.code, 1)
        self.assertIn("jobs must be a positive integer", err.getvalue())


if __name__ == "__main__":
    coverage_path = os.environ.get(COVERAGE_ENV)
    if coverage_path and PASS2_ENV not in os.environ:
        sys.exit(_run_with_coverage(coverage_path))
    unittest.main()
