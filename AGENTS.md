# Project agent instructions

## AI commit attribution

Any commit whose substantive content was produced with AI assistance must
carry provenance trailers in the final paragraph of the commit message (no
blank line between them). This applies regardless of who runs `git commit`
— the agent or the human committing agent-produced changes:

```
Co-authored-by: Zed AI Agent <253101093+zed-agent@users.noreply.github.com>
AI-assisted: Zed agent (model: <model-name>, local)
```

- `<model-name>` is the model reported by the agent at commit time, verbatim
  (e.g. `qwen3.8-flash-next-unsloth-ud-iq4_xs`). Do not invent or normalize it.
- The model runs locally; the trailer documents this. If the setup moves to a
  hosted model, replace `local` accordingly.
- Human-only commits need no trailers.
- Trailer lines are identifiers, not prose: never line-wrap or reflow them,
  even when they exceed the usual 72-character body limit (this overrides
  any wrapping rules).

Issues and pull requests created by the agent start their body with:
`🤖 Generated with AI assistance`.

See `docs/CONTRIBUTING.md` and `.github/copilot-instructions.md` for the
project contribution and style rules.
