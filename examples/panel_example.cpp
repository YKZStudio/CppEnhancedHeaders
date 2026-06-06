// Panel 使用示例
#include <ceh/ui/panel.hpp>
#include <iostream>

int main() {
    using namespace ceh::ui;

    Panel p("项目构建成功 ✓\n用时 1.2s，输出 build/app");
    p.set_title("结果")
     .set_border(BorderStyle::Rounded)
     .set_border_style(Style().fg(Color::green()))
     .set_title_style(Style().bold());

    std::cout << p.render() << "\n";
    return 0;
}
