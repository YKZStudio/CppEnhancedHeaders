// ceh/algorithms/math.hpp
//
// 常用数论算法（header-only，自由函数）。
//
// 提供：gcd / lcm、整数快速幂 ipow、模运算 add_mod / mul_mod / pow_mod、
//        素性判定 is_prime、埃氏筛 primes_up_to、扩展欧几里得 extended_gcd、
//        模逆元 mod_inverse。
//
// 可移植性：模乘用「二进制快速乘」实现，不依赖 128 位整数或编译器内建，
// 在 mod 接近 2^64 时也不会溢出；GCC / Clang / MSVC 行为一致。
//
// 异常：模为 0 抛 std::invalid_argument；模逆元不存在抛 std::invalid_argument。
//
// 用法：
//     ceh::gcd(12, 18);                  // 6
//     ceh::pow_mod(2, 100, 1000000007);  // 2^100 mod 1e9+7
//     auto ps = ceh::primes_up_to(30);   // {2,3,5,7,11,13,17,19,23,29}

#ifndef CEH_ALGORITHMS_MATH_HPP
#define CEH_ALGORITHMS_MATH_HPP

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace ceh {

namespace detail {
template <typename T>
constexpr T abs_val(T x) {
    if constexpr (std::is_signed<T>::value) {
        return x < 0 ? static_cast<T>(-x) : x;
    } else {
        return x;
    }
}
}  // namespace detail

// 最大公约数（欧几里得算法），结果非负
template <typename T>
T gcd(T a, T b) {
    static_assert(std::is_integral<T>::value, "ceh::gcd requires an integral type");
    a = detail::abs_val(a);
    b = detail::abs_val(b);
    while (b != 0) {
        T t = a % b;
        a = b;
        b = t;
    }
    return a;
}

// 最小公倍数；任一为 0 则结果为 0。先除后乘以减小溢出
template <typename T>
T lcm(T a, T b) {
    static_assert(std::is_integral<T>::value, "ceh::lcm requires an integral type");
    if (a == 0 || b == 0) return 0;
    T g = gcd(a, b);
    return detail::abs_val(static_cast<T>(a / g * b));
}

// 整数快速幂（不取模），exp 非负
template <typename T>
T ipow(T base, unsigned long long exp) {
    static_assert(std::is_integral<T>::value, "ceh::ipow requires an integral type");
    T result = 1;
    while (exp > 0) {
        if (exp & 1ull) result *= base;
        exp >>= 1;
        if (exp) base *= base;
    }
    return result;
}

// (a + b) mod m，避免相加溢出（要求 m > 0）
inline std::uint64_t add_mod(std::uint64_t a, std::uint64_t b, std::uint64_t mod) {
    a %= mod;
    b %= mod;
    // 若 a + b 会溢出/越界，等价转换为 a - (mod - b)
    if (a >= mod - b) return a - (mod - b);
    return a + b;
}

// (a * b) mod m，二进制快速乘，避免 64 位乘法溢出（要求 m > 0）
inline std::uint64_t mul_mod(std::uint64_t a, std::uint64_t b, std::uint64_t mod) {
    a %= mod;
    std::uint64_t result = 0;
    while (b > 0) {
        if (b & 1ull) result = add_mod(result, a, mod);
        a = add_mod(a, a, mod);
        b >>= 1;
    }
    return result;
}

// (base ^ exp) mod m（要求 m > 0，否则抛 std::invalid_argument）
inline std::uint64_t pow_mod(std::uint64_t base, std::uint64_t exp, std::uint64_t mod) {
    if (mod == 0) throw std::invalid_argument("ceh::pow_mod: modulus must be > 0");
    if (mod == 1) return 0;
    std::uint64_t result = 1;
    base %= mod;
    while (exp > 0) {
        if (exp & 1ull) result = mul_mod(result, base, mod);
        exp >>= 1;
        if (exp) base = mul_mod(base, base, mod);
    }
    return result;
}

// 素性判定（试除到 sqrt(n)，写成 i <= n/i 避免 i*i 溢出）
inline bool is_prime(std::uint64_t n) {
    if (n < 2) return false;
    if (n < 4) return true;          // 2, 3
    if (n % 2 == 0) return false;
    for (std::uint64_t i = 3; i <= n / i; i += 2) {
        if (n % i == 0) return false;
    }
    return true;
}

// 埃拉托色尼筛：返回所有 <= n 的素数（升序）
inline std::vector<std::size_t> primes_up_to(std::size_t n) {
    std::vector<std::size_t> primes;
    if (n < 2) return primes;
    std::vector<bool> composite(n + 1, false);
    for (std::size_t i = 2; i <= n; ++i) {
        if (!composite[i]) {
            primes.push_back(i);
            if (i <= n / i) {        // 仅当 i*i <= n 时才需要标记
                for (std::size_t j = i * i; j <= n; j += i) composite[j] = true;
            }
        }
    }
    return primes;
}

// 扩展欧几里得：返回 gcd(a,b)，并求出满足 a*x + b*y = gcd 的 x, y
template <typename T>
T extended_gcd(T a, T b, T& x, T& y) {
    static_assert(std::is_integral<T>::value && std::is_signed<T>::value,
                  "ceh::extended_gcd requires a signed integral type");
    if (b == 0) {
        x = 1;
        y = 0;
        return a;
    }
    // 递归解出 b·x1 + (a%b)·y1 = g，再回代（a%b = a - ⌊a/b⌋·b）得本层的 x、y
    T x1, y1;
    T g = extended_gcd(b, static_cast<T>(a % b), x1, y1);
    x = y1;
    y = static_cast<T>(x1 - (a / b) * y1);
    return g;
}

// 模逆元：求 a^{-1} (mod m)，要求 m > 0 且 gcd(a, m) == 1，否则抛 std::invalid_argument
inline std::int64_t mod_inverse(std::int64_t a, std::int64_t m) {
    if (m <= 0) throw std::invalid_argument("ceh::mod_inverse: modulus must be > 0");
    std::int64_t x, y;
    std::int64_t a_norm = ((a % m) + m) % m;
    std::int64_t g = extended_gcd<std::int64_t>(a_norm, m, x, y);
    if (g != 1) throw std::invalid_argument("ceh::mod_inverse: inverse does not exist");
    return ((x % m) + m) % m;
}

}  // namespace ceh

#endif  // CEH_ALGORITHMS_MATH_HPP
