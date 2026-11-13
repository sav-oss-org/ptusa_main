---
name: multiagent-pipeline
description: Run the ptusa_main issue-to-PR pipeline as a bounded
  multi-agent loop. Claim eligible issues, spawn one sub-agent per issue
  through to an open PR, answer review threads, and watch CI. Use when
  asked to "run the pipeline", "process issues with agents", or a
  "conveyor tick".
---

# Multi-agent issue pipeline

GitHub is the state machine. Agents tick and exit; no resident daemons.
One tick = one pass over every active process, then report and exit.

## Label protocol

The labels below already exist in `sav-oss-org/ptusa_main`; create
them only where they are missing:

- `agent:ok` — triaged by a maintainer as agent-suitable.
- `agent:claimed` — an active pipeline process; its PR is linked in a
  comment on the issue.
- `agent:needs-input` — escalated; a human must act before continuing.

A process holds `agent:claimed` only while it needs agent work: the
pipeline removes the label when the PR goes to human review or the
issue escalates to `agent:needs-input`.

Concurrency limit: at most **5** issues with `agent:claimed`. Never
claim an issue that is assigned to a human, already linked to an open
non-agent PR, or missing `agent:ok`. Once a machine account such as
`ptusa-bot` exists, the claim signal may move from the
`agent:claimed` label to assigning the issue to the bot; eligibility
(`agent:ok`) and escalation (`agent:needs-input`) stay labels.

