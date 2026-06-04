// ceh/types/safe_int.hpp
//
// SafeInt<T> —— 带溢出检测的有符号整数包装（header-only）。
//
// 把任意有符号整型 T（默认 long long）包装起来，在每次算术运算前做边界检查，
// 一旦会溢出/下溢就抛 std::overflow_error，而不是产生未定义行为或静默回绕。
//
// 检查方式参考 SEI CERT C 的 INT32-C 规则，纯标准 C++ 实现（不依赖编译器内建），
// 因此在 GCC / Clang / MSVC 上行为一致。
//
// 支持：构造、+ - * / %、一元负号、自增自减、复合赋值、比较、to_string、流输出。
//
// 异常：溢出/下溢抛 std::overflow_error；除以零抛 std::domain_error。
//
// 用法：
//     ceh::SafeInt<int> a = 2'000'000'000;
//     auto b = a + a;          // 抛 std::overflow_error
//     ceh::SafeInt<long long> c = 9'000'000'000LL;   // OK

#ifndef CEH_TYPES_SAFE_INT_HPP
#define CEH_TYPES_SAFE_INT_HPP

#include <limits>
#include <string>
#include <stdexcept>
#include <type_traits>
#include <ostream>
#include <istream>

namespace ceh {

template <typename T = long long>
class SafeInt {
    static_assert(std::is_integral<T>::value && std::is_signed<T>::value,
                  "ceh::SafeInt requires a signed integral type");

public:
    using value_type = T;

    // ---- 构造 ----
    SafeInt() = default;
    SafeInt(T v) : v_(v) {}

    // ---- 访问 ----
    T value() const { return v_; }
    explicit operator T() const { return v_; }

    // ---- 一元 ----
    SafeInt operator-() const {
        if (v_ == kMin)
            throw std::overflow_error("ceh::SafeInt: negation overflow");
        return SafeInt(static_cast<T>(-v_));
    }
    SafeInt operator+() const { return *this; }

    // ---- 四则（运算前检查边界）----
    SafeInt operator+(const SafeInt& o) const {
        const T a = v_, b = o.v_;
        if (b > 0 && a > static_cast<T>(kMax - b))
            throw std::overflow_error("ceh::SafeInt: addition overflow");
        if (b < 0 && a < static_cast<T>(kMin - b))
            throw std::overflow_error("ceh::SafeInt: addition underflow");
        return SafeInt(static_cast<T>(a + b));
    }

    SafeInt operator-(const SafeInt& o) const {
        const T a = v_, b = o.v_;
        if (b < 0 && a > static_cast<T>(kMax + b))
            throw std::overflow_error("ceh::SafeInt: subtraction overflow");
        if (b > 0 && a < static_cast<T>(kMin + b))
            throw std::overflow_error("ceh::SafeInt: subtraction underflow");
        return SafeInt(static_cast<T>(a - b));
    }

    SafeInt operator*(const SafeInt& o) const {
        const T a = v_, b = o.v_;
        if (a == 0 || b == 0) return SafeInt(0);
        // 按 a、b 正负分四种象限，用除法（而非乘法）预判 a*b 是否越过 kMax/kMin，
        // 从而在真正相乘之前就发现溢出（参考 CERT INT32-C）。
        bool bad;
        if (a > 0) {
            bad = (b > 0) ? (a > kMax / b) : (b < kMin / a);
        } else {
            bad = (b > 0) ? (a < kMin / b) : (b < kMax / a);
        }
        if (bad)
            throw std::overflow_error("ceh::SafeInt: multiplication overflow");
        return SafeInt(static_cast<T>(a * b));
    }

    SafeInt operator/(const SafeInt& o) const {
        if (o.v_ == 0)
            throw std::domain_error("ceh::SafeInt: division by zero");
        if (v_ == kMin && o.v_ == static_cast<T>(-1))
            throw std::overflow_error("ceh::SafeInt: division overflow");
        return SafeInt(static_cast<T>(v_ / o.v_));
    }

    SafeInt operator%(const SafeInt& o) const {
        if (o.v_ == 0)
            throw std::domain_error("ceh::SafeInt: modulo by zero");
        if (v_ == kMin && o.v_ == static_cast<T>(-1))
            return SafeInt(0);          // 数学结果为 0，规避 INT_MIN % -1 的 UB
        return SafeInt(static_cast<T>(v_ % o.v_));
    }

    // ---- 复合赋值 ----
    SafeInt& operator+=(const SafeInt& o) { return *this = *this + o; }
    SafeInt& operator-=(const SafeInt& o) { return *this = *this - o; }
    SafeInt& operator*=(const SafeInt& o) { return *this = *this * o; }
    SafeInt& operator/=(const SafeInt& o) { return *this = *this / o; }
    SafeInt& operator%=(const SafeInt& o) { return *this = *this % o; }

    // ---- 自增自减 ----
    SafeInt& operator++() { return *this = *this + SafeInt(1); }
    SafeInt& operator--() { return *this = *this - SafeInt(1); }
    SafeInt operator++(int) { SafeInt t = *this; ++*this; return t; }
    SafeInt operator--(int) { SafeInt t = *this; --*this; return t; }

    // ---- 比较 ----
    bool operator==(const SafeInt& o) const { return v_ == o.v_; }
    bool operator!=(const SafeInt& o) const { return v_ != o.v_; }
    bool operator<(const SafeInt& o)  const { return v_ <  o.v_; }
    bool operator<=(const SafeInt& o) const { return v_ <= o.v_; }
    bool operator>(const SafeInt& o)  const { return v_ >  o.v_; }
    bool operator>=(const SafeInt& o) const { return v_ >= o.v_; }

    // ---- 输出 ----
    std::string to_string() const { return std::to_string(v_); }

    friend std::ostream& operator<<(std::ostream& os, const SafeInt& s) {
        return os << s.v_;
    }
    friend std::istream& operator>>(std::istream& is, SafeInt& s) {
        return is >> s.v_;
    }

private:
    static constexpr T kMax = std::numeric_limits<T>::max();
    static constexpr T kMin = std::numeric_limits<T>::min();

    T v_ = 0;
};

}  // namespace ceh

#endif  // CEH_TYPES_SAFE_INT_HPP
