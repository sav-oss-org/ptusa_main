# Project agent instructions

## AI commit attribution

Any commit whose substantive content was produced with AI assistance must
carry provenance trailers in the final paragraph of the commit message (no
blank line between them). This applies regardless of who runs `git commit`
— the agent or the human committing agent-produced changes:

```
Co-authored-by: <agent-name> <agent-noreply-email>
AI-assisted: <agent-name> (model: <model-name>, <execution-mode>)
```

- `<agent-name>` and `<agent-noreply-email>` identify the assistant that
  produced the change: its display name and GitHub noreply e-mail, taken
  from that agent. Never credit an unrelated assistant and never reuse
  another agent's identity.
- `<model-name>` is the model reported by the agent at commit time,
  verbatim (e.g. `qwen3.8-flash-next-unsloth-ud-iq4_xs`). Do not invent
  or normalize it.
- `<execution-mode>` documents where the model runs: `local` for local
  models, the hosted provider otherwise.
- For commits produced by the Zed agent running a local model, the
  trailers are:

  ```
  Co-authored-by: Zed AI Agent <253101093+zed-agent@users.noreply.github.com>
  AI-assisted: Zed agent (model: <model-name>, local)
  ```

- Human-only commits need no trailers.
- Trailer lines are identifiers, not prose: never line-wrap or reflow them,
  even when they exceed the usual 72-character body limit (this overrides
  any wrapping rules).

Issues and pull requests created by the agent start their body with:
`🤖 Generated with AI assistance`.

See `docs/CONTRIBUTING.md` and `.github/copilot-instructions.md` for the
project contribution and style rules.

## Response links

When an agent reply mentions a GitHub pull request, issue, review
thread, comment, commit, or CI run, render the reference as a
Markdown link with the full URL so the reader can click through
directly.
