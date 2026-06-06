// Panel 单元测试（零依赖，基于 assert）
#include <ceh/ui/panel.hpp>

#include <iostream>
#include <string>

using namespace ceh::ui;

static int g_checks = 0;

#define CHECK_EQ(expr, expected)                                       \
    do {                                                               \
        ++g_checks;                                                    \
        std::string got = (expr);                                     \
        std::string exp = (expected);                                 \
        if (got != exp) {                                             \
            std::cerr << "FAIL [" << __LINE__ << "]\n--- got ---\n"   \
                      << got << "\n--- expected ---\n" << exp << "\n";\
            std::exit(1);                                             \
        }                                                            \
    } while (0)

#define CHECK_TRUE(expr)                                               \
    do { ++g_checks; if (!(expr)) {                                    \
        std::cerr << "FAIL [" << __LINE__ << "] " #expr "\n"; std::exit(1);} } while (0)

int main() {
    // 带标题，多行内容（ASCII 边框）
    {
        Panel p("Hello\nWorld!");
        p.set_border(BorderStyle::Ascii).set_title("Hi");
        CHECK_EQ(p.render(false),
                 "+-- Hi --+\n"
                 "| Hello  |\n"
                 "| World! |\n"
                 "+--------+");
    }

    // 无标题
    {
        Panel p("ab");
        p.set_border(BorderStyle::Ascii);
        CHECK_EQ(p.render(false),
                 "+----+\n"
                 "| ab |\n"
                 "+----+");
    }

    // 标题左对齐
    {
        Panel p("content");
        p.set_border(BorderStyle::Ascii).set_title("T").set_title_align(Align::Left);
        // 内容宽 7，inner 9；标题 " T " 占 3，左 0 右 6
        CHECK_EQ(p.render(false),
                 "+ T ------+\n"
                 "| content |\n"
                 "+---------+");
    }

    // 固定宽度（比内容更宽）
    {
        Panel p("x");
        p.set_border(BorderStyle::Ascii).set_width(5);
        CHECK_EQ(p.render(false),
                 "+-------+\n"
                 "| x     |\n"
                 "+-------+");
    }

    // 启用样式时边框被着色
    {
        Panel p("x");
        p.set_border(BorderStyle::Ascii).set_border_style(Style().fg(Color::red()));
        CHECK_TRUE(p.render(true).find("\x1b[31m") != std::string::npos);
        CHECK_TRUE(p.render(false).find("\x1b[") == std::string::npos);
    }

    std::cout << "All Panel tests passed (" << g_checks << " checks).\n";
    return 0;
}
