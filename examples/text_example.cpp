// Text 使用示例：富文本拼接与显示宽度
#include <ceh/ui/text.hpp>
#include <iostream>

int main() {
    using namespace ceh::ui;

    Text line;
    line.append("[", Style().fg(Color::bright_black()));
    line.append("OK", Style().fg(Color::green()).bold());
    line.append("] ", Style().fg(Color::bright_black()));
    line.append("构建成功");

    std::cout << line.render() << "\n";
    std::cout << "纯文本: " << line.plain() << "\n";
    std::cout << "显示列宽: " << line.width() << " 列\n";

    // 显示宽度对中英文混排的处理
    std::cout << "display_width(\"中文abc\") = "
              << display_width("中文abc") << "\n";   // 2+2+3 = 7

    return 0;
}
