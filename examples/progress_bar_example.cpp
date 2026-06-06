// ProgressBar 使用示例（静态展示若干进度；实际可配合 '\r' 刷新）
#include <ceh/ui/progress_bar.hpp>
#include <iostream>

int main() {
    using namespace ceh::ui;

    ProgressBar bar;
    bar.set_width(30)
       .set_fill_style(Style().fg(Color::green()))
       .set_empty_style(Style().fg(Color::bright_black()));

    for (double p : {0.0, 0.25, 0.5, 0.75, 1.0}) {
        bar.set_progress(p);
        std::cout << "下载 " << bar.render() << "\n";
    }
    return 0;
}
