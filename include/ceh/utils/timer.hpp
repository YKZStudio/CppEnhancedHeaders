// ceh/utils/timer.hpp
//
// Timer —— 轻量计时器 / 性能测量（header-only）。
//
// 基于 std::chrono::steady_clock（单调时钟，不受系统时间调整影响）。
// 支持 start / stop / reset，stop 后再 start 会在原有基础上继续累加，
// 适合分段计时。另提供 ScopedTimer（析构时回调耗时）与静态 measure_ms。
//
// 用法：
//     ceh::Timer t;                       // 构造即开始计时
//     // ... 做一些事 ...
//     double ms = t.elapsed_ms();
//
//     double cost = ceh::Timer::measure_ms([]{ heavy_work(); });

#ifndef CEH_UTILS_TIMER_HPP
#define CEH_UTILS_TIMER_HPP

#include <chrono>
#include <utility>

namespace ceh {

class Timer {
public:
    using clock = std::chrono::steady_clock;

    // 默认构造即开始计时；传 false 则创建后处于停止状态
    explicit Timer(bool start_now = true) {
        if (start_now) start();
    }

    void start() {
        if (!running_) {
            running_ = true;
            start_ = clock::now();
        }
    }

    void stop() {
        if (running_) {
            accumulated_ += clock::now() - start_;
            running_ = false;
        }
    }

    void reset() {
        accumulated_ = clock::duration::zero();
        running_ = false;
    }

    // 复位并立即重新开始计时
    void restart() {
        reset();
        start();
    }

    bool is_running() const { return running_; }

    // 已累计的时长（含当前正在运行的一段）
    clock::duration elapsed() const {
        clock::duration total = accumulated_;
        if (running_) total += clock::now() - start_;
        return total;
    }

    double elapsed_seconds() const { return to<std::chrono::duration<double>>(); }
    double elapsed_ms() const { return to<std::chrono::duration<double, std::milli>>(); }
    double elapsed_us() const { return to<std::chrono::duration<double, std::micro>>(); }
    long long elapsed_ns() const {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed()).count();
    }

    // 执行一次 fn 并返回其耗时（毫秒）
    template <typename Fn>
    static double measure_ms(Fn&& fn) {
        Timer t;
        std::forward<Fn>(fn)();
        t.stop();
        return t.elapsed_ms();
    }

private:
    template <typename Duration>
    double to() const {
        return std::chrono::duration_cast<Duration>(elapsed()).count();
    }

    clock::time_point start_{};
    clock::duration accumulated_{clock::duration::zero()};
    bool running_ = false;
};

// 作用域计时器：析构时把耗时（毫秒）交给回调
template <typename Callback>
class ScopedTimer {
public:
    explicit ScopedTimer(Callback cb) : cb_(std::move(cb)) {}
    ~ScopedTimer() { cb_(timer_.elapsed_ms()); }

    ScopedTimer(const ScopedTimer&) = delete;
    ScopedTimer& operator=(const ScopedTimer&) = delete;

private:
    Timer timer_;
    Callback cb_;
};

template <typename Callback>
ScopedTimer<Callback> make_scoped_timer(Callback cb) {
    return ScopedTimer<Callback>(std::move(cb));
}

}  // namespace ceh

#endif  // CEH_UTILS_TIMER_HPP
