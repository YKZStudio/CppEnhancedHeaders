// Timer 单元测试（零依赖，基于 assert）
#include <ceh/utils/timer.hpp>

#include <chrono>
#include <iostream>
#include <thread>

using ceh::Timer;

static int g_checks = 0;

#define CHECK_TRUE(expr)                                               \
    do {                                                               \
        ++g_checks;                                                    \
        if (!(expr)) {                                                 \
            std::cerr << "FAIL [" << __LINE__ << "] " #expr "\n";      \
            std::exit(1);                                              \
        }                                                              \
    } while (0)

static void sleep_ms(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

int main() {
    // 构造即运行；计时单调不减
    {
        Timer t;
        CHECK_TRUE(t.is_running());
        sleep_ms(15);
        double a = t.elapsed_ms();
        CHECK_TRUE(a > 0.0);
        sleep_ms(5);
        CHECK_TRUE(t.elapsed_ms() >= a);   // 不减小
    }

    // 默认不启动
    {
        Timer t(false);
        CHECK_TRUE(!t.is_running());
        CHECK_TRUE(t.elapsed_ns() == 0);
    }

    // stop 后读数稳定，再 start 在原基础上累加
    {
        Timer t;
        sleep_ms(10);
        t.stop();
        CHECK_TRUE(!t.is_running());
        double after_stop = t.elapsed_ms();
        sleep_ms(10);
        CHECK_TRUE(t.elapsed_ms() == after_stop);   // 停止期间不变
        t.start();                                   // 继续累加
        CHECK_TRUE(t.is_running());
        sleep_ms(10);
        CHECK_TRUE(t.elapsed_ms() > after_stop);
    }

    // reset / restart
    {
        Timer t;
        sleep_ms(10);
        t.reset();
        CHECK_TRUE(!t.is_running());
        CHECK_TRUE(t.elapsed_ns() == 0);
        t.restart();
        CHECK_TRUE(t.is_running());
    }

    // 单位换算关系（粗略）：ms 约等于 ns/1e6
    {
        Timer t;
        sleep_ms(12);
        t.stop();
        CHECK_TRUE(t.elapsed_seconds() > 0.0);
        CHECK_TRUE(t.elapsed_us() > t.elapsed_ms());      // 微秒数 > 毫秒数
        CHECK_TRUE(t.elapsed_ns() > 0);
    }

    // measure_ms：测量一段 sleep
    {
        double cost = Timer::measure_ms([] { sleep_ms(15); });
        CHECK_TRUE(cost > 0.0);
    }

    // ScopedTimer：析构时回调
    {
        double captured = -1.0;
        {
            auto st = ceh::make_scoped_timer([&captured](double ms) { captured = ms; });
            (void)st;
            sleep_ms(5);
        }
        CHECK_TRUE(captured >= 0.0);
    }

    std::cout << "All Timer tests passed (" << g_checks << " checks).\n";
    return 0;
}
