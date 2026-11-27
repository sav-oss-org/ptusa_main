# GitHub Issue Templates / Шаблоны GitHub Issues

This directory contains issue templates for the ptusa_main project to help
contributors create well-structured and informative issues.

Эта директория содержит шаблоны issues для проекта ptusa_main, чтобы помочь
участникам создавать хорошо структурированные и информативные issues.

## Available Templates / Доступные шаблоны

### 🐛 Bug Report / Отчет об ошибке (`bug_report.yml`)

Use this template to report bugs, unexpected behavior, or technical issues.

- Platform selection (Windows, Linux, PLCnext)
- Environment details (compiler, CMake, toolchain versions)
- Lua script context
- Error logs and debugging information

Используйте этот шаблон для сообщения об ошибках, неожиданном поведении или
технических проблемах.

### ✨ Feature Request / Запрос функции (`feature_request.yml`)

Use this template to suggest new features or enhancements.

- Component categorization
- Priority levels
- Implementation ideas
- Use cases and compatibility considerations

Используйте этот шаблон для предложения новых функций или улучшений.

### 💬 General Discussion / Общее обсуждение (`general.yml`)

Use this template for questions, discussions, or general help.

- Multiple category options
- Context and environment information
- Code/configuration sharing

Используйте этот шаблон для вопросов, обсуждений или общей помощи.

### 📚 Documentation Issue / Проблема с документацией (`documentation.yml`)

Use this template to report documentation issues or suggest improvements.

- Issue type categorization
- Language-specific targeting (English/Russian)
- Audience specification
- Related files tracking

Используйте этот шаблон для сообщения о проблемах с документацией или
предложения улучшений.

### 🔌 Device/Sensor Addition / Добавление устройства/датчика (`device_request.yml`)

Use this template to request addition of new device or sensor types.

- Device type categorization (valves, motors, sensors, etc.)
- Interface type specification (IO-Link, Analog, Digital, etc.)
- Hardware specifications and I/O configuration
- Lua interface requirements
- Control logic description
- Use cases and implementation notes

Используйте этот шаблон для запроса добавления новых типов устройств или
датчиков.

## Template Features / Особенности шаблонов

- **Bilingual support** / **Поддержка двух языков**: All templates include both
  English and Russian text
- **Project-specific fields** / **Поля, специфичные для проекта**: Tailored for
  industrial automation and PLCnext technology
- **Structured input** / **Структурированный ввод**: Dropdown menus, checkboxes,
  and text areas for organized information
- **Required validation** / **Обязательная проверка**: Important fields are
  marked as required
- **Context-aware** / **Учет контекста**: Different templates for different
  types of issues

## Configuration / Конфигурация

The `config.yml` file:

- Disables blank issues to encourage template usage
- Provides contact links to documentation and maintainers
- Includes bilingual descriptions

Файл `config.yml`:

- Отключает пустые issues для поощрения использования шаблонов
- Предоставляет ссылки на документацию и сопровождающих
- Включает двуязычные описания

## Usage / Использование

When creating a new issue on GitHub, users will see these templates as options.
The templates guide users to provide comprehensive information that helps
maintainers understand and address issues efficiently.

При создании нового issue на GitHub пользователи увидят эти шаблоны как опции.
Шаблоны помогают пользователям предоставить исчерпывающую информацию, которая
помогает сопровождающим понимать и решать проблемы эффективно.
