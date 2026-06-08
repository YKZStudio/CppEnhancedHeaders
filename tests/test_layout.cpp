// Layout 单元测试（零依赖，基于 assert）
#include <ceh/ui/layout.hpp>

#include <iostream>
#include <string>
#include <vector>

using namespace ceh::ui;

static int g_checks = 0;

#define CHECK_EQ(expr, expected)                                       \
    do { ++g_checks; std::string got = (expr); std::string exp = (expected); \
        if (got != exp) { std::cerr << "FAIL [" << __LINE__ << "]\n--got--\n" \
            << got << "\n--exp--\n" << exp << "\n"; std::exit(1);} } while (0)

#define CHECK_TRUE(expr)                                               \
    do { ++g_checks; if (!(expr)) {                                    \
        std::cerr << "FAIL [" << __LINE__ << "] " #expr "\n"; std::exit(1);} } while (0)

int main() {
    // 横向并排，不同高度补齐，间隔 1 列
    CHECK_EQ(hbox({"a\nbb", "X\nY\nZ"}, 1),
             "a  X\n"
             "bb Y\n"
             "   Z");

    // 单块原样；空输入为空
    CHECK_EQ(hbox({"only"}), "only");
    CHECK_EQ(hbox({}), "");

    // 纵向堆叠
    CHECK_EQ(vbox({"a", "b\nc"}), "a\nb\nc");

    // 跳过 ANSI 计算可见宽度
    CHECK_TRUE(detail::visual_width("\x1b[31mab\x1b[0m") == 2);
    CHECK_TRUE(detail::visual_width("中\x1b[1m文\x1b[0m") == 4);
    CHECK_TRUE(detail::visual_width("plain") == 5);

    // 带颜色的块也能正确对齐：第一块第一行可见宽 1，应补到块宽 2
    CHECK_EQ(hbox({"\x1b[31mX\x1b[0m\nYY", "Z"}, 0),
             "\x1b[31mX\x1b[0m Z\n"
             "YY ");

    std::cout << "All Layout tests passed (" << g_checks << " checks).\n";
    return 0;
}
