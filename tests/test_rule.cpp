// Rule 单元测试（零依赖，基于 assert）
#include <ceh/ui/rule.hpp>

#include <iostream>
#include <string>

using ceh::ui::horizontal_rule;
using ceh::ui::titled_rule;
using ceh::ui::repeat_fill;
using ceh::ui::display_width;
using ceh::ui::Style;
using ceh::ui::Color;

static int g_checks = 0;

#define CHECK_EQ(expr, expected)                                       \
    do {                                                               \
        ++g_checks;                                                    \
        auto got = (expr);                                            \
        if (!(got == (expected))) {                                    \
            std::cerr << "FAIL [" << __LINE__ << "] " #expr "\n";      \
            std::exit(1);                                              \
        }                                                            \
    } while (0)

int main() {
    // ---- repeat_fill 精确铺满列宽 ----
    CHECK_EQ(repeat_fill("-", 5), std::string("-----"));
    CHECK_EQ(repeat_fill("-", 0), std::string(""));
    CHECK_EQ(display_width(repeat_fill("─", 4)), std::size_t(4));   // 宽 1 的 ─
    CHECK_EQ(display_width(repeat_fill("中", 5)), std::size_t(5));  // 宽 2 + 空格补齐
    CHECK_EQ(repeat_fill("", 3), std::string("   "));              // 空 fill -> 空格

    // ---- horizontal_rule ----
    CHECK_EQ(horizontal_rule(8, "-"), std::string("--------"));
    CHECK_EQ(display_width(horizontal_rule(20)), std::size_t(20)); // 默认 ─

    // ---- titled_rule（ASCII，便于断言）----
    CHECK_EQ(titled_rule("hi", 11, "-"), std::string("--- hi ----"));
    CHECK_EQ(display_width(titled_rule("hi", 11, "-")), std::size_t(11));
    CHECK_EQ(display_width(titled_rule("title", 30, "-")), std::size_t(30));

    // CJK 标题也应对齐到目标列宽
    CHECK_EQ(display_width(titled_rule("配置", 20, "-")), std::size_t(20));
    CHECK_EQ(display_width(titled_rule("中", 9, "-")), std::size_t(9));

    // 宽度不足以容纳填充时，退化为只输出标题
    CHECK_EQ(titled_rule("x", 3, "-"), std::string("x"));
    CHECK_EQ(titled_rule("hello", 4, "-"), std::string("hello"));

    // ---- 带样式 ----
    CHECK_EQ(horizontal_rule(3, Style().fg(Color::red()), "-"),
             std::string("\x1b[31m---\x1b[0m"));
    // 标题样式只包住标题文字
    CHECK_EQ(titled_rule("hi", 11, "-", Style().bold()),
             std::string("--- \x1b[1mhi\x1b[0m ----"));

    std::cout << "All Rule tests passed (" << g_checks << " checks).\n";
    return 0;
}
