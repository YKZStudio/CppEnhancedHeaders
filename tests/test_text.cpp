// Text 单元测试（零依赖，基于 assert）
#include <ceh/ui/text.hpp>

#include <iostream>
#include <string>

using ceh::ui::Text;
using ceh::ui::Style;
using ceh::ui::Color;
using ceh::ui::display_width;

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

#define CHECK_TRUE(expr)                                               \
    do {                                                               \
        ++g_checks;                                                    \
        if (!(expr)) {                                                 \
            std::cerr << "FAIL [" << __LINE__ << "] " #expr "\n";      \
            std::exit(1);                                              \
        }                                                              \
    } while (0)

int main() {
    // ---- display_width：ASCII 1 列，CJK 全角 2 列 ----
    CHECK_EQ(display_width(""), std::size_t(0));
    CHECK_EQ(display_width("abc"), std::size_t(3));
    CHECK_EQ(display_width("中文"), std::size_t(4));       // 两个汉字 = 4 列
    CHECK_EQ(display_width("a中b"), std::size_t(4));       // 1 + 2 + 1
    CHECK_EQ(display_width("你好world"), std::size_t(9));  // 2+2 + 5
    CHECK_EQ(display_width("ＡＢ"), std::size_t(4));       // 全角字母各 2 列
    CHECK_EQ(display_width("a\tb"), std::size_t(2));       // 控制字符(Tab) 0 列

    // ---- 富文本拼接 / 渲染 ----
    {
        Text t;
        t.append("错误：", Style().fg(Color::red()).bold());
        t.append("文件未找到");
        CHECK_EQ(t.plain(), std::string("错误：文件未找到"));
        // 带样式渲染：第一段有 ANSI，第二段无
        CHECK_EQ(t.render(true),
                 std::string("\x1b[1;31m错误：\x1b[0m文件未找到"));
        // 不带样式 == plain
        CHECK_EQ(t.render(false), t.plain());
        // 宽度：5 个汉字 + 全角冒号？"错误：" = 3 全角(6) + "文件未找到" 5 全角(10) = 16
        CHECK_EQ(t.width(), std::size_t(16));
    }

    // 构造时即带样式
    {
        Text t("hi", Style().underline());
        CHECK_EQ(t.render(true), std::string("\x1b[4mhi\x1b[0m"));
        CHECK_EQ(t.width(), std::size_t(2));
    }

    // 空文本
    {
        Text t;
        CHECK_TRUE(t.empty());
        CHECK_EQ(t.plain(), std::string(""));
        CHECK_EQ(t.width(), std::size_t(0));
        t.append("");                 // 空串不产生片段
        CHECK_TRUE(t.empty());
    }

    // operator+= 合并
    {
        Text a("foo");
        Text b("bar", Style().bold());
        a += b;
        CHECK_EQ(a.plain(), std::string("foobar"));
        CHECK_EQ(a.spans().size(), std::size_t(2));
    }

    // 无样式片段渲染不含转义序列
    {
        Text t("plain");
        CHECK_EQ(t.render(true), std::string("plain"));
    }

    std::cout << "All Text tests passed (" << g_checks << " checks).\n";
    return 0;
}
