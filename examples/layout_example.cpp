// Layout 使用示例：把两个面板并排
#include <ceh/ui/layout.hpp>
#include <ceh/ui/panel.hpp>
#include <iostream>

int main() {
    using namespace ceh::ui;

    std::string left = Panel("CPU  42%\nMEM  61%")
                           .set_title("系统")
                           .set_border(BorderStyle::Rounded)
                           .render(false);
    std::string right = Panel("ok: 128\nerr: 3")
                            .set_title("任务")
                            .set_border(BorderStyle::Rounded)
                            .render(false);

    std::cout << hbox({left, right}, 2) << "\n\n";

    std::cout << vbox({"--- 上 ---", "--- 下 ---"}) << "\n";
    return 0;
}
