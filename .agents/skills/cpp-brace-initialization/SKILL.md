---
name: cpp-brace-initialization
description: Use brace initialization for new or touched C++ code.
---

# Modern C++ brace initialization

Follow `docs/style.md` and `.github/copilot-instructions.md`.

- Prefer `bool error_logged{ false };` over `bool error_logged = false;`.
- Prefer `int count{ 0 };` over `int count = 0;`.
- Value-initialize pointers with braces: `std::byte* params_data{};`.
- Keep `auto x = expr;` when the value is the result of an expression.
- Apply the rule to touched code only; do not reformat untouched lines only
  for brace initialization.
