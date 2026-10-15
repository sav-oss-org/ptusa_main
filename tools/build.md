# Сборка нескольких конфигураций ptusa_main

Требуются Python 3.9+, CMake из PATH (либо `--cmake "путь/cmake.exe"`)
и компиляторы/SDK для выбранных конфигураций. Версия CMake должна поддерживать
генератор выбранного preset; минимальная версия проекта — 3.31.

Из корня репозитория:

```powershell
python tools/build.py
```

Выберите несколько номеров через пробел или запятую, например `1,2,3`.
`all` выбирает все доступные конфигурации. Пустой ввод отменяет сборку.
Список берётся из `CMakePresets.json` и `CMakeUserPresets.json`, включая
подключённые JSON-файлы с обычными относительными/абсолютными путями.
Условия доступности проверяет CMake. Наличие самого SDK проверяется при
конфигурировании: доступность preset на Windows ещё не означает наличие SDK.
Для генерации локальных PLCnext presets используйте `configure_plcnext_presets.py`.

Каждая выбранная конфигурация последовательно выполняет:

```text
cmake --preset <configurePreset>
cmake --build --preset <buildPreset> --target install --parallel 4
```

Цель `install` собирает проект и выполняет его правила установки: `ptusa_main`,
библиотеки и, для PLCnext, пакет `.pcwlx`. Пути установки определяются
`CMAKE_INSTALL_PREFIX` и `CMakeLists.txt`. Debug/Release берутся из build preset.
Сборки идут последовательно, чтобы не конфликтовать при установке в общие каталоги;
`--jobs` задаёт параллелизм внутри одной сборки.

Примеры:

```powershell
# Только список (номера могут изменяться при добавлении presets).
python tools/build.py --list

# Две конфигурации без интерактивного меню.
python tools/build.py build-windows-VS2026-debug build-windows-VS2026-release --jobs 8

# Выбрать набор, сохранить имена и параметры, просмотреть команды без сборки.
python tools/build.py --jobs 8 --save-settings .build-settings.json --dry-run

# Повторить сохранённую сборку; параметры командной строки имеют приоритет.
python tools/build.py --settings .build-settings.json

# Продолжать после ошибок и передать дополнительную переменную CMake.
python tools/build.py --settings .build-settings.json --keep-going --configure-arg=-DSKIP_DEMO=ON

# Использовать уже настроенные каталоги сборки.
python tools/build.py --settings .build-settings.json --skip-configure
```

Сохранённый JSON содержит `presets` (имена), `jobs`, `cmake`,
`skip_configure`, `keep_going`, `configure_args`. Его можно редактировать вручную.
`--dry-run` и `--list` не сохраняются как настройки.
Без `--keep-going` первая ошибка останавливает выполнение. Если хотя бы одна
сборка завершилась ошибкой, код возврата скрипта — 1; при успехе — 0.
`--dry-run` запускает только команды CMake для чтения списка presets.

Проверка логики скрипта без компиляции:

```powershell
python -m unittest discover -s tools -p test_build.py
```
