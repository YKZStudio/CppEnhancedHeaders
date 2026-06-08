// Live 单元测试（零依赖，基于 assert）
#include <ceh/ui/live.hpp>

#include <iostream>
#include <sstream>
#include <string>

using ceh::ui::Live;

static int g_checks = 0;

#define CHECK_EQ(expr, expected)                                       \
    do { ++g_checks; std::string got = (expr); std::string exp = (expected); \
        if (got != exp) { std::cerr << "FAIL [" << __LINE__ << "] got [" \
            << got << "] exp [" << exp << "]\n"; std::exit(1);} } while (0)

#define CHECK_TRUE(expr)                                               \
    do { ++g_checks; if (!(expr)) {                                    \
        std::cerr << "FAIL [" << __LINE__ << "] " #expr "\n"; std::exit(1);} } while (0)

int main() {
    // 纯函数：行数统计
    CHECK_TRUE(ceh::ui::detail::count_lines("") == 1);
    CHECK_TRUE(ceh::ui::detail::count_lines("a") == 1);
    CHECK_TRUE(ceh::ui::detail::count_lines("a\nb") == 2);
    CHECK_TRUE(ceh::ui::detail::count_lines("a\nb\n") == 3);

    // 纯函数：重绘前缀
    CHECK_EQ(ceh::ui::detail::redraw_prefix(0), "");
    CHECK_EQ(ceh::ui::detail::redraw_prefix(1), "\r\x1b[0J");
    CHECK_EQ(ceh::ui::detail::redraw_prefix(3), "\r\x1b[2A\x1b[0J");

    // 启用：首帧直接打印，后续帧带重绘前缀
    {
        std::ostringstream os;
        Live live(os, true);
        live.update("a\nb");                       // 首帧，2 行
        CHECK_EQ(os.str(), "a\nb");
        live.update("c");                          // 覆盖：上移 1 行 + 清屏 + 新内容
        CHECK_EQ(os.str(), "a\nb\r\x1b[1A\x1b[0Jc");
        live.done();                               // 收尾补换行
        CHECK_EQ(os.str(), "a\nb\r\x1b[1A\x1b[0Jc\n");
    }

    // 禁用：每帧另起一行，无控制序列
    {
        std::ostringstream os;
        Live live(os, false);
        CHECK_TRUE(!live.enabled());
        live.update("x");
        live.update("y");
        CHECK_EQ(os.str(), "x\ny\n");
    }

    std::cout << "All Live tests passed (" << g_checks << " checks).\n";
    return 0;
}
