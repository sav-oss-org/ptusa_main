---
name: russian-tech-writing
description: Write or edit Russian-language text in ptusa_main -
  documentation, code comments, issues, review replies, release
  notes. Applies the review-enforced style of this repository for
  the APCS (ПАК/АСУ ТП) and CI/release domain: pronoun rules,
  GitHub terminology, language links, anchors, and examples.
  Keeps the shared ru-en term glossary in `GLOSSARY.md`
  current.
---

# Russian technical text for ptusa_main

Rules below were established in review on this repository
(maintainers `idzm` and `Lictwin`). The gold-standard example of
a Russian document is `docs/RELEASE_PROCESS_RU.md`; the English
counterpart is `docs/RELEASE_PROCESS.md`.

## When to use

- Writing or editing Russian documentation (`docs/**`,
  especially `docs/russian_readme/**` and `*_RU.md` files).
- Writing Russian code comments or test comments.
- Answering review threads and posting issue/PR comments (`idzm`
  communicates in Russian).

## Hard rules

1. No pronoun references. Do not use `он`, `она`, `оно`, `это`,
   `её`, `их`, `ней` to refer back to a term; repeat the term
   instead.

   - Bad: `Она добавляется в binaries через version_info.h`.
   - Good: `Номер версии добавляется в двоичные исполняемые
     файлы (binaries) через version_info.h`.

2. Name what is added/checked/done, where and in what form. If a
   sentence says what happens, it must not rely on a previous
   sentence to know about what: state the object, the target and
   the format.

3. Russian term first, GitHub/platform term in parentheses in
   backticks, on first use in a section:

   - `запрос на внесение изменений (`pull request`)`
   - `объедините его с веткой `master` (операция merge)`
   - `фиксация в master-ветви (`push`)`
   - `черновик релиза (`draft`)`, `публикация (`Publish`)`
   - `примечания к релизу (`release notes`)`
   - `вложения релиза (`assets`)`
   - `конвейер (`workflow`)`, `прогон` (a single run)
   - `артефакты сборки (`build artifacts`)`

   Prefer an established Russian term when one exists
   (`журнал (лог)` = log); never translate identifiers,
   commands or parameter names.  `журнал` is polysemous:
   every log is a журнал, but not every журнал is a log, so
   clarify the kind when the context is not obvious
   (`журнал событий`, `журнал сборки`).

4. Cross-references are anchor links, bare `см. ниже` is not
   enough:

   - Bad: `(см. ниже)`.
   - Good: `([см. ниже](#что-гарантирует-ci))` - the link text
     may stay `см. ниже`, the anchor is mandatory.

5. Give a concrete example at first mention of a format or a
   command: real value and real command, for example `номер
   2026.11.18.0`, публикация -
   `gh release edit 2026.11.18.0 --draft=false`.

6. A fact lives in one document. Tag-naming rules live in
   `docs/CONTRIBUTING.md` (section `Versioning and tags`); other
   documents link to that anchor instead of restating them.

## Language links

Name every linked document in the language of its content:

- English document, link to the Russian sibling:
  `[Версия на русском](FILE_RU.md)`.
- Russian document, link to the English sibling:
  `[English version](FILE.md)`.
- A list of language variants (e.g. in `docs/CONTRIBUTING.md`):
  `[English version](RELEASE_PROCESS.md)` and
  `[Русская версия](RELEASE_PROCESS_RU.md)`, without an extra
  lead-in like `the Russian version is` - the link text already
  says it.

## Style

- Quotes are `«ёлочка»`; sentences are complete and end with a
  period.
- Verb forms must agree with the real subject: `Следующая
  фиксация (`push`) в `master` снова создаст черновик этой
  версии` - singular subject, singular verb.
- Close participial and adverbial phrases with commas.
- Section headings are short noun phrases: `Источник версии`,
  `Выпуск релиза`, `Что гарантирует CI`, `Использование`.
- Bans are phrased impersonally: `Не использовать ручной
  gh release create, ручное управление тегами...`.
- Do not switch languages mid-sentence except for terms in
  backticks and the language links above.

## Change and merge lists

- Format change lists, accepted PR lists and release summaries
  as multi-level lists whenever possible.
- Top level is the category (`Исправления`, `Новое`, `CI`),
  inner levels are the items.
- Each item names the object and links the PR:
  `- починен `TCP` лог ([#1520](url))`.

## Technical constraints

- Line length stays <= 80 characters. Count characters, not
  bytes: Cyrillic is 1 character for python `len`, but awk
  counts bytes and lies.
- Files are UTF-8; the terminal may show mojibake when echoing
  Cyrillic - verify the file, not the console.
- Code comments may be Russian; keep them complete sentences
  with a period, same 80-character limit.
- Commit messages and pull request bodies are English; the PR
  body starts with `🤖 Generated with AI assistance`.
- Russian replies to review threads end with the footer
  `🤖 Posted with AI assistance (Zed agent)`.

## Glossary file

- The skill directory holds `GLOSSARY.md`, the accumulating
  Russian-English dictionary of terms used in ptusa_main.
- Before translating a new term, check `GLOSSARY.md` and
  reuse the recorded translation.
- When review approves a term (or a term is newly
  introduced), append it to `GLOSSARY.md`: term, English,
  context.  Create the file when it is missing.

## Domain vocabulary

- `ПАК` = programmable automation controller (`PAC`); АСУ ТП
  context: технологический объект, операция, датчик,
  исполнительное устройство.
- `параметры` = the controller parameter set managed by
  `params_manager`; `циклическое автосохранение` = periodic
  autosave; `команда принудительного сохранения` = monitor
  command `system cmd 102` (`FORCE_SAVE_PARAMS`).
- Monitor protocol fields stay as identifiers: `CMD`,
  `CMD_ANSWER`, `cmd_answer`.
- CI terms: `сборочная матрица` (build matrix), `кросс-сборка`
  (cross build), `покрытие нового кода` (new code coverage);
  `плавающий тест` for a flaky test in formal text.
- Release terms: `выпуск релиза` (cutting), `публикация`
  (publishing), `номер версии` (version string), `тег релиза`
  (release tag); `метка` stays for CI gate markers such as
  `version_ok`.
