// Console 单元测试（零依赖，基于 assert）
#include <ceh/ui/console.hpp>

#include <iostream>
#include <sstream>
#include <string>

using ceh::ui::Console;
using ceh::ui::Style;
using ceh::ui::Color;
using ceh::ui::Text;

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
    // ---- 颜色启用判定（纯逻辑）----
    CHECK_TRUE(ceh::ui::resolve_color_enabled(true, false));    // TTY 且无 NO_COLOR
    CHECK_TRUE(!ceh::ui::resolve_color_enabled(true, true));    // NO_COLOR 覆盖
    CHECK_TRUE(!ceh::ui::resolve_color_enabled(false, false));  // 非 TTY
    CHECK_TRUE(!ceh::ui::resolve_color_enabled(false, true));

    // ---- 启用颜色：输出带 ANSI ----
    {
        std::ostringstream os;
        Console con(os, true);
        CHECK_TRUE(con.color_enabled());
        con.print("x", Style().fg(Color::red()));
        CHECK_EQ(os.str(), "\x1b[31mx\x1b[0m");
    }

    // ---- 关闭颜色：退化为纯文本 ----
    {
        std::ostringstream os;
        Console con(os, false);
        con.print("x", Style().fg(Color::red()).bold());
        CHECK_EQ(os.str(), "x");
    }

    // ---- println 追加换行 ----
    {
        std::ostringstream os;
        Console con(os, false);
        con.println("line");
        CHECK_EQ(os.str(), "line\n");
    }
    {
        std::ostringstream os;
        Console con(os, true);
        con.println("warn", Style().fg(Color::yellow()));
        CHECK_EQ(os.str(), "\x1b[33mwarn\x1b[0m\n");
    }

    // ---- 富文本 ----
    {
        std::ostringstream os;
        Console con(os, true);
        Text t;
        t.append("A", Style().bold());
        t.append("B");
        con.print(t);
        CHECK_EQ(os.str(), "\x1b[1mA\x1b[0mB");
    }
    {
        std::ostringstream os;
        Console con(os, false);
        Text t;
        t.append("A", Style().bold());
        t.append("B");
        con.print(t);
        CHECK_EQ(os.str(), "AB");
    }

    // ---- 纯文本 print 不加样式 ----
    {
        std::ostringstream os;
        Console con(os, true);
        con.print("plain");
        CHECK_EQ(os.str(), "plain");
    }

    // ---- 运行时切换颜色 ----
    {
        std::ostringstream os;
        Console con(os, false);
        con.set_color_enabled(true);
        con.print("y", Style().fg(Color::green()));
        CHECK_EQ(os.str(), "\x1b[32my\x1b[0m");
    }

    // ---- 终端探测：不崩溃、宽度为正 ----
    CHECK_TRUE(ceh::ui::terminal_width() > 0);
    CHECK_TRUE(ceh::ui::terminal_width(123) > 0);
    (void)ceh::ui::stdout_is_terminal();   // 仅确保可调用（CI 下通常为 false）
    (void)ceh::ui::stderr_is_terminal();

    std::cout << "All Console tests passed (" << g_checks << " checks).\n";
    return 0;
}
