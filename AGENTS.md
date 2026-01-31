# AGENTS.md - Cathedral Assets Optimizer

This file contains instructions and guidelines for AI agents working on the Cathedral Assets Optimizer project.

## Project Overview

Cathedral Assets Optimizer (CAO) is a C++/Qt application for optimizing game assets (meshes, textures, animations) for Bethesda games like Skyrim, Fallout, etc. It features both a GUI and CLI interface.

## Development Environment Setup

### Prerequisites
- CMake 3.18+
- vcpkg package manager (set `VCPKG_ROOT` environment variable)
- Qt 6
- C++20 compatible compiler (MSVC 2022, GCC, or Clang)

### Build Instructions

1. Configure with CMake preset:
   ```bash
   # Windows with Visual Studio
   cmake --preset vs2022-windows

   # Windows with Ninja
   cmake --preset ninja-windows

   # Linux with Ninja
   cmake --preset ninja-linux
   ```

2. Build:
   ```bash
   cmake --build build
   ```

See the [project wiki](https://gitlab.com/G_ka/sse-assets-optimiser/wikis/Build-instructions) for detailed setup.

## Code Style Guidelines

### Formatting
- Use `.clang-format` in the repository root for automatic formatting
- Run `clang-format -i <file>` before committing
- Key style points:
  - Brace wrapping: custom (braces on new line for functions, classes, control statements)
  - Indent: 4 spaces
  - Line length: flexible but reasonable
  - Align consecutive assignments

### Naming Conventions
- Classes/Types: `PascalCase`
- Functions/Methods: `snake_case`
- Variables: `snake_case`
- Private members: suffix with underscore `member_`
- Constants: `k_constant_name`
- Namespaces: `snake_case` (main namespace: `cao`)

### Code Organization
- Header files: `.hpp` extension
- Implementation files: `.cpp` extension
- Include guards: `#pragma once`
- Structure: `src/` for source files, `tests/` for tests

## Architecture Notes

### Key Components
- **Manager**: Orchestrates optimization process, handles mod folders
- **Settings/Profile**: Configuration management, JSON-serializable with nlohmann/json
- **MainWindow**: Qt-based GUI
- **main_process**: Core processing logic for meshes, textures, animations

### Important Patterns
- Settings are passed by value to `Manager::run_optimization()` to ensure isolation
- Profile class uses `NLOHMANN_DEFINE_TYPE_INTRUSIVE` for JSON serialization
- Qt QString to std::u8string conversion: use `cao::to_u8string()` from `gui/utils/utils.hpp`
- File processing uses BTU library functions (btu::nif, btu::tex, btu::hkx)

## Testing Instructions

Currently, the test infrastructure is minimal (see `tests/CMakeLists.txt`). When adding tests:
- Build with `BUILD_TESTING=ON` (enabled in cmake-dev preset)
- Tests should go in `tests/` directory
- Link against `CAO_LIB` target

## Making Changes

### Before Changing Code
1. Understand the existing architecture by reading relevant source files
2. Check if similar functionality exists elsewhere
3. Keep changes minimal and focused
4. Split large functions into smaller, testable units (max ~80 lines per function)

### Code Quality
- Prefer modern C++ features (C++20)
- Use RAII for resource management
- Avoid raw pointers; use smart pointers or references
- Handle errors with exceptions or `tl::expected`
- Add comments for non-obvious logic
- Keep functions focused on a single responsibility

### Common Pitfalls
- Qt's command-line parser requires `QCoreApplication` to be created first
- Don't forget `#include "settings/json.hpp"` when using JSON functions
- Profile modifications should be done on copies to avoid side effects
- Manager instances store state, create new ones for independent operations

## Git Workflow

- Keep commits focused and atomic
- Use descriptive commit messages
- Reference issue numbers where applicable
- Avoid committing build artifacts, IDE files, or generated code
