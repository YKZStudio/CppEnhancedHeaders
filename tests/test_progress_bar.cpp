// ProgressBar 单元测试（零依赖，基于 assert）
#include <ceh/ui/progress_bar.hpp>

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
    ProgressBar bar;
    bar.set_chars("#", "-").set_width(10);

    bar.set_progress(0.0);
    CHECK_EQ(bar.render(false), "---------- 0%");
    bar.set_progress(0.3);
    CHECK_EQ(bar.render(false), "###------- 30%");
    bar.set_progress(0.5);
    CHECK_EQ(bar.render(false), "#####----- 50%");
    bar.set_progress(1.0);
    CHECK_EQ(bar.render(false), "########## 100%");

    // 关闭百分比
    bar.set_progress(0.5).set_show_percent(false);
    CHECK_EQ(bar.render(false), "#####-----");
    bar.set_show_percent(true);

    // fraction
    bar.set_fraction(1, 4);
    CHECK_TRUE(bar.percent() == 25);
    bar.set_fraction(2, 2);
    CHECK_TRUE(bar.percent() == 100);
    bar.set_fraction(5, 0);                 // total 0 -> 视为 0
    CHECK_TRUE(bar.percent() == 0);

    // 越界夹取
    bar.set_progress(2.0);
    CHECK_TRUE(bar.percent() == 100);
    bar.set_progress(-1.0);
    CHECK_TRUE(bar.percent() == 0);

    // 带样式：填充段被着色，空白段无样式
    bar.set_progress(0.5).set_fill_style(Style().fg(Color::red()));
    CHECK_EQ(bar.render(true), "\x1b[31m#####\x1b[0m----- 50%");

    std::cout << "All ProgressBar tests passed (" << g_checks << " checks).\n";
    return 0;
}
