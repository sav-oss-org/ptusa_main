# Release Process

Russian version: [RELEASE_PROCESS_RU.md](RELEASE_PROCESS_RU.md).

One authoritative version string, one command to publish. This
runbook describes how `ptusa_main` releases are produced by CI;
the tag naming rules live in
[CONTRIBUTING.md](CONTRIBUTING.md#versioning-and-tags).

## Source of the version

The project version is the four-component CalVer string in
`CMakeLists.txt`:

```cmake
project(ptusa_main VERSION YYYY.MM.DD.N)
```

It is compiled into the binaries through `version_info.h`
(`-v` output, OPC UA server info, Windows version resource),
so the release tag must equal it byte-for-byte — no `v`
prefix, no suffix. A published release whose tag differs from
the CMake string cannot produce honest binaries, so CI refuses
it (see the gate below).

## Cutting a release

1. Bump the version. Open a pull request changing
   `project(ptusa_main VERSION ...)` in `CMakeLists.txt` and
   merge it into `master`. The merge is the release decision.
2. Review the draft. The `autocut-release` job of
   `cmake.yml` watches master pushes and, when the CMake
   version has no release yet, drafts one named after it,
   targeting the merged commit. GitHub materializes the tag
   from that exact commit on publish; the body carries
   automatically generated notes — the list of pull requests
   merged since the previous release.
3. Publish. Click Publish (or
   `gh release edit <version> --draft=false`). That fires
   `on: release` in `cmake.yml`: the workflow runs from the
   tag commit, verifies the gate, builds the full matrix, and
   `publish-release` attaches every archive to the release.

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
- Rerun assets: rerun the failed release run. Note that a
  workflow run uses the workflow file from the tag commit;
  fixes merged to `master` afterwards do not apply to an
  already cut tag — re-upload from the run artifacts manually
  in that case.
- Regenerate notes (draft): `gh release edit <version>
  --generate-notes`.
- Hotfix on the same day: bump the `.N` revision and repeat
  the ritual.
- Deleting a release leaves its tag behind. The next master
  push will draft that version again onto the existing tag.
  To retire a version completely, delete both the release and
  the tag, or move on with a newer version bump.
