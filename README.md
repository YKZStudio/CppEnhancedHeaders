# CppEnhancedHeaders

> 一组现代化、header-only 的 C++ 增强头文件库 —— 把常用的算法、数据结构、实用工具与终端 UI，做成「拿来即用」的预制组件。

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
| ✅ | `BigDecimal` | 任意精度十进制小数（基于 BigInt，多种舍入模式、精确十进制运算） |
| ✅ | `Fraction` | 有理数（基于 BigInt，自动约分、负指数幂、to_double） |
| ✅ | `SafeInt` | 带溢出检测的整数包装（溢出/除零抛异常，跨编译器一致） |
| ✅ | `Complex` | 增强版复数（与 std::complex 互操作、整数幂、极坐标、a+bi 输出） |

### 数据结构 `ceh/structures`
| 状态 | 组件 | 说明 |
|------|------|------|
| ✅ | `BinarySearchTree` | 二叉搜索树（增删查、三序遍历、min/max、高度、深拷贝、自定义比较器） |
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
| ✅ | `sort` | 排序合集（插入、归并[稳定]、快排[三数取中]、堆排，泛型迭代器 + 比较器） |
| ✅ | `search` | 二分查找及变体（下标版 binary/lower/upper_bound、二分答案） |
| ⬜ | `graph` | 图算法（BFS/DFS、Dijkstra、最小生成树、拓扑排序） |
| ⬜ | `string` | 字符串算法（KMP、Trie 匹配、最长公共子串） |
| ✅ | `math` | 数论算法（GCD/LCM、整数与模幂、素性判定、埃氏筛、扩展欧几里得、模逆元） |
| ⬜ | `dp` | 常见动态规划模板 |

### 实用工具 `ceh/utils`
| 状态 | 组件 | 说明 |
|------|------|------|
| ⬜ | `Timer` | 计时 / 性能测量 |
| ⬜ | `Logger` | 轻量日志 |
| ⬜ | `StringUtils` | 字符串拆分、修剪、格式化 |
| ⬜ | `Random` | 易用的随机数封装 |

### 终端 UI `ceh/ui`

> 灵感来自 [FTXUI](https://github.com/ArthurSonzogni/FTXUI) 与 Python 的
> [Rich](https://github.com/Textualize/rich)：把「在终端里画出漂亮的表格、面板、
> 进度条、树状结构」这类需求，做成纯头文件、零外部依赖的组件。仅依赖标准库 +
> 各平台终端能力探测，跨 Linux / macOS / Windows（含 Windows 10+ 的 ANSI 支持）。

**渲染基础**

| 状态 | 组件 | 说明 |
|------|------|------|
| ⬜ | `Color` / `Style` | ANSI 颜色与文本样式（16 / 256 / truecolor，粗体、斜体、下划线、反色，前景/背景） |
| ⬜ | `Text` | 富文本片段：带样式拼接、对齐、截断、自动换行，正确处理 CJK 全角宽度 |
| ⬜ | `Console` | 终端输出封装：样式标记解析、TTY 检测、终端宽高探测、光标控制 |
| ⬜ | `Rule` | 水平分隔线（可带居中标题） |

**组件 / 控件**

| 状态 | 组件 | 说明 |
|------|------|------|
| ⬜ | `Table` | 表格：表头、列对齐、自动列宽，多种边框风格（方角 / 圆角 / 双线 / 无框） |
| ⬜ | `Panel` | 带边框与标题的面板（可嵌套、内边距、对齐） |
| ⬜ | `Tree` | 树形结构渲染（缩进与连接线，可对接 `ceh::BinarySearchTree`） |
| ⬜ | `ProgressBar` | 进度条：百分比、速率、已用 / 剩余时间（ETA），多任务进度组 |
| ⬜ | `Spinner` | 加载动画（多种帧风格） |
| ⬜ | `Markdown` | 简易 Markdown 渲染（标题 / 列表 / 强调 / 代码块 / 分隔线） |

**布局 / 绘图 / 交互**

| 状态 | 组件 | 说明 |
|------|------|------|
| ⬜ | `Layout` | 盒式布局：水平 / 垂直容器、flex 伸缩、分割窗格（参考 FTXUI 的 hbox / vbox） |
| ⬜ | `Canvas` | 字符画布：点 / 线 / 矩形，盲文（Braille）子像素绘制、折线图 |
| ⬜ | `Live` | 可刷新的实时区域：原地重绘、帧率控制 |
| ⬜ | `Prompt` | 交互式输入：文本 / 确认（y-n）/ 单选 / 多选菜单 |

> **设计构想**：底层先做好 `Color`/`Style`/`Text`/`Console` 四件套（负责样式与
> 宽度计算），上层组件（`Table`、`Panel`、`ProgressBar` 等）都把自己渲染成
> `Text` 行再交给 `Console` 输出；`Layout`/`Canvas` 提供二维拼接与绘图能力，
> `Live`/`Prompt` 负责动态刷新与交互。所有组件遵循库的统一约定：header-only、
> `ceh::ui::` 命名空间、零外部依赖。

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
#include <ceh/structures/binary_search_tree.hpp>
#include <iostream>

int main() {
    // 高精度整数：计算 100!
    ceh::BigInt factorial = 1;
    for (int i = 1; i <= 100; ++i) factorial *= i;
    std::cout << "100! = " << factorial << '\n';

    // 二叉搜索树
    ceh::BinarySearchTree<int> tree;
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
│       ├── ui/             # 终端 UI（规划中）
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
