---
name: ai-commit-attribution
description: Use when creating commits, issues or pull requests with AI assistance.
---

# AI commit attribution

Follow `AGENTS.md`, `docs/CONTRIBUTING.md` and
`.github/copilot-instructions.md`.

- Put provenance trailers in the final paragraph of the commit message,
  with no blank line between them:

  ```
  Co-authored-by: <agent-name> <agent-noreply-email>
  AI-assisted: <agent-name> (model: <model-name>, <execution-mode>)
  ```

- Take `<agent-name>` and `<agent-noreply-email>` from the assistant
  that produced the change; never credit an unrelated agent.
- Use the model name reported by the agent verbatim; never invent or
  normalize it.
- Document the execution mode: `local` for local models, the hosted
  provider otherwise.
- For the Zed agent running a local model the trailers are:

  ```
  Co-authored-by: Zed AI Agent <253101093+zed-agent@users.noreply.github.com>
  AI-assisted: Zed agent (model: <model-name>, local)
  ```

- Never line-wrap or reflow trailer lines, even when they are longer
  than 72 characters.
- Human-only commits need no trailers.
- Start the body of AI-created issues and pull requests with
  `🤖 Generated with AI assistance`.
