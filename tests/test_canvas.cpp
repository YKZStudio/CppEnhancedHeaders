// Canvas / BrailleCanvas 单元测试（零依赖，基于 assert）
#include <ceh/ui/canvas.hpp>

#include <iostream>
#include <string>

using ceh::ui::Canvas;
using ceh::ui::BrailleCanvas;

static int g_checks = 0;

#define CHECK_EQ(expr, expected)                                       \
    do { ++g_checks; std::string got = (expr); std::string exp = (expected); \
        if (got != exp) { std::cerr << "FAIL [" << __LINE__ << "]\n--got--\n" \
            << got << "\n--exp--\n" << exp << "\n"; std::exit(1);} } while (0)

#define CHECK_TRUE(expr)                                               \
    do { ++g_checks; if (!(expr)) {                                    \
        std::cerr << "FAIL [" << __LINE__ << "] " #expr "\n"; std::exit(1);} } while (0)

int main() {
    // 基本网格 + set/get
    {
        Canvas c(3, 2, ".");
        CHECK_TRUE(c.width() == 3 && c.height() == 2);
        CHECK_EQ(c.render(), "...\n...");
        c.set(0, 0, "#");
        c.set(2, 1, "@");
        CHECK_EQ(c.render(), "#..\n..@");
        CHECK_EQ(c.get(0, 0), "#");
        CHECK_EQ(c.get(2, 1), "@");
        // 越界静默忽略，get 返回空
        c.set(-1, -1, "x");
        c.set(99, 99, "x");
        CHECK_EQ(c.get(99, 99), "");
    }

    // 横竖线
    {
        Canvas c(4, 3, ".");
        c.draw_hline(0, 3, 0, "-");
        c.draw_vline(0, 2, 0, "|");   // 后画的竖线覆盖角点 (0,0)
        CHECK_EQ(c.render(), "|---\n|...\n|...");
    }

    // 矩形（制表符边框）
    {
        Canvas c(5, 3);
        c.draw_rect(0, 0, 5, 3);
        CHECK_EQ(c.render(),
                 "┌───┐\n"
                 "│   │\n"
                 "└───┘");
    }

    // Bresenham 直线
    {
        Canvas c(5, 3, ".");
        c.draw_line(0, 0, 4, 2, "*");
        CHECK_EQ(c.render(), "*....\n.**..\n...**");
    }

    // 填充矩形
    {
        Canvas c(4, 3, ".");
        c.fill_rect(1, 1, 2, 2, "#");
        CHECK_EQ(c.render(), "....\n.##.\n.##.");
    }

    // ---- BrailleCanvas ----
    {
        BrailleCanvas b(2, 4);                 // 恰好一个盲文字符
        CHECK_TRUE(b.pixel_width() == 2 && b.pixel_height() == 4);
        CHECK_EQ(b.render(), "⠀");        // 空盲文 ⠀
        b.plot(0, 0);
        CHECK_EQ(b.render(), "⠁");        // 左上点 ⠁
    }
    {
        BrailleCanvas b(2, 4);
        for (int x = 0; x < 2; ++x)
            for (int y = 0; y < 4; ++y) b.plot(x, y);
        CHECK_EQ(b.render(), "⣿");        // 八点全亮 ⣿
        b.clear();
        CHECK_EQ(b.render(), "⠀");
    }
    {
        // 尺寸向上取整到字符格：3x5 -> 2 列 2 行
        BrailleCanvas b(3, 5);
        CHECK_EQ(b.render(), "⠀⠀\n⠀⠀");
        b.plot(-1, -1);                        // 越界忽略，不崩溃
        b.plot(100, 100);
    }

    std::cout << "All Canvas tests passed (" << g_checks << " checks).\n";
    return 0;
}
