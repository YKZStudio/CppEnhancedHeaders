# CLAUDE.md

本文件为 [Claude Code](https://claude.ai/code) 在本仓库工作时提供指引。

## 项目概览

**CppEnhancedHeaders** 是一套 **纯头文件（header-only）** 的现代 C++ 增强库，
把标准库没有、但项目里常被重复造的轮子（高精度类型、数据结构、算法、工具）
预制成「`#include` 即用」的组件。完整愿景与模块路线图见 [README.md](./README.md)。

- 语言标准：**C++17**（部分模块可选启用 C++20）。
- 统一命名空间：所有公开内容位于 `ceh::` 下；实现细节放在 `ceh::detail::`。
- 零链接依赖：只需把 `include/` 加入头文件搜索路径。

## 目录结构

```
include/ceh/
  ├── types/        # 增强数据类型（big_int / fraction / safe_int / complex …）
  ├── structures/   # 数据结构（规划中）
  ├── algorithms/   # 算法（规划中）
  └── utils/        # 实用工具（规划中）
tests/              # 单元测试，每个组件一个 test_<name>.cpp
examples/           # 使用示例，每个组件一个 <name>_example.cpp
CMakeLists.txt      # INTERFACE 库 ceh::ceh + CTest
.github/workflows/  # CI（ubuntu / macos / windows 上 cmake + ctest）
```

## 构建与测试

```bash
# 配置 + 构建（含测试与示例）
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j

# 跑全部测试
ctest --test-dir build --output-on-failure

# 单独跑某个测试可执行文件
./build/test_big_int

# 快速验证某个头文件（无需 CMake）
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude tests/test_big_int.cpp -o /tmp/t && /tmp/t
```

提交前务必保证：`ctest` 全绿，且 `-Wall -Wextra -Wpedantic` 零警告。

## 编码约定

- **header-only**：所有实现写在 `.hpp` 中；自由函数用 `inline`，类内方法默认内联。
- **头文件保护**：用 `#ifndef CEH_<PATH>_HPP` 形式的 include guard（不用 `#pragma once`）。
- **命名**：类型 `PascalCase`，函数/变量/方法 `snake_case`，宏与常量 `CEH_UPPER_CASE`。
- **命名空间**：公开 API 在 `ceh::`，内部实现细节藏进 `ceh::detail::`。
- **错误处理**：用标准异常表达错误——除零用 `std::domain_error`，
  溢出用 `std::overflow_error`，非法输入用 `std::invalid_argument`。
- **文档**：每个头文件顶部用注释块说明用途、不变式、复杂度与用法示例。
- **可移植性**：避免编译器内建（如 `__builtin_*`），保证 MSVC / Clang / GCC 都能编。

## 新增一个模块的步骤

1. 在 `include/ceh/<分类>/<name>.hpp` 实现组件（遵循上面的约定）。
2. 在 `tests/test_<name>.cpp` 写零依赖的 assert 单测（参照现有测试的 `CHECK_*` 宏风格）。
3. 在 `examples/<name>_example.cpp` 写一个简短示例。
4. 在 `CMakeLists.txt` 的 `CEH_BUILD_TESTS` / `CEH_BUILD_EXAMPLES` 块中注册新目标。
5. 在 `README.md` 的路线图表格中把对应行标记为 ✅，并补上能力说明。
6. 本地 `ctest` 全绿后再提交。

## Git / PR 约定

- 在指定的功能分支上开发，不要直接推 `main`。
- commit message 用中文、聚焦「做了什么、为什么」；遵循 `type(scope): 摘要` 形式
  （如 `feat(types): ...`、`docs: ...`、`fix(structures): ...`）。
- 推送后为分支创建 **draft PR**；PR 描述说明实现要点与验证方式。
