"""Проверка аргументов и команд службы без запуска контроллера."""

import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest


SHELL = os.environ.get("PTUSA_TEST_SHELL") or shutil.which("sh")
SCRIPT = Path(__file__).with_name("setup-ptusa-autostart.sh")


@unittest.skipUnless(SHELL, "Для проверки нужен POSIX sh (PTUSA_TEST_SHELL).")
class AutostartTests(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory(prefix="ptusa-autostart-")
        self.addCleanup(self.temporary.cleanup)
        self.directory = Path(self.temporary.name)
        self.defaults = self.directory / "defaults"
        self.shortcuts = self.directory / "commands"
        self.legacy_shortcuts = self.directory / "legacy-commands"
        self.project = self.directory / "main"
        self.project.mkdir()
        self.application = self.directory / "ptusamain"
        self.application.mkdir()
        self.config = self.project / "ptusa_main.ini"
        for filename in ("ptusa_main", "libAxiobus.so.11"):
            (self.application / filename).touch()
        (self.project / "main.plua").touch()
        self.installation = self.directory / "installation files"
        self.installation.mkdir()
        self.installer = self.installation / SCRIPT.name
        self.installer.write_bytes(SCRIPT.read_bytes())
        (self.installation / "ptusa_main").write_bytes(b"new executable\n")
        (self.installation / "libAxiobus.so.11").write_bytes(b"new library\n")
        self.service = self.directory / "service.sh"
        self.service.write_text(
            "#!/bin/sh\n# ptusa_main-autostart-v1\n"
            "printf 'service\\n'\nprintf '%s\\n' \"$@\"\n",
            encoding="utf-8")
        self.utilities = self.directory / "utilities"
        self.utilities.mkdir()
        # Подменяем только проверку UID и su: реальные права не повышаются.
        (self.utilities / "id").write_text(
            '#!/bin/sh\nprintf \'%s\\n\' "${PTUSA_TEST_UID:-0}"\n',
            encoding="utf-8")
        (self.utilities / "su").write_text('''#!/bin/sh
printf 'su\\n' >> "$PTUSA_TEST_SU_LOG"
if [ -n "${PTUSA_TEST_SU_FAILURE:-}" ]; then
    exit "$PTUSA_TEST_SU_FAILURE"
fi
[ "$1" = -s ] || exit 90
shell=$2
shift 2
[ "$1" = -c ] || exit 91
command=$2
shift 2
[ "$1" = root ] || exit 92
shift
export PTUSA_TEST_UID=0
exec "$shell" -c "$command" "$@"
''', encoding="utf-8")
        for utility in self.utilities.iterdir():
            utility.chmod(0o755)

    def run_shell(self, program, check=True, installer=None):
        environment = os.environ.copy()
        environment["PTUSA_TEST_SCRIPT"] = SCRIPT.as_posix()
        environment["PTUSA_TEST_DIRECTORY"] = self.directory.as_posix()
        environment["PTUSA_TEST_UID"] = "0"
        environment["PTUSA_TEST_WINDOWS"] = "1" if os.name == "nt" else "0"
        environment["PTUSA_TEST_SU_LOG"] = (
            self.directory / "su.log").as_posix()
        # Переопределяем системные пути: все файлы создаются во временной папке.
        result = subprocess.run(
            [SHELL, "-c", '''
set -- help
. "$PTUSA_TEST_SCRIPT" >/dev/null
DEFAULT_SHORTCUT_DIR=$SHORTCUT_DIR
DEFAULT_SERVICE_PATH=$PATH
DEFAULT_APP_DIR=$APP_DIR
DEFAULT_WORK_DIR=$WORK_DIR
DEFAULT_EXECUTABLE=$EXECUTABLE
DEFAULT_CONFIG_FILE=$CONFIG_FILE
APP_DIR=$PTUSA_TEST_DIRECTORY/ptusamain
WORK_DIR=$PTUSA_TEST_DIRECTORY/main
EXECUTABLE=$APP_DIR/ptusa_main
CONFIG_FILE=$WORK_DIR/ptusa_main.ini
DEFAULTS=$PTUSA_TEST_DIRECTORY/defaults
SHORTCUT_DIR=$PTUSA_TEST_DIRECTORY/commands
LEGACY_SHORTCUT_DIR=$PTUSA_TEST_DIRECTORY/legacy-commands
SERVICE=$PTUSA_TEST_DIRECTORY/service.sh
SYSV_SERVICE=$SERVICE
SYSTEMD_HELPER=$PTUSA_TEST_DIRECTORY/systemd-helper.sh
SYSTEMD_UNIT=$PTUSA_TEST_DIRECTORY/ptusa_main.service
SYSTEMD_RUNTIME_DIR=$PTUSA_TEST_DIRECTORY/systemd
PLCNEXT_SERVICE=$PTUSA_TEST_DIRECTORY/plcnext
touch "$PLCNEXT_SERVICE"
PID_FILE=$PTUSA_TEST_DIRECTORY/ptusa_main.pid
LOG_FILE=$PTUSA_TEST_DIRECTORY/ptusa_main.log
PATH=$(cd "$PTUSA_TEST_DIRECTORY/utilities" && pwd):$PATH
export PATH
chown() { :; }
''' + program, (installer or SCRIPT).as_posix()],
            env=environment, capture_output=True,
            text=True, encoding="utf-8", timeout=10)
        if check:
            self.assertEqual(result.returncode, 0, result.stderr)
        return result

    def run_installation(self, program="", check=True, installer=None):
        return self.run_shell('''
check_requirements() { detect_init_system; }
start-stop-daemon() { return 1; }
plcnext_running() { return 1; }
prepare_permissions() { :; }
update-rc.d() { :; }
install() {
    printf '%s\\n' "$*" >> "$PTUSA_TEST_DIRECTORY/install.log"
    # Windows не поддерживает группу с правом записи для POSIX-каталогов.
    # Проверяем запрошенные права в логе, а сам каталог создаём с 755.
    if [ "$PTUSA_TEST_WINDOWS" = 1 ] && [ "$1" = -d ] && [ "$3" = 775 ]; then
        shift 3
        command install -d -m 755 "$@"
    else
        command install "$@"
    fi
}
chown() { printf '%s\\n' "$*" >> "$PTUSA_TEST_DIRECTORY/chown.log"; }
''' + program + '\ninstall_service\n', check=check,
                              installer=installer or self.installer)

    def load_arguments(self):
        result = self.run_shell('load_defaults\nprintf \'%s\\n\' "$PTUSA_ARGS"')
        return result.stdout.strip()

    def write_defaults(self, arguments):
        self.defaults.write_text(
            "PTUSA_ARGS='" + arguments + "'\n", encoding="utf-8")

    def test_shell_syntax(self):
        result = subprocess.run([SHELL, "-n", SCRIPT.as_posix()],
                                capture_output=True, text=True, timeout=10)
        self.assertEqual(result.returncode, 0, result.stderr)

    def test_default_application_and_work_directories_are_separate(self):
        result = self.run_shell('''
printf '%s\\n' "$DEFAULT_APP_DIR" "$DEFAULT_WORK_DIR" \
    "$DEFAULT_EXECUTABLE" "$DEFAULT_CONFIG_FILE"
''')
        self.assertEqual(result.stdout.splitlines(), [
            "/opt/ptusamain", "/opt/main", "/opt/ptusamain/ptusa_main",
            "/opt/main/ptusa_main.ini"])

    def test_files_are_checked_in_separate_directories(self):
        self.run_shell("check_application_files")

    def test_script_in_application_directory_does_not_replace_work_script(self):
        (self.project / "main.plua").unlink()
        (self.application / "main.plua").touch()
        result = self.run_shell("check_application_files", check=False)
        self.assertNotEqual(result.returncode, 0)
        self.assertIn((self.project / "main.plua").as_posix(), result.stderr)

    def test_library_in_work_directory_does_not_replace_application_library(self):
        (self.application / "libAxiobus.so.11").unlink()
        (self.project / "libAxiobus.so.11").touch()
        result = self.run_shell("check_application_files", check=False)
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("libAxiobus.so.11", result.stderr)

    def test_start_uses_application_binary_and_project_working_directory(self):
        self.config.touch()
        self.run_shell('''
check_files() { check_application_files; }
prepare_permissions() { :; }
plcnext_running() { return 1; }
process_running() { [ -f "$PID_FILE" ]; }
sleep() { :; }
start-stop-daemon() {
    [ "$1" = --start ] || return 1
    printf '%s\\n' "$@" > "$PTUSA_TEST_DIRECTORY/start-arguments"
    printf '123\\n' > "$PID_FILE"
}
start_service
''')
        arguments = (self.directory / "start-arguments").read_text().splitlines()
        self.assertEqual(arguments[arguments.index("--exec") + 1],
                         (self.application / "ptusa_main").as_posix())
        self.assertEqual(arguments[arguments.index("--chdir") + 1],
                         self.project.as_posix())
        self.assertEqual(arguments[arguments.index("--") + 1:],
                         ["--config", self.config.as_posix()])

    def test_start_rejects_manually_started_legacy_binary(self):
        result = self.run_shell('''
check_files() { check_application_files; }
plcnext_running() { return 1; }
running() { return 1; }
prepare_permissions() { echo 'permissions changed'; }
start-stop-daemon() {
    [ "$*" = "--status --exec $WORK_DIR/ptusa_main" ]
}
start_service
''', check=False)
        self.assertNotEqual(result.returncode, 0)
        self.assertIn((self.project / "ptusa_main").as_posix(), result.stderr)
        self.assertEqual(result.stdout, "")

    def test_reinstall_stops_previous_service_before_preparing_directories(self):
        self.write_defaults("main.plua --opc off")
        original_defaults = self.defaults.read_bytes()
        self.config.write_text("sleep_time=2\n", encoding="utf-8")
        original_config = self.config.read_bytes()
        self.service.write_text('''#!/bin/sh
# ptusa_main-autostart-v1
[ "$1" = stop ] || exit 90
printf 'stopped\\n' > "$PTUSA_TEST_DIRECTORY/previous-stopped"
''', encoding="utf-8")
        self.run_shell('''
check_requirements() { :; }
readlink() { printf '%s\\n' "$PTUSA_TEST_SCRIPT"; }
start-stop-daemon() { return 1; }
plcnext_running() { return 1; }
prepare_permissions() {
    [ -f "$PTUSA_TEST_DIRECTORY/previous-stopped" ] || exit 91
}
update-rc.d() { :; }
install_service
''')
        self.assertEqual(self.service.read_bytes(), SCRIPT.read_bytes())
        self.assertEqual(self.defaults.read_bytes(), original_defaults)
        self.assertEqual(self.config.read_bytes(), original_config)
        self.assertEqual(len(list(self.shortcuts.iterdir())), 5)

    def test_reinstall_aborts_if_previous_service_cannot_stop(self):
        self.service.write_text(
            "#!/bin/sh\n# ptusa_main-autostart-v1\nexit 17\n",
            encoding="utf-8")
        original_service = self.service.read_bytes()
        result = self.run_shell('''
check_requirements() { :; }
readlink() { printf '%s\\n' "$PTUSA_TEST_SCRIPT"; }
prepare_permissions() { echo 'permissions changed'; }
install_service
''', check=False)
        self.assertEqual(result.returncode, 17)
        self.assertEqual(result.stdout, "")
        self.assertEqual(self.service.read_bytes(), original_service)
        self.assertFalse(self.shortcuts.exists())

    def test_fresh_install_creates_directories_and_copies_payload(self):
        shutil.rmtree(self.application)
        shutil.rmtree(self.project)
        (self.installation / "ptusa_main.ini").write_bytes(b"sleep_time=2\n")
        self.run_installation()
        for filename in ("ptusa_main", "libAxiobus.so.11", SCRIPT.name):
            self.assertEqual((self.application / filename).read_bytes(),
                             (self.installation / filename).read_bytes())
        self.assertEqual(self.config.read_bytes(), b"sleep_time=2\n")
        self.assertTrue(self.project.is_dir())
        self.assertFalse((self.project / "main.plua").exists())
        installs = (self.directory / "install.log").read_text().splitlines()
        owners = (self.directory / "chown.log").read_text().splitlines()
        for directory in (self.application, self.project):
            self.assertIn("-d -m 775 " + directory.as_posix(), installs)
            self.assertIn("admin:plcnext " + directory.as_posix(), owners)

    def test_reinstall_stops_service_before_replacing_binary(self):
        binary = self.application / "ptusa_main"
        binary.write_bytes(b"old executable\n")
        self.service.write_text('''#!/bin/sh
# ptusa_main-autostart-v1
[ "$1" = stop ] || exit 90
grep -q '^old executable$' "$PTUSA_TEST_DIRECTORY/ptusamain/ptusa_main" || exit 91
touch "$PTUSA_TEST_DIRECTORY/previous-stopped"
''', encoding="utf-8")
        self.run_installation('''
install() {
    [ -f "$PTUSA_TEST_DIRECTORY/previous-stopped" ] || exit 92
    command install "$@"
}
''')
        self.assertEqual(binary.read_bytes(), b"new executable\n")
        owners = (self.directory / "chown.log").read_text().splitlines()
        for directory in (self.application, self.project):
            self.assertNotIn("admin:plcnext " + directory.as_posix(), owners)

    def test_failed_stop_preserves_binary_and_config(self):
        binary = self.application / "ptusa_main"
        binary.write_bytes(b"old executable\n")
        self.config.write_bytes(b"sleep_time=5\n")
        (self.installation / "ptusa_main.ini").write_bytes(b"sleep_time=2\n")
        self.service.write_text(
            "#!/bin/sh\n# ptusa_main-autostart-v1\nexit 17\n",
            encoding="utf-8")
        result = self.run_installation(check=False)
        self.assertEqual(result.returncode, 17)
        self.assertEqual(binary.read_bytes(), b"old executable\n")
        self.assertEqual(self.config.read_bytes(), b"sleep_time=5\n")
        self.assertFalse((self.directory / "install.log").exists())

    def test_install_preserves_existing_config(self):
        self.config.write_bytes(b"sleep_time=5\n")
        (self.installation / "ptusa_main.ini").write_bytes(b"sleep_time=2\n")
        self.run_installation()
        self.assertEqual(self.config.read_bytes(), b"sleep_time=5\n")

    def test_install_without_ini_keeps_config_optional(self):
        self.run_installation()
        self.assertFalse(self.config.exists())

    def test_install_from_application_directory_skips_copying_onto_itself(self):
        installer = self.application / SCRIPT.name
        installer.write_bytes(SCRIPT.read_bytes())
        (self.application / "ptusa_main.ini").write_bytes(b"sleep_time=2\n")
        self.run_installation(installer=installer)
        self.assertEqual((self.application / "ptusa_main").read_bytes(), b"")
        self.assertEqual(self.config.read_bytes(), b"sleep_time=2\n")
        self.assertEqual(installer.read_bytes(), SCRIPT.read_bytes())

    def test_install_uses_existing_library_if_payload_has_none(self):
        (self.installation / "libAxiobus.so.11").unlink()
        library = self.application / "libAxiobus.so.11"
        library.write_bytes(b"existing library\n")
        self.run_installation()
        self.assertEqual(library.read_bytes(), b"existing library\n")

    def test_missing_payload_aborts_before_stopping_service_or_creating_directories(self):
        for missing in ("ptusa_main", "libAxiobus.so.11"):
            with self.subTest(missing=missing):
                payload = self.installation / missing
                original = payload.read_bytes()
                payload.unlink()
                shutil.rmtree(self.application, ignore_errors=True)
                shutil.rmtree(self.project, ignore_errors=True)
                result = self.run_installation(check=False)
                self.assertNotEqual(result.returncode, 0)
                self.assertEqual(result.stdout, "")
                self.assertIn(missing, result.stderr)
                self.assertFalse(self.application.exists())
                self.assertFalse(self.project.exists())
                payload.write_bytes(original)

    def test_install_rejects_directory_symlink_before_stopping_service(self):
        shutil.rmtree(self.application)
        try:
            self.application.symlink_to(self.installation, target_is_directory=True)
        except OSError:
            self.skipTest("Создание символических ссылок недоступно.")
        result = self.run_installation(check=False)
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("не должен быть ссылкой", result.stderr)
        self.assertEqual(result.stdout, "")

    def test_install_preserves_dangling_config_symlink(self):
        try:
            self.config.symlink_to(self.directory / "missing.ini")
        except OSError:
            self.skipTest("Создание символических ссылок недоступно.")
        (self.installation / "ptusa_main.ini").write_bytes(b"sleep_time=2\n")
        self.run_installation()
        self.assertTrue(self.config.is_symlink())
        self.assertFalse(self.config.exists())

    def test_without_config_uses_main_script(self):
        self.assertEqual(self.load_arguments(), "main.plua")

    def test_detects_sysv_without_systemd(self):
        result = self.run_shell('detect_init_system\nprintf "%s\\n" "$INIT_SYSTEM"')
        self.assertEqual(result.stdout.strip(), "sysv")

    def test_prefers_running_systemd_over_legacy_init_script(self):
        result = self.run_shell('''
mkdir "$SYSTEMD_RUNTIME_DIR"
systemctl() { printf 'loaded\\n'; }
detect_init_system
printf '%s\\n' "$INIT_SYSTEM" "$SERVICE"
''')
        self.assertEqual(result.stdout.splitlines(), [
            "systemd", (self.directory / "systemd-helper.sh").as_posix()])

    def test_missing_plcnext_systemd_unit_is_reported(self):
        result = self.run_shell('''
mkdir "$SYSTEMD_RUNTIME_DIR"
systemctl() { printf 'not-found\\n'; }
detect_init_system
''', check=False)
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("plcnext.service (not-found)", result.stderr)

    def test_systemd_start_and_stop_delegate_to_manager(self):
        result = self.run_shell('''
check_files() { INIT_SYSTEM=systemd; }
systemctl() { printf '%s\\n' "$*"; }
start_service
stop_service
''')
        self.assertEqual(result.stdout.splitlines(), [
            "start ptusa_main.service", "stop ptusa_main.service"])

    def test_systemd_active_runtime_blocks_bus_permission_changes(self):
        result = self.run_shell('''
check_files() { INIT_SYSTEM=systemd; }
process_running() { return 1; }
systemctl() { printf 'activating\\n'; }
prepare_permissions() { echo 'permissions changed'; }
start_service --from-systemd
''', check=False)
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("PLCnext работает", result.stderr)
        self.assertEqual(result.stdout, "")

    def test_systemd_failed_runtime_with_live_process_still_blocks_start(self):
        result = self.run_shell('''
check_files() { INIT_SYSTEM=systemd; }
process_running() { return 1; }
systemctl() { printf 'failed\\n'; }
pidof() { return 0; }
prepare_permissions() { echo 'permissions changed'; }
start_service --from-systemd
''', check=False)
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("PLCnext работает", result.stderr)
        self.assertEqual(result.stdout, "")

    def test_systemd_reinstall_preserves_settings_and_does_not_start(self):
        self.write_defaults("main.plua --read_only_io --opc off")
        original_defaults = self.defaults.read_bytes()
        (self.application / "ptusa_main").write_bytes(b"old executable\n")
        unit = self.directory / "ptusa_main.service"
        unit.write_text("# ptusa_main-systemd-v1\n", encoding="utf-8")
        self.run_installation('''
check_requirements() {
    mkdir "$SYSTEMD_RUNTIME_DIR"
    detect_init_system
}
systemctl() {
    if [ "$*" = 'show -p LoadState --value plcnext.service' ]; then
        printf 'loaded\\n'
        return
    fi
    printf '%s\\n' "$*" >> "$PTUSA_TEST_DIRECTORY/systemctl.log"
    case "$*" in
        'stop ptusa_main.service')
            grep -q '^old executable$' "$EXECUTABLE" || exit 95
            touch "$PTUSA_TEST_DIRECTORY/previous-stopped" ;;
        'stop plcnext.service') touch "$PTUSA_TEST_DIRECTORY/plcnext-stopped" ;;
        'enable ptusa_main.service') [ ! -e "$SYSV_SERVICE" ] || exit 94 ;;
    esac
}
plcnext_running() { [ ! -f "$PTUSA_TEST_DIRECTORY/plcnext-stopped" ]; }
check_unmanaged_processes() { :; }
prepare_permissions() {
    [ -f "$PTUSA_TEST_DIRECTORY/previous-stopped" ] || exit 91
    [ -f "$PTUSA_TEST_DIRECTORY/plcnext-stopped" ] || exit 92
}
install() {
    [ -f "$PTUSA_TEST_DIRECTORY/previous-stopped" ] || exit 96
    [ -f "$PTUSA_TEST_DIRECTORY/plcnext-stopped" ] || exit 97
    command install "$@"
}
update-rc.d() { echo 'unexpected SysV registration' >&2; exit 93; }
''')
        self.assertEqual((self.application / "ptusa_main").read_bytes(),
                         b"new executable\n")
        self.assertEqual(self.defaults.read_bytes(), original_defaults)
        helper = self.directory / "systemd-helper.sh"
        self.assertEqual(helper.read_bytes(), SCRIPT.read_bytes())
        self.assertFalse(self.service.exists())
        self.assertEqual((self.directory / "systemctl.log").read_text().splitlines(), [
            "stop ptusa_main.service", "stop plcnext.service", "daemon-reload",
            "enable ptusa_main.service", "disable plcnext.service"])
        contents = unit.read_text()
        self.assertIn("Conflicts=plcnext.service", contents)
        self.assertIn("After=network.target localbus.service plcnext.service", contents)
        self.assertIn("ExecStart=/bin/sh " + helper.as_posix() + " _start", contents)
        self.assertIn("ExecStop=/bin/sh " + helper.as_posix() + " _stop", contents)
        self.assertIn("PIDFile=" + (self.directory / "ptusa_main.pid").as_posix(), contents)

    def test_systemd_install_preserves_foreign_unit_before_stopping(self):
        unit = self.directory / "ptusa_main.service"
        unit.write_text("foreign\n", encoding="utf-8")
        result = self.run_shell('''
check_requirements() { INIT_SYSTEM=systemd; }
readlink() { printf '%s\\n' "$PTUSA_TEST_SCRIPT"; }
systemctl() { echo 'service changed'; }
install_service
''', check=False)
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("другим установщиком", result.stderr)
        self.assertEqual(result.stdout, "")
        self.assertEqual(unit.read_text(), "foreign\n")

    def test_systemd_install_aborts_before_permissions_if_runtime_survives(self):
        result = self.run_shell('''
check_requirements() { INIT_SYSTEM=systemd; }
readlink() { printf '%s\\n' "$PTUSA_TEST_SCRIPT"; }
plcnext_running() { return 0; }
check_unmanaged_processes() { :; }
systemctl() { :; }
prepare_permissions() { echo 'permissions changed'; }
install_service
''', check=False)
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("PLCnext не остановился", result.stderr)
        self.assertEqual(result.stdout.splitlines(), ["service", "stop"])
        self.assertFalse((self.directory / "ptusa_main.service").exists())

    def test_systemd_uninstall_restores_plcnext_without_starting_it(self):
        unit = self.directory / "ptusa_main.service"
        unit.write_text("# ptusa_main-systemd-v1\n", encoding="utf-8")
        helper = self.directory / "systemd-helper.sh"
        helper.write_bytes(SCRIPT.read_bytes())
        self.run_shell('''
detect_init_system() { INIT_SYSTEM=systemd; SERVICE=$SYSTEMD_HELPER; }
systemctl() { printf '%s\\n' "$*" >> "$PTUSA_TEST_DIRECTORY/systemctl.log"; }
update-rc.d() { exit 93; }
uninstall_service
''')
        self.assertFalse(unit.exists())
        self.assertFalse(helper.exists())
        self.assertFalse(self.service.exists())
        self.assertEqual((self.directory / "systemctl.log").read_text().splitlines(), [
            "stop ptusa_main.service", "disable ptusa_main.service", "daemon-reload",
            "enable plcnext.service"])

    def test_config_is_used_by_default(self):
        self.config.touch()
        self.assertEqual(self.load_arguments(),
                         "--config " + self.config.as_posix())

    def test_existing_default_arguments_use_config(self):
        self.config.touch()
        self.write_defaults("main.plua")
        original = self.defaults.read_bytes()
        self.assertEqual(self.load_arguments(),
                         "--config " + self.config.as_posix())
        self.assertEqual(self.defaults.read_bytes(), original)

    def test_custom_arguments_override_config(self):
        self.config.touch()
        arguments = "main.plua --read_only_io --opc off --sleep_time=2"
        self.write_defaults(arguments)
        self.assertEqual(self.load_arguments(),
                         "--config " + self.config.as_posix() + " " + arguments)

    def test_explicit_config_is_preserved(self):
        self.config.touch()
        for option in ("--config other.ini", "--config=other.ini",
                       "-c other.ini", "-cother.ini", "-c=other.ini"):
            with self.subTest(option=option):
                self.write_defaults(option + " --debug")
                self.assertEqual(self.load_arguments(), option + " --debug")

    def test_without_default_config_preserves_custom_arguments(self):
        arguments = "main.plua --read_only_io --opc off"
        self.write_defaults(arguments)
        self.assertEqual(self.load_arguments(), arguments)

    def test_positional_separator_does_not_hide_default_config(self):
        self.config.touch()
        arguments = "-- --config"
        self.write_defaults(arguments)
        self.assertEqual(self.load_arguments(),
                         "--config " + self.config.as_posix() + " " + arguments)

    def test_shortcuts_forward_actions_and_arguments(self):
        result = self.run_shell('''
install_shortcuts
PATH=$(cd "$SHORTCUT_DIR" && pwd):$PATH
export PATH
for action in start stop restart install uninstall; do
    "ptusa$action" --check "two words"
done
''')
        expected = []
        for action in ("start", "stop", "restart", "install", "uninstall"):
            expected.extend(["service", action, "--check", "two words"])
        self.assertEqual(result.stdout.splitlines(), expected)
        self.assertEqual(len(list(self.shortcuts.iterdir())), 5)
        self.assertFalse((self.directory / "su.log").exists())

    def test_shortcut_directory_is_in_system_path(self):
        result = self.run_shell(
            'printf \'%s\\n\' "$DEFAULT_SHORTCUT_DIR" "$DEFAULT_SERVICE_PATH"')
        directory, path = result.stdout.splitlines()
        self.assertIn(directory, path.split(":"))

    def test_admin_shortcuts_request_root_once_and_preserve_arguments(self):
        result = self.run_shell('''
install_shortcuts
export PTUSA_TEST_UID=1002
for action in start stop restart install uninstall; do
    "$SHORTCUT_DIR/ptusa$action" "two words" '; exit 7'
done
''')
        expected = []
        for action in ("start", "stop", "restart", "install", "uninstall"):
            expected.extend(["service", action, "two words", "; exit 7"])
        self.assertEqual(result.stdout.splitlines(), expected)
        self.assertEqual((self.directory / "su.log").read_text().splitlines(),
                         ["su"] * 5)

    def test_failed_authentication_does_not_call_service(self):
        result = self.run_shell('''
install_shortcuts
export PTUSA_TEST_UID=1002
export PTUSA_TEST_SU_FAILURE=1
"$SHORTCUT_DIR/ptusastart"
''', check=False)
        self.assertEqual(result.returncode, 1)
        self.assertEqual(result.stdout, "")

    def test_admin_shortcut_preserves_service_exit_code(self):
        self.service.write_text("#!/bin/sh\nexit 23\n", encoding="utf-8")
        result = self.run_shell('''
install_shortcuts
export PTUSA_TEST_UID=1002
"$SHORTCUT_DIR/ptusastart"
''', check=False)
        self.assertEqual(result.returncode, 23)

    def test_reinstall_migrates_owned_shortcuts_from_legacy_directory(self):
        self.legacy_shortcuts.mkdir()
        managed = self.legacy_shortcuts / "ptusastart"
        managed.write_text("#!/bin/sh\n# ptusa_main-shortcut-v1\n",
                           encoding="utf-8")
        foreign = self.legacy_shortcuts / "ptusastop"
        foreign.write_text("foreign\n", encoding="utf-8")
        self.run_shell("install_shortcuts")
        self.assertFalse(managed.exists())
        self.assertEqual(foreign.read_text(encoding="utf-8"), "foreign\n")
        self.assertEqual(len(list(self.shortcuts.iterdir())), 5)

    def test_install_shortcut_uses_updated_application_script(self):
        self.run_shell("install_shortcuts")
        installer = self.application / "setup-ptusa-autostart.sh"
        installer.write_text("#!/bin/sh\nprintf 'updated\\n'\n"
                             "printf '%s\\n' \"$@\"\n", encoding="utf-8")
        result = self.run_shell('"$SHORTCUT_DIR/ptusainstall" --check')
        self.assertEqual(result.stdout.splitlines(),
                         ["updated", "install", "--check"])

    def test_reinstall_updates_managed_shortcuts(self):
        self.run_shell("install_shortcuts")
        command = self.shortcuts / "ptusastart"
        command.write_text("#!/bin/sh\n# ptusa_main-shortcut-v1\nexit 7\n",
                           encoding="utf-8")
        result = self.run_shell('install_shortcuts\n"$SHORTCUT_DIR/ptusastart"')
        self.assertEqual(result.stdout.splitlines(), ["service", "start"])

    def test_install_does_not_overwrite_foreign_commands(self):
        self.shortcuts.mkdir()
        command = self.shortcuts / "ptusastop"
        command.write_text("foreign\n", encoding="utf-8")
        result = self.run_shell("install_shortcuts", check=False)
        self.assertNotEqual(result.returncode, 0)
        self.assertEqual(command.read_text(encoding="utf-8"), "foreign\n")
        self.assertEqual(list(self.shortcuts.iterdir()), [command])

    def test_uninstall_preserves_foreign_files(self):
        self.run_shell("install_shortcuts")
        command = self.shortcuts / "ptusastop"
        command.write_text("foreign\n", encoding="utf-8")
        unrelated = self.shortcuts / "unrelated"
        unrelated.touch()
        self.run_shell("remove_shortcuts")
        self.assertEqual(set(self.shortcuts.iterdir()), {command, unrelated})

    def test_uninstall_service_removes_shortcuts(self):
        self.legacy_shortcuts.mkdir()
        (self.legacy_shortcuts / "ptusastart").write_text(
            "#!/bin/sh\n# ptusa_main-shortcut-v1\n", encoding="utf-8")
        self.run_shell('''
require_root() { :; }
stop_service() { :; }
update-rc.d() { :; }
install_shortcuts
uninstall_service
''')
        self.assertFalse(self.service.exists())
        self.assertEqual(list(self.shortcuts.iterdir()), [])
        self.assertEqual(list(self.legacy_shortcuts.iterdir()), [])

    def test_install_does_not_follow_shortcut_symlinks(self):
        self.shortcuts.mkdir()
        command = self.shortcuts / "ptusastart"
        try:
            command.symlink_to(self.service)
        except OSError:
            self.skipTest("Создание символических ссылок недоступно.")
        original = self.service.read_bytes()
        result = self.run_shell("install_shortcuts", check=False)
        self.assertNotEqual(result.returncode, 0)
        self.assertTrue(command.is_symlink())
        self.assertEqual(self.service.read_bytes(), original)


if __name__ == "__main__":
    unittest.main()
