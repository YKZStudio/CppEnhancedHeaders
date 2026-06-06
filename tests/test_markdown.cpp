// Markdown 单元测试（零依赖，基于 assert）
#include <ceh/ui/markdown.hpp>

#include <iostream>
#include <string>

using namespace ceh::ui;

static int g_checks = 0;

#define CHECK_EQ(expr, expected)                                       \
    do { ++g_checks; std::string got = (expr); std::string exp = (expected); \
        if (got != exp) { std::cerr << "FAIL [" << __LINE__ << "] got [" \
            << got << "] exp [" << exp << "]\n"; std::exit(1);} } while (0)

#define CHECK_TRUE(expr)                                               \
    do { ++g_checks; if (!(expr)) {                                    \
        std::cerr << "FAIL [" << __LINE__ << "] " #expr "\n"; std::exit(1);} } while (0)

int main() {
    Markdown md;

    // ---- 块级（无样式，便于断言结构）----
    CHECK_EQ(md.render("# Title", false), "Title");
    CHECK_EQ(md.render("### Sub", false), "Sub");
    CHECK_EQ(md.render("- item", false), "  • item");
    CHECK_EQ(md.render("* item", false), "  • item");
    CHECK_EQ(md.render("> quote", false), "│ quote");

    // 行内格式去标记
    CHECK_EQ(md.render("a **bold** b", false), "a bold b");
    CHECK_EQ(md.render("a `code` b", false), "a code b");
    CHECK_EQ(md.render("a *it* b", false), "a it b");

    // 代码块：围栏被吃掉，内部原样
    CHECK_EQ(md.render("```\nx = 1\n```", false), "x = 1");
    CHECK_EQ(md.render("```cpp\nint a;\nint b;\n```", false), "int a;\nint b;");

    // 分隔线
    CHECK_TRUE(display_width(Markdown().set_width(10).render("---", false)) == 10);
    CHECK_TRUE(display_width(Markdown().set_width(8).render("***", false)) == 8);

    // 多行综合（无样式）
    CHECK_EQ(md.render("# 标题\n- 甲\n- 乙", false), "标题\n  • 甲\n  • 乙");

    // ---- 带样式 ----
    CHECK_EQ(md.render("**b**", true), "\x1b[1mb\x1b[0m");
    CHECK_EQ(md.render("`c`", true), "\x1b[33mc\x1b[0m");
    CHECK_EQ(md.render("*i*", true), "\x1b[3mi\x1b[0m");
    // 一级标题：粗体 + 下划线 + 青色
    CHECK_EQ(md.render("# T", true), "\x1b[1;4;36mT\x1b[0m");

    // 普通段落原样
    CHECK_EQ(md.render("just text", false), "just text");

    std::cout << "All Markdown tests passed (" << g_checks << " checks).\n";
    return 0;
}
