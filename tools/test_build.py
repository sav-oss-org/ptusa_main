"""Build orchestration checks, without compiling or installing the project."""

import contextlib
import io
import json
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch
import subprocess

import build


class BuildTests(unittest.TestCase):
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


if __name__ == "__main__":
    unittest.main()
