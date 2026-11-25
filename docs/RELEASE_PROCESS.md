# Release Process

[Версия на русском](RELEASE_PROCESS_RU.md).

One authoritative version string, one command to publish.
For example: version `2026.11.18.0`, published by
`gh release edit 2026.11.18.0 --draft=false`. This runbook
describes how `ptusa_main` releases are produced by CI; the
tag naming rules live in
[CONTRIBUTING.md](CONTRIBUTING.md#versioning-and-tags).

## Source of the version

The project version is a four-part number in the format
YYYY.MM.DD.N (CalVer), specified in CMakeLists.txt:

```cmake
project(ptusa_main VERSION YYYY.MM.DD.N)
# for example: project(ptusa_main VERSION 2026.11.18.0)
```

The serial `N` counts releases on that day: `0` is the
first release of the day, then `1`, `2`, and so on for
repeat releases (hotfixes) of the same day.

The version string is compiled into the binaries through
`version_info.h` (`-v` output, OPC UA server info, Windows
version resource), so the release tag must equal the version
string byte-for-byte — no `v` prefix, no suffix. A published
release whose tag differs from the CMake string cannot
produce honest binaries, so CI rejects such a release
(see [What CI guarantees](#what-ci-guarantees)).

## Cutting a release

1. Bump the version. Open a pull request changing
   `project(ptusa_main VERSION ...)` in `CMakeLists.txt` and
   merge it into `master`. The merge is the release decision.
2. Review the draft. The `autocut-release` job of
   `cmake.yml` watches master pushes and, when the CMake
   version has no release yet, drafts a release named after
   the CMake version, targeting the merged commit. GitHub
   creates the tag from that exact commit on publish; the
   draft body lists the pull requests merged since the
   previous release, grouped by their title prefix
   (`feat:`, `fix:`, `docs:`, and so on), followed by a
   link to the full changelog. Without a previous release,
   or when the comparison spans more than 250 commits,
   GitHub's own generated notes are used instead.
3. Publish. Click Publish (or run
   `gh release edit <version> --draft=false`). Publishing
   fires `on: release` in `cmake.yml`: the workflow runs
   from the tag commit, verifies the gate, builds the full
   matrix, and `publish-release` attaches every archive to
   the release.

No manual `gh release create`, no tag management, no hand
written changelog.

## What CI guarantees

- The version gate (`version_ok` in the change detector)
  compares the tag with `project(... VERSION ...)` parsed from
  the tag commit. A mismatch fails within seconds, skips the
  matrix, and leaves the release without assets.
- `publish-release` runs only on the `release` event and only
  after a fully successful matrix; uploads use
  `gh release upload --clobber`, so reruns are idempotent.
- `contents: write` is scoped to these two jobs only.

## Operations

- Archives: `Windows-MSVC.7z`, `Windows-MinGW.7z`, `Linux.7z`,
  and the PLCnext cross builds `Linux-AXCF2152-*`,
  `Linux-AXCF3152-*`, `Linux-AXCF2000-*` — produced from the
  tag commit, so binaries always match the tag.
- Rerun release assets: rerun the failed release run. Note that a
  workflow run uses the workflow file from the tag commit;
  fixes merged to `master` afterwards do not apply to an
  already cut tag — re-upload from the run artifacts manually
  in that case.
- Regenerate notes (draft): `gh release edit <version>
  --generate-notes`. GitHub replaces the grouped body
  with its own flat list; the grouped body is produced
  only once, when `autocut-release` cuts the draft.
- Hotfix on the same day: bump the `.N` revision and repeat
  this procedure.
- Deleting a release leaves the release tag behind. The next master
  push will draft that version again onto the existing tag.
  To retire a version completely, delete both the release and
  the tag, or move on with a newer version bump.
