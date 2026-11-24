# Словарь терминов ПТУСА

Накапливаемый словарь терминов (русский → английский),
которые применяются в ПТУСА. Формат записи:
`- Термин — English (контекст)`. Перед переводом нового
термина сверяйтесь со словарём; после принятия термина
в ревью дополняйте его.

## Ревью, CI и релиз

- журнал (лог) — log; уточняйте вид: журнал событий, журнал
  сборки
- двоичные исполняемые файлы — binaries
- запрос на внесение изменений — pull request
- фиксация в master-ветви — push
- черновик релиза — draft
- публикация — Publish; publishing
- примечания к релизу — release notes
- вложения релиза — assets
- конвейер — workflow
- прогон — run (single workflow run)
- артефакты сборки — build artifacts
- сборочная матрица — build matrix
- кросс-сборка — cross build
- покрытие нового кода — new code coverage
- плавающий тест — flaky test (в формальном тексте)
- выпуск релиза — cutting a release
- номер версии — version string
- тег релиза — release tag
- метка — gate marker (например, `version_ok`)

## ПАК и технологические объекты

- ПАК — programmable automation controller (PAC), в
  контексте АСУ ТП
- технологический объект — technology object
- операция — operation
- датчик — sensor
- исполнительное устройство — actuator
- параметры — controller parameter set (управляет
  `params_manager`)
- циклическое автосохранение — periodic autosave
- команда принудительного сохранения — FORCE_SAVE_PARAMS
  (monitor `system cmd 102`)
