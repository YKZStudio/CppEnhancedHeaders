// Timer 使用示例
#include <ceh/utils/timer.hpp>

#include <chrono>
#include <iostream>
#include <thread>

int main() {
    using ceh::Timer;

    Timer t;
    long long sum = 0;
    for (long long i = 0; i < 50000000; ++i) sum += i;
    t.stop();
    std::cout << "累加耗时: " << t.elapsed_ms() << " ms (sum=" << sum << ")\n";

    // 一次性测量
    double cost = Timer::measure_ms([] {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    });
    std::cout << "sleep(20ms) 实测: " << cost << " ms\n";

    // 作用域计时器：离开作用域自动打印
    {
        auto st = ceh::make_scoped_timer(
            [](double ms) { std::cout << "作用域耗时: " << ms << " ms\n"; });
        (void)st;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return 0;
}