The same conventions are documented for humans in
`docs/CONTRIBUTING.md` (sections "Branch Names" and "Issue Types and
Labels").

## Tick procedure

1. Sync: `git fetch sav-oss-org master --quiet`.
2. Advance every `agent:claimed` process exactly one step:
   - PR has failing checks: if the same check already failed with the
     same cause before, do not retry; if it is unrelated to the diff,
     report it as infra; otherwise diagnose and push a fix. Max 3
     attempts per check, then `agent:needs-input`.
   - PR has unaddressed review threads: reply in each thread and push
     fixes. Max 3 rounds per thread, then `agent:needs-input` and a
     mention of the reviewer. Never resolve a thread opened by a human.
   - PR is green with open threads answered: comment "ready for human
     review" once, remove `agent:claimed` to free the slot, and stop
     touching it; merging is humans only.
3. GitHub has no atomic label compare-and-set, so serialize claiming
   under an exclusive lock for this step: `mkdir ../ptusa-wt/tick.lock`
   fails while another tick holds it (transient coordination, not
   state); remove the lock on exit. Ticks run on one machine, so a
   local lock suffices. While holding it, take the oldest unclaimed
   `agent:ok` issue. Before claiming, verify no human already works
   on it: no assignees, no entry in
   `closedByPullRequestsReferences` pointing at an open PR, and no
   open PR body mentioning `#<issue>` (`gh pr list --state open`).
   Claim only then: label it `agent:claimed`, and spawn one sub-agent
   with the full contract below.
4. Print a summary table (issue, PR, state, next action) and exit.

## Working copies

Claimed work runs in a dedicated linked worktree, never in the main
checkout:

```
git worktree add ../ptusa-wt/wt-<issue> -b <prefix>_<slug> \
    sav-oss-org/master
git -C ../ptusa-wt/wt-<issue> submodule update --init --recursive
```

Each worktree builds into its own `bin/build/...` because the CMake
presets bind the binary directory to the source directory: parallel
builds cannot clobber each other, but they each pay a full build.
Therefore limit concurrent full builds to one or two, prefer
sub-agent phases that do not compile at all, and remove finished
worktrees with `git worktree remove` plus `git worktree prune`.
Sharing compiled objects across worktrees via ccache needs
`base_dir` set to the common `../ptusa-wt` root: treat that as an
experiment and measure before relying on it.

## Issue-agent contract

Include all of this in the sub-agent prompt:

- Timing bookends: first action `date -u +%s > "$WT/.agent_start"`,
  final report must include wall-clock runtime computed from it.
- Read `AGENTS.md`, `docs/CONTRIBUTING.md`, and
  `.github/copilot-instructions.md` before coding.
- One issue, one branch (snake_case), one focused PR into
  `sav-oss-org/ptusa_main:master` from the configured fork. Issue kind
  is decided in order: native issue type (`issueType`, the intended
  canonical source, not yet populated on older issues), then labels
  (`bug`, `enhancement`), then the template title prefix (`[BUG]: `,
  `[FEATURE]: `, `[DEVICE]: `) as mirror and fallback; the title-only
  submarkers `[DEVICE]` and `[DISCUSSION]` (the latter means humans
  only) have no type or label equivalent yet. Branch prefixes are a
  closed set, mapped from the issue kind and the commit verb: `fix_`
  (`bug`), `add_`
  (`enhancement`, `[DEVICE]`, new devices as `add_device_<name>`),
  `remove_`, `refactor_`, `ci_` (CI and test infrastructure), `docs_`.
  Never use Conventional Branch slashes (`feat/`, `fix/`);
  `dependabot/*` belongs to the bot. Do not confuse the `bug` label
  (issues) with the `bugfix` label (PR kind).
- House style: C++17, brace initialization, 80-column lines. Verify the
  project style skills when touching C++ or commits. The prefix and
  classification rules above are documented in `docs/CONTRIBUTING.md`;
  read them from the file, this contract is only a summary.
- Commits follow the house commit policy: imperative subject within 50
  characters, body wrapped at 72, AI provenance trailers verbatim and
  never wrapped.
- The PR body starts with the AI-assistance marker, states scope, risk,
  and validation, and links the issue.
- Once the PR is open, comment the PR URL on the issue
  (`gh issue comment <issue> --repo sav-oss-org/ptusa_main --body
  <pr-url>`): that comment is the claim signal the label protocol
  promises.
- Forbidden without a human decision: merging, approving, resolving
  human threads, touching `deps/` submodules, adding labels,
  `git commit --amend` of pushed commits, force-push. Editing
  workflows or CI files is allowed only for a claimed `ci_` issue,
  only within its stated scope, and never to job permissions,
  secrets, or the trusted-author CI gate.
- Verification is scoped: prefer targeted `ctest -R` runs from the
  build directory; full builds only when the change needs them and the
  user allowed builds.
- If the task cannot be decided from repository docs and code, stop
  with the line `NEEDS-HUMAN: <question>` instead of guessing.

## Review replies

- Reply to each inline thread with the concrete fix or the verified
  reason for disagreeing, citing the command and result. No generic
  agreement.
- Match the reviewer's language. Keep code pushes in the same PR.

## Stop conditions

- 5 processes claimed: do not claim more; advance and report only.
- The same tick step fails 3 ticks in a row for one process: label
  `agent:needs-input` and report.
- Infra or flaky-external failures: report only, never loop on them.

## Command reference

```
gh issue list --repo sav-oss-org/ptusa_main --label agent:ok \
  --json number,title --jq '.[] | "#\(.number) \(.title)"'
gh issue edit N --repo sav-oss-org/ptusa_main --add-label agent:claimed
gh pr checks N --repo sav-oss-org/ptusa_main
gh api repos/sav-oss-org/ptusa_main/pulls/N/comments \
  --jq '.[] | "\(.id) \(.path):\(.line) \(.user.login)"'
```

`N` is the issue number in `gh issue` commands and the PR number in
`gh pr` commands and `pulls/N` API paths. All state lives in GitHub
labels, PR threads, and check results; never in local files.

CI gotcha: required checks on fork PRs run the base branch's copy
of the workflow through `pull_request_target`, gated by trusted
author (see the build job condition in `cmake.yml`). A workflow
job added in a pull request therefore cannot run on its own PR;
new jobs must mirror that gate and check out the PR head sha.
