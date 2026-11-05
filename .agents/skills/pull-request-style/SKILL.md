---
name: pull-request-style
description: Write focused GitHub pull request titles, descriptions, and scope notes. Use this when creating or reviewing pull requests, commit summaries, or proposed follow-up changes.
---

# Pull request style

Use this when creating or reviewing pull requests.

## Keep one PR focused

- One pull request should solve one user-visible or engineering problem.
- Good scope is a narrow change such as
  `Removes DNS reverse lookup from TCP logging`.
- Do not mix unrelated work into the same pull request.
- If an improvement is useful but not required for the current pull request
  to be correct, propose it as a separate pull request.
- Follow-up refactors, formatting sweeps, test infrastructure, logging
  framework changes, and dependency updates belong in separate requests
  unless they are required to make the current change work.

## Title style

- Prefer a concise third-person present title for the pull request list:
  `Removes DNS reverse lookup from TCP logging`.
- Keep the title under 80 characters.
- Do not end the title with punctuation.
- Mention the subsystem when it helps filtering.
- Keep commit subjects imperative, for example
  `Remove DNS reverse lookup from TCP logging`.

Good examples:

- `Removes DNS reverse lookup from TCP logging`
- `Stores NVRAM and EEPROM under /opt/main on PLCnext`
- `Logs accepted TCP connections without blocking DNS`

Bad examples:

- `Fix stuff`
- `Refactor, cleanup, and tests`
- `Update network code`

## Body style

Include only information that helps review:

1. Why the change is needed.
2. What changed.
3. Risk or behavior change.
4. Validation performed.
5. Follow-up work intentionally left to separate pull requests.

Example:

```markdown
### Why

Reverse DNS lookups in the TCP accept path can block the network loop.

### What changed

- Logs numeric IP addresses instead of host names.
- Removes `gethostbyaddr()` from connection logging.

### Follow-up

- Optional asynchronous hostname resolution can be a separate PR.
```
