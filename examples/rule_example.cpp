// Rule 使用示例：分隔线
#include <ceh/ui/rule.hpp>
#include <iostream>

int main() {
    using namespace ceh::ui;

    const std::size_t W = 40;

    std::cout << horizontal_rule(W) << "\n";
    std::cout << titled_rule("配置", W) << "\n";
    std::cout << "  key = value\n";
    std::cout << titled_rule("日志", W, "─", Style().fg(Color::cyan()).bold()) << "\n";
    std::cout << "  [INFO] started\n";
    std::cout << horizontal_rule(W, Style().fg(Color::bright_black()), "═") << "\n";

    return 0;
}
