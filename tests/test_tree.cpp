// Tree（UI 渲染）单元测试（零依赖，基于 assert）
#include <ceh/ui/tree.hpp>

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
    // 经典层级渲染
    {
        Tree root("root");
        auto& a = root.add_child("a");
        a.add_child("a1");
        a.add_child("a2");
        root.add_child("b").add_child("b1");
        CHECK_EQ(root.render(false),
                 "root\n"
                 "├─ a\n"
                 "│  ├─ a1\n"
                 "│  └─ a2\n"
                 "└─ b\n"
                 "   └─ b1");
        CHECK_TRUE(root.child_count() == 2);
        CHECK_TRUE(root.label() == "root");
    }

    // 单结点
    {
        Tree t("only");
        CHECK_EQ(t.render(false), "only");
        CHECK_TRUE(t.child_count() == 0);
    }

    // 深层链
    {
        Tree root("1");
        root.add_child("2").add_child("3").add_child("4");
        CHECK_EQ(root.render(false),
                 "1\n"
                 "└─ 2\n"
                 "   └─ 3\n"
                 "      └─ 4");
    }

    // 样式：启用时结点标签带 ANSI
    {
        Tree t("x", Style().fg(Color::green()));
        CHECK_TRUE(t.render(true).find("\x1b[32m") != std::string::npos);
        CHECK_EQ(t.render(false), "x");
    }

    std::cout << "All Tree(ui) tests passed (" << g_checks << " checks).\n";
    return 0;
}
