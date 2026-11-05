---
name: cpp-brace-initialization
description: Use modern C++ brace initialization for variables and class members when writing or editing C++ code. Apply to new declarations, member initializers and local variables; do not reformat untouched lines.
---

# Modern C++ brace initialization

Use brace initialization instead of `=` for initializing variables and class
members in C++.

## Rules

- Prefer `bool error_logged{ false };` over `bool error_logged = false;`.
- Prefer `int count{ 0 };` over `int count = 0;`.
- Value-initialize pointers with braces: `std::byte* params_data{};`.
- Keep `auto x = expr;` when the value is the result of an expression, such as
  `auto res = fopen( path, "w+b" );`, because the project prefers `auto` for
  type inference.
- Do not change function default arguments: `int auto_init_params = 1` stays as
  is.
- Do not rewrite existing constructor initializer lists just for this style;
  follow the style of the file being edited.
- Only convert lines the task touches. Reformatting untouched code is out of
  scope.

## Why

Brace initialization rejects narrowing conversions, so `int size{ 2.5 };` fails
to compile instead of silently truncating. That matters in this codebase, where
sizes are converted between `size_t`, `u_int` and `double`, and where explicit
`static_cast` is already used, for example
`static_cast<size_t>( CONSTANTS::C_TOTAL_PARAMS_SIZE )`.

## Project formatting

- Keep line length at or below 80 characters.
- Comments are complete sentences ending with a period.
- Match the spacing style of the surrounding file.
