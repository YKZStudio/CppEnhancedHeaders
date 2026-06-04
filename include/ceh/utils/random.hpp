// ceh/utils/random.hpp
//
// Random —— 易用的随机数封装（header-only），底层用 std::mt19937_64。
//
// 把「选引擎 + 配分布」这套样板收进一个对象：给定种子可复现，提供闭区间整数、
// 半开区间实数、布尔、随机选取与洗牌等常用操作。
//
// 注意：跨标准库实现时，分布（uniform_int_distribution 等）的具体取值可能不同；
// 但「同一程序内、相同种子」一定可复现。需要严格跨平台一致的原始序列时用 next_u64()。
//
// 异常：next_int 的 lo > hi 抛 std::invalid_argument；choice 空容器抛 std::out_of_range。
//
// 用法：
//     ceh::Random rng(42);             // 固定种子，可复现
//     int d = rng.next_int(1, 6);      // 掷骰子 [1,6]
//     double u = rng.next_double();    // [0,1)
//     rng.shuffle(deck);

#ifndef CEH_UTILS_RANDOM_HPP
#define CEH_UTILS_RANDOM_HPP

#include <algorithm>
#include <cstdint>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace ceh {

class Random {
public:
    using engine_type = std::mt19937_64;

    // 用 random_device 播种（非确定）
    Random() : engine_(std::random_device{}()) {}

    // 用固定种子播种（可复现）
    explicit Random(std::uint64_t seed) : engine_(seed) {}

    void seed(std::uint64_t s) { engine_.seed(s); }

    // 闭区间 [lo, hi] 上的均匀整数
    template <typename T = int>
    T next_int(T lo, T hi) {
        static_assert(std::is_integral<T>::value, "ceh::Random::next_int requires integral T");
        if (lo > hi) throw std::invalid_argument("ceh::Random::next_int: lo > hi");
        std::uniform_int_distribution<T> dist(lo, hi);
        return dist(engine_);
    }

    // 半开区间 [lo, hi) 上的均匀实数
    double next_double(double lo = 0.0, double hi = 1.0) {
        std::uniform_real_distribution<double> dist(lo, hi);
        return dist(engine_);
    }

    // 以概率 p 返回 true
    bool next_bool(double p = 0.5) {
        std::bernoulli_distribution dist(p);
        return dist(engine_);
    }

    // 原始 64 位输出（跨实现确定）
    std::uint64_t next_u64() { return engine_(); }

    // 从非空容器中等概率取一个元素
    template <typename T>
    const T& choice(const std::vector<T>& v) {
        if (v.empty()) throw std::out_of_range("ceh::Random::choice: empty container");
        std::uniform_int_distribution<std::size_t> dist(0, v.size() - 1);
        return v[dist(engine_)];
    }

    // 原地洗牌（Fisher-Yates，由 std::shuffle 实现）
    template <typename RandomIt>
    void shuffle(RandomIt first, RandomIt last) {
        std::shuffle(first, last, engine_);
    }

    template <typename T>
    void shuffle(std::vector<T>& v) {
        std::shuffle(v.begin(), v.end(), engine_);
    }

    // 暴露底层引擎，便于配合标准分布使用
    engine_type& engine() { return engine_; }

private:
    engine_type engine_;
};

}  // namespace ceh

#endif  // CEH_UTILS_RANDOM_HPP
