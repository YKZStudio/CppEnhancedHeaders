// Color / Style 使用示例（在支持 ANSI 的终端运行可看到彩色效果）
#include <ceh/ui/style.hpp>
#include <iostream>

int main() {
    using namespace ceh::ui;

    // 基础 16 色
    std::cout << "标准色: ";
    std::cout << colored("黑 ", Color::black()) << colored("红 ", Color::red())
              << colored("绿 ", Color::green()) << colored("黄 ", Color::yellow())
              << colored("蓝 ", Color::blue()) << colored("品红 ", Color::magenta())
              << colored("青 ", Color::cyan()) << "\n";

    // 文本属性
    std::cout << "属性  : " << bold("粗体 ") << italic("斜体 ")
              << underline("下划线 ")
              << Style().reverse().apply("反显") << "\n";

    // 组合样式：白字红底加粗
    std::cout << "组合  : "
              << Style().fg(Color::white()).bg(Color::red()).bold().apply(" 警告 ")
              << "\n";

    // 真彩色渐变
    std::cout << "真彩色: ";
    for (int i = 0; i < 24; ++i) {
        auto c = Color::rgb(static_cast<std::uint8_t>(255 - i * 10),
                            static_cast<std::uint8_t>(i * 10), 128);
        std::cout << Style().fg(c).apply("█");
    }
    std::cout << "\n";

    // 256 调色板
    std::cout << "256 色: ";
    for (int i = 196; i < 208; ++i)
        std::cout << Style().fg(Color::palette(static_cast<std::uint8_t>(i))).apply("●");
    std::cout << "\n";

    return 0;
}
