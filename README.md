# CppEnhancedHeaders

> 一组现代化、header-only 的 C++ 增强头文件库 —— 把常用的算法、数据结构与实用工具，做成「拿来即用」的预制组件。

[![CI](https://github.com/YKZStudio/CppEnhancedHeaders/actions/workflows/ci.yml/badge.svg)](https://github.com/YKZStudio/CppEnhancedHeaders/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](./LICENSE)
[![C++17](https://img.shields.io/badge/C%2B%2B-17%2F20-blue.svg)](https://isocpp.org/)
[![Header Only](https://img.shields.io/badge/header--only-yes-brightgreen.svg)](#)

---

## ✨ 这是什么

`CppEnhancedHeaders` 是一套 **纯头文件（header-only）** 的 C++ 增强库。
目标是把那些「标准库没有、但写项目时经常要重复造轮子」的东西，预先实现好、测试好，
让你只要 `#include` 一行就能用。

- 🚀 **零编译依赖**：只需把 `include/` 目录加入头文件搜索路径，无需链接 `.lib` / `.a`。
- 🧩 **模块化**：每个功能独立成头文件，按需引入，不想要的不会进二进制。
- 🛡️ **现代 C++**：基于 C++17/20，善用模板、`constexpr`、概念（concepts）。
- 📦 **统一命名空间**：所有内容位于 `ceh::`（**C**pp **E**nhanced **H**eaders）下。

---

## 📚 规划中的模块

> 以下为路线图，✅ 表示已实现，🚧 表示开发中，⬜ 表示待规划。欢迎一起 vibe coding 补全。

### 数据类型 `ceh/types`
| 状态 | 组件 | 说明 |
|------|------|------|
| ✅ | `BigInt` | 任意精度整数（高精度加减乘除、取模、快速幂、比较、流 I/O） |
| ⬜ | `BigDecimal` | 任意精度定点/浮点小数 |
| ✅ | `Fraction` | 有理数（基于 BigInt，自动约分、负指数幂、to_double） |
| ⬜ | `SafeInt` | 带溢出检测的整数包装 |
| ⬜ | `Complex` | 增强版复数（与标准库互操作） |

### 数据结构 `ceh/structures`
| 状态 | 组件 | 说明 |
|------|------|------|
| ⬜ | `BinaryTree` / `BST` | 二叉树 / 二叉搜索树 |
| ⬜ | `AVLTree` | 自平衡 AVL 树 |
| ⬜ | `RedBlackTree` | 红黑树 |
| ⬜ | `Trie` | 字典树（前缀树） |
| ⬜ | `Heap` | 二叉堆 / 可合并堆 |
| ⬜ | `Graph` | 图（邻接表/邻接矩阵） |
| ⬜ | `LinkedList` | 增强版双向链表 |
| ⬜ | `DisjointSet` | 并查集（路径压缩 + 按秩合并） |
| ⬜ | `SkipList` | 跳表 |

### 算法 `ceh/algorithms`
| 状态 | 组件 | 说明 |
|------|------|------|
| ⬜ | `sort` | 排序合集（归并、快排、堆排、基数排序…） |
| ⬜ | `search` | 查找（二分及其变体） |
| ⬜ | `graph` | 图算法（BFS/DFS、Dijkstra、最小生成树、拓扑排序） |
| ⬜ | `string` | 字符串算法（KMP、Trie 匹配、最长公共子串） |
| ⬜ | `math` | 数学算法（GCD/LCM、素数筛、快速幂、组合数） |
| ⬜ | `dp` | 常见动态规划模板 |

### 实用工具 `ceh/utils`
| 状态 | 组件 | 说明 |
|------|------|------|
| ⬜ | `Timer` | 计时 / 性能测量 |
| ⬜ | `Logger` | 轻量日志 |
| ⬜ | `StringUtils` | 字符串拆分、修剪、格式化 |
| ⬜ | `Random` | 易用的随机数封装 |

---

## 🚀 快速开始

### 1. 获取代码

```bash
git clone https://github.com/YKZStudio/CppEnhancedHeaders.git
```

### 2. 引入头文件

把 `include/` 目录加入编译器的头文件搜索路径：

```bash
g++ -std=c++17 -I path/to/CppEnhancedHeaders/include main.cpp -o main
```

或在 CMake 中：

```cmake
add_subdirectory(CppEnhancedHeaders)
target_link_libraries(your_app PRIVATE ceh::ceh)
```

### 3. 使用示例

```cpp
#include <ceh/types/big_int.hpp>
#include <ceh/structures/binary_tree.hpp>
#include <iostream>

int main() {
    // 高精度整数：计算 100!
    ceh::BigInt factorial = 1;
    for (int i = 1; i <= 100; ++i) factorial *= i;
    std::cout << "100! = " << factorial << '\n';

    // 二叉搜索树
    ceh::BST<int> tree;
    for (int v : {5, 3, 8, 1, 4}) tree.insert(v);
    std::cout << "中序遍历: ";
    tree.inorder([](int v) { std::cout << v << ' '; });
    std::cout << '\n';
    return 0;
}
```

---

## 📂 目录结构

```
CppEnhancedHeaders/
├── include/
│   └── ceh/
│       ├── types/          # 增强数据类型
│       ├── structures/     # 数据结构
│       ├── algorithms/     # 算法
│       ├── utils/          # 实用工具
│       └── ceh.hpp         # 一键引入全部
├── examples/               # 使用示例
├── tests/                  # 单元测试
├── CMakeLists.txt
├── LICENSE
└── README.md
```

---

## 🤝 贡献

这是一个 vibe coding 项目，欢迎随时加新模块。建议遵循：

1. **header-only**：所有实现放在 `.hpp` 中（必要时用 `inline`）。
2. **命名空间**：统一放在 `ceh::` 下，子模块可用 `ceh::detail::` 隐藏实现细节。
3. **命名风格**：类型用 `PascalCase`，函数/变量用 `snake_case`，宏用 `CEH_UPPER_CASE`。
4. **测试先行**：每个组件在 `tests/` 下配套单元测试。
5. **文档**：在头文件顶部用注释说明用途、复杂度与用法示例。

---

## 📋 环境要求

- 支持 C++17 的编译器（GCC 7+ / Clang 5+ / MSVC 2017+）
- 部分模块可选启用 C++20 特性（concepts、`<ranges>`）
- 构建测试需要 CMake 3.14+

---

## 📄 许可证

本项目基于 [MIT License](./LICENSE) 开源 © 2026 Yakezhou Studio。
