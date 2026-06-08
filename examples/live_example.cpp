// Live 使用示例：原地刷新进度条 + Spinner
#include <ceh/ui/live.hpp>
#include <ceh/ui/progress_bar.hpp>
#include <ceh/ui/spinner.hpp>

#include <chrono>
#include <thread>

int main() {
    using namespace ceh::ui;

    Live live;
    ProgressBar bar;
    bar.set_width(30).set_fill_style(Style().fg(Color::green()));
    Spinner sp = Spinner::dots();

    for (int i = 0; i <= 100; i += 5) {
        bar.set_progress(i / 100.0);
        live.update(sp.next() + " 下载中 " + bar.render());
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
    }
    live.update("✓ 下载完成");
    live.done();
    return 0;
}
