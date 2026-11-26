# Contributing

When contributing to this repository, please first discuss the change you
to make via issue, email, or any other method with the owners of this
repository before making a change.

Please note we have a code of conduct, please follow it in all your
interactions with the project.

## Identifying a Build

Every build carries the source revision it was compiled from. The
`--version` option and the start log line print the build ID in
the `<version>+<git sha>[-dirty]` form, and installed artifacts
ship a `BUILDINFO.txt` with the commit, the git ref and the CI
run context (see issue #1647).

## Pull Request Process

1. Ensure any install or build dependencies are removed before the end
   of the layer when doing a build.
2. Update the README.md with details of changes to the interface, this
   includes new environment variables, exposed ports, useful file
   locations and container parameters.
3. Increase the version numbers in any examples files and the README.md
   to the new version that this Pull Request would represent.
4. You may merge the Pull Request in once you have the sign-off of two
   other developers, or if you do not have permission to do that, you
   may request the second reviewer to merge it for you.

## Pull Request Scope and Titles

- Keep each pull request focused on one concrete change.
- Put unrelated improvements into separate pull requests.
- Use concise pull request titles. Write them in the imperative mood,
  because a squash merge builds the commit message from the title.
- Use Markdown backticks to highlight technical words when useful, for
  example:
  ``Remove `DNS` reverse lookup from `TCP` logging``.
- Start the title with a change-type prefix from the closed
  set below, followed by `:`. The release drafter groups
  release notes by this prefix, so a missing prefix files the
  change under "Other changes":
  - `fix:` - bug fixes (issue labels `bug`, `bugfix`);
  - `feat:` - new functionality, devices, signals
    (label `enhancement`);
  - `test:` - test and fixture stabilization;
  - `build:` - CMake, presets, toolchain;
  - `ci:` - workflows, Sonar, merge queue
    (label `github_actions`);
  - `perf:` - optimizations and speedups;
  - `deps:` - submodule and action bumps (labels
    `dependencies`, `submodules`);
  - `docs:` - documentation (label `documentation`);
  - `chore:` - everything else.
- A squash merge copies the title into the commit subject,
  so keep the prefixed title as concise as the commit
  subject rules require.
- Pull requests changing product code (`PAC`, `common`,
  `test`) must link the issue they close (`Closes #N` in the
  body or the Development section). The `no-issue` label
  exempts urgent fixes and `dependabot` is exempt. The
  `pr-policy` job of `commit-check.yml` reports both rules:
  titles warn during rollout, missing links fail product
  changes and only warn for infrastructure ones.
- Explain why the change is needed, what changed, expected risk and validation
  in the pull request description.

## Branch Names

- Use snake_case. Prefixes form a closed set mapped from the issue
  kind and the commit verb:
  - `fix_` for bugs: `fix_pac_info_test_order`;
  - `add_` for features and new devices, written as
    `add_device_<name>` for devices:
    `add_lua_test_error_counter`;
  - `remove_` and `refactor_` for removals and refactoring;
  - `ci_` for CI and test infrastructure: `ci_test_order_shuffle`;
  - `docs_` for documentation.
- The `dependabot/` prefix is reserved for automated pull requests.
- Never use Conventional Branch slashes (`feat/`, `fix/`): the
  branch vocabulary follows the commit verbs, not Conventional
  Commit types.
- CI does not validate branch names: squash merges keep them out of
  history, and commit-check only knows the Conventional Branch
  format, which rejects every branch in this repository.

## Issue Types and Labels

- The native GitHub issue type is the canonical classification:
  `Bug`, `Feature`, or `Task`. Templates apply it automatically.
- The `bug` and `enhancement` labels are GitHub's own mirror of the
  native type: treat them as read-only reflections, not as a second
  source of truth to keep in sync by hand.
- The issue templates write title prefixes (`[BUG]:`,
  `[FEATURE]:`, `[DEVICE]:`) as human-readable reflections of the
  type. Edit a prefix only to fix a wrong reflection, never to
  reclassify the issue.
- `[DEVICE]` refines `[FEATURE]` for new device support and lives in
  the title until the organization owns a Device type.
- A `[DISCUSSION]` marker in the title means the issue is a design
  discussion for humans, not something to implement directly.
- Labels remain for workflow state beyond classification, such as
  `urgent`, `help wanted`, and `iteration-plan`.
- The `bug` label classifies issues while `bugfix` classifies pull
  requests; they are separate axes, not duplicates.

## Versioning and Tags

- Release tags use CalVer: `YYYY.MM.DD.N`. The serial `N` is `0`
  for the first release of a day and increments for every same-day
  hotfix. The tag equals the four-component
  `project(ptusa_main VERSION ...)` string.
- Components carry no leading zeros (`2026.1.28`), and new tags carry
  no leading `v`; the legacy `v2024.01.31` is a historical exception.
- Tags carry no free-form suffixes such as `CP1251`; rule `CC401` in
  `.github/cchk.toml` enforces the pattern.
- Semantic Versioning is reserved for independently distributed
  libraries or SDKs, should this repository ever publish them.

## Release Process

- The four-component `project(ptusa_main VERSION ...)`
  string in `CMakeLists.txt` is the single source of truth:
  the binaries embed it, and the release tag must equal it
  byte-for-byte.
- Bump the version in a pull request into `master`; the
  draft release is cut automatically after the merge, and
  publishing it builds the archives from the tag commit and
  attaches them to the release.
- The full runbook:
  [English version](RELEASE_PROCESS.md);
  [Русская версия](RELEASE_PROCESS_RU.md).

## AI-Assisted Commit Attribution

- Commits whose substantive content was produced with AI assistance must
  carry provenance trailers in the final paragraph of the commit message,
  with no blank line between them:

  ```text
  Co-authored-by: <agent-name> <agent-noreply-email>
  AI-assisted: <agent-name> (model: <model-name>, <execution-mode>)
  ```

- Take `<agent-name>` and `<agent-noreply-email>` from the assistant
  that produced the change; never credit an unrelated agent.
- Use the model name reported by the agent at commit time verbatim; do
  not invent or normalize it.
- Document the execution mode: `local` for local models, the hosted
  provider otherwise.
- For the Zed agent running a local model the trailers are:

  ```text
  Co-authored-by: Zed AI Agent <253101093+zed-agent@users.noreply.github.com>
  AI-assisted: Zed agent (model: <model-name>, local)
  ```

- Never line-wrap or reflow trailer lines, even when they exceed the
  usual 72-character body limit.
- Human-only commits need no trailers.
- Start the body of AI-created issues and pull requests with
  `🤖 Generated with AI assistance`.
- See `AGENTS.md` for the full rule and
  `.agents/skills/ai-commit-attribution/SKILL.md` for the agent skill.

## Code of Conduct

### Our Pledge

In the interest of fostering an open and welcoming environment, we as
contributors and maintainers pledge to making participation in our project and
our community a harassment-free experience for everyone, regardless of age, body
size, disability, ethnicity, gender identity and expression, level of experience,
nationality, personal appearance, race, religion, or sexual identity and
orientation.

### Our Standards

Examples of behavior that contributes to creating a positive environment
include:

- Using welcoming and inclusive language
- Being respectful of differing viewpoints and experiences
- Gracefully accepting constructive criticism
- Focusing on what is best for the community
- Showing empathy towards other community members

Examples of unacceptable behavior by participants include:

- The use of sexualized language or imagery and unwelcome sexual attention or
advances
- Trolling, insulting/derogatory comments, and personal or political attacks
- Public or private harassment
- Publishing others' private information, such as a physical or electronic
  address, without explicit permission
- Other conduct which could reasonably be considered inappropriate in a
  professional setting

### Our Responsibilities

Project maintainers are responsible for clarifying the standards of acceptable
behavior and are expected to take appropriate and fair corrective action in
response to any instances of unacceptable behavior.

Project maintainers have the right and responsibility to remove, edit, or
reject comments, commits, code, wiki edits, issues, and other contributions
that are not aligned to this Code of Conduct, or to ban temporarily or
permanently any contributor for other behaviors that they deem inappropriate,
threatening, offensive, or harmful.

### Scope

This Code of Conduct applies both within project spaces and in public spaces
when an individual is representing the project or its community. Examples of
representing a project or community include using an official project e-mail
address, posting via an official social media account, or acting as an appointed
representative at an online or offline event. Representation of a project may be
further defined and clarified by project maintainers.

### Enforcement

Instances of abusive, harassing, or otherwise unacceptable behavior may be
reported by contacting the project team at [dzimitriy@gmail.com]. All
complaints will be reviewed and investigated and will result in a response that
is deemed necessary and appropriate to the circumstances. The project team is
obligated to maintain confidentiality with regard to the reporter of an incident.
Further details of specific enforcement policies may be posted separately.

Project maintainers who do not follow or enforce the Code of Conduct in good
faith may face temporary or permanent repercussions as determined by other
members of the project's leadership.

### Attribution

This Code of Conduct is adapted from the [Contributor Covenant][homepage],
version 1.4,
available at [http://contributor-covenant.org/version/1/4][version]

[homepage]: http://contributor-covenant.org
[version]: http://contributor-covenant.org/version/1/4/
