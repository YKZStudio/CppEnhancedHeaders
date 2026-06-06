// Spinner 单元测试（零依赖，基于 assert）
#include <ceh/ui/spinner.hpp>

#include <iostream>
#include <string>
#include <vector>

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
    // 自定义帧：current 不前进，next 前进并回卷
    {
        Spinner sp(std::vector<std::string>{"a", "b", "c"});
        CHECK_TRUE(sp.frame_count() == 3);
        CHECK_EQ(sp.current(false), "a");
        CHECK_EQ(sp.current(false), "a");     // 仍是 a
        CHECK_EQ(sp.next(false), "b");
        CHECK_EQ(sp.next(false), "c");
        CHECK_EQ(sp.next(false), "a");         // 回卷
        CHECK_TRUE(sp.index() == 0);
    }

    // reset
    {
        Spinner sp(std::vector<std::string>{"x", "y"});
        sp.next(false);
        CHECK_TRUE(sp.index() == 1);
        sp.reset();
        CHECK_TRUE(sp.index() == 0);
        CHECK_EQ(sp.current(false), "x");
    }

    // 预设帧集非空
    CHECK_TRUE(Spinner::dots().frame_count() == 10);
    CHECK_TRUE(Spinner::line().frame_count() == 4);
    CHECK_TRUE(Spinner::arrow().frame_count() == 8);
    CHECK_TRUE(Spinner::clock().frame_count() == 12);
    CHECK_EQ(Spinner::line().current(false), "-");

    // 空帧集容错（至少一帧）
    {
        Spinner sp(std::vector<std::string>{});
        CHECK_TRUE(sp.frame_count() == 1);
        CHECK_EQ(sp.current(false), "");
    }

    // 样式
    {
        Spinner sp(std::vector<std::string>{"x"}, Style().bold());
        CHECK_EQ(sp.current(true), "\x1b[1mx\x1b[0m");
        CHECK_EQ(sp.current(false), "x");
    }

    std::cout << "All Spinner tests passed (" << g_checks << " checks).\n";
    return 0;
}
