// Spinner 使用示例（打印各预设的全部帧；实际可配合定时刷新）
#include <ceh/ui/spinner.hpp>
#include <iostream>

static void show(const char* name, ceh::ui::Spinner sp) {
    std::cout << name << ": ";
    for (std::size_t i = 0; i < sp.frame_count(); ++i) {
        std::cout << sp.current(false) << ' ';
        sp.next(false);
    }
    std::cout << "\n";
}

int main() {
    using namespace ceh::ui;
    show("dots ", Spinner::dots());
    show("line ", Spinner::line());
    show("arrow", Spinner::arrow());

    // 配合提示文字（带颜色）
    Spinner sp = Spinner::dots();
    sp.set_style(Style().fg(Color::cyan()));
    std::cout << sp.current() << " 加载中...\n";
    return 0;
}
