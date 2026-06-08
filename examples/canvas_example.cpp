// Canvas 使用示例：字符画布与盲文折线图
#include <ceh/ui/canvas.hpp>
#include <cmath>
#include <iostream>

int main() {
    using namespace ceh::ui;

    // 字符画布：画框 + 对角线 + 文本
    Canvas c(20, 7);
    c.draw_rect(0, 0, 20, 7);
    c.draw_line(1, 1, 18, 5, "*");
    c.set(9, 3, "O");
    std::cout << c.render() << "\n\n";

    // 盲文画布：画一条正弦曲线
    BrailleCanvas b(80, 24);
    for (int px = 0; px < 80; ++px) {
        double y = 12 + 10 * std::sin(px * 0.2);
        b.plot(px, static_cast<int>(y));
    }
    std::cout << b.render() << "\n";
    return 0;
}
