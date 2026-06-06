// Color / Style 单元测试（零依赖，基于 assert）
//
// 直接断言生成的 ANSI 转义序列（\x1b 为 ESC）。
#include <ceh/ui/style.hpp>

#include <iostream>
#include <string>

using ceh::ui::Color;
using ceh::ui::Style;

static int g_checks = 0;

#define CHECK_EQ(expr, expected)                                       \
    do {                                                               \
        ++g_checks;                                                    \
        std::string got = (expr);                                     \
        std::string exp = (expected);                                 \
        if (got != exp) {                                             \
            std::cerr << "FAIL [" << __LINE__ << "] " #expr "\n";     \
            std::exit(1);                                             \
        }                                                            \
    } while (0)

#define CHECK_TRUE(expr)                                               \
    do {                                                               \
        ++g_checks;                                                    \
        if (!(expr)) {                                                 \
            std::cerr << "FAIL [" << __LINE__ << "] " #expr "\n";      \
            std::exit(1);                                              \
        }                                                              \
    } while (0)

int main() {
    // ---- Color 前景/背景参数 ----
    CHECK_EQ(Color::red().fg_code(), "31");
    CHECK_EQ(Color::red().bg_code(), "41");
    CHECK_EQ(Color::black().fg_code(), "30");
    CHECK_EQ(Color::white().fg_code(), "37");
    CHECK_EQ(Color::green().bg_code(), "42");

    // 高亮 16 色：前景 90-97，背景 100-107
    CHECK_EQ(Color::bright_red().fg_code(), "91");
    CHECK_EQ(Color::bright_red().bg_code(), "101");
    CHECK_EQ(Color::bright_white().fg_code(), "97");

    // 256 调色板
    CHECK_EQ(Color::palette(200).fg_code(), "38;5;200");
    CHECK_EQ(Color::palette(200).bg_code(), "48;5;200");
    CHECK_EQ(Color::palette(0).fg_code(), "30");      // 0-7 仍走标准色

    // 真彩色
    CHECK_EQ(Color::rgb(255, 128, 0).fg_code(), "38;2;255;128;0");
    CHECK_EQ(Color::rgb(255, 128, 0).bg_code(), "48;2;255;128;0");

    // 默认色：无输出
    CHECK_TRUE(Color::default_color().is_default());
    CHECK_EQ(Color::default_color().fg_code(), "");
    CHECK_TRUE(!Color::red().is_default());

    // ---- Style SGR 序列 ----
    CHECK_EQ(Style().fg(Color::red()).bold().sgr(), "\x1b[1;31m");
    CHECK_EQ(Style().bg(Color::blue()).sgr(), "\x1b[44m");
    // 多属性按固定顺序：粗体(1) 下划线(4) 前景绿(32) 背景白(47)
    CHECK_EQ(Style().bold().underline().fg(Color::green()).bg(Color::white()).sgr(),
             "\x1b[1;4;32;47m");
    CHECK_EQ(Style().fg(Color::rgb(10, 20, 30)).sgr(), "\x1b[38;2;10;20;30m");

    // ---- 空样式 ----
    CHECK_TRUE(Style().empty());
    CHECK_EQ(Style().sgr(), "");
    CHECK_EQ(Style().apply("plain"), "plain");        // 无样式不加任何序列
    CHECK_TRUE(!Style().bold().empty());

    // ---- apply 包裹 ----
    CHECK_EQ(Style().fg(Color::red()).bold().apply("错误"),
             "\x1b[1;31m错误\x1b[0m");
    CHECK_EQ(Style().underline().apply("x"), "\x1b[4mx\x1b[0m");

    // ---- 属性可关闭 ----
    CHECK_TRUE(Style().bold().bold(false).empty());
    CHECK_EQ(Style().bold(true).italic().bold(false).sgr(), "\x1b[3m");  // 仅剩斜体

    // ---- 便捷函数 ----
    CHECK_EQ(ceh::ui::bold("hi"), "\x1b[1mhi\x1b[0m");
    CHECK_EQ(ceh::ui::underline("hi"), "\x1b[4mhi\x1b[0m");
    CHECK_EQ(ceh::ui::colored("hi", Color::cyan()), "\x1b[36mhi\x1b[0m");
    CHECK_EQ(ceh::ui::stylize("hi", Style().fg(Color::yellow())), "\x1b[33mhi\x1b[0m");

    // ---- reset 常量 ----
    CHECK_EQ(std::string(ceh::ui::reset_sequence()), "\x1b[0m");

    std::cout << "All Style tests passed (" << g_checks << " checks).\n";
    return 0;
}
