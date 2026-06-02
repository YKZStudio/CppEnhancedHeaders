// ceh/types/fraction.hpp
//
// Fraction —— 任意精度有理数（header-only）。
//
// 基于 ceh::BigInt 实现，分子/分母均为任意精度整数。
//
// 不变式（始终维持）：
//   - 分母 den_ > 0。
//   - gcd(|num_|, den_) == 1（始终最简）。
//   - 零统一表示为 0/1。
//
// 支持：整型 / BigInt / 字符串（"3/4"、"-5"）构造、+ - * / 与复合赋值、
//        一元负号、比较、reciprocal（倒数）、pow（含负指数）、abs、sign、
//        to_string、to_double、流输出。
//
// 用法：
//     ceh::Fraction a(1, 3), b(1, 6);
//     std::cout << (a + b) << '\n';          // 1/2
//     std::cout << ceh::Fraction(2, 4) << '\n';  // 1/2（自动约分）
//     std::cout << ceh::Fraction("22/7").to_double() << '\n';

#ifndef CEH_TYPES_FRACTION_HPP
#define CEH_TYPES_FRACTION_HPP

#include <ceh/types/big_int.hpp>

#include <string>
#include <stdexcept>
#include <type_traits>
#include <ostream>

namespace ceh {

class Fraction {
public:
    // ---- 构造 ----
    Fraction() : num_(0), den_(1) {}

    // 任意整型（模板约束，规避字面量 0 与 const char* 构造的歧义）
    template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
    Fraction(T v) : num_(v), den_(1) {}

    Fraction(const BigInt& v) : num_(v), den_(1) {}

    // 分子 / 分母
    Fraction(const BigInt& num, const BigInt& den) : num_(num), den_(den) {
        normalize();
    }

    Fraction(const std::string& s) { parse(s); }
    Fraction(const char* s) { parse(std::string(s)); }

    // ---- 访问 ----
    const BigInt& numerator() const { return num_; }
    const BigInt& denominator() const { return den_; }

    bool is_zero() const { return num_.is_zero(); }
    int sign() const { return num_.sign(); }          // 分母恒正，符号由分子决定
    bool is_integer() const { return den_ == BigInt(1); }

    Fraction abs() const {
        Fraction r = *this;
        if (r.num_.sign() < 0) r.num_ = -r.num_;
        return r;
    }

    // 倒数；对 0 抛异常
    Fraction reciprocal() const {
        if (num_.is_zero())
            throw std::domain_error("ceh::Fraction: reciprocal of zero");
        Fraction r;
        if (num_.sign() < 0) { r.num_ = -den_; r.den_ = -num_; }
        else                 { r.num_ = den_;  r.den_ = num_; }
        return r;   // 已最简、分母为正，无需再 normalize
    }

    // ---- 一元 ----
    Fraction operator-() const {
        Fraction r = *this;
        if (!r.num_.is_zero()) r.num_ = -r.num_;
        return r;
    }
    Fraction operator+() const { return *this; }

    // ---- 四则 ----
    Fraction operator+(const Fraction& o) const {
        return Fraction(num_ * o.den_ + o.num_ * den_, den_ * o.den_);
    }
    Fraction operator-(const Fraction& o) const {
        return Fraction(num_ * o.den_ - o.num_ * den_, den_ * o.den_);
    }
    Fraction operator*(const Fraction& o) const {
        return Fraction(num_ * o.num_, den_ * o.den_);
    }
    Fraction operator/(const Fraction& o) const {
        if (o.num_.is_zero())
            throw std::domain_error("ceh::Fraction: division by zero");
        return Fraction(num_ * o.den_, den_ * o.num_);
    }

    // ---- 复合赋值 ----
    Fraction& operator+=(const Fraction& o) { return *this = *this + o; }
    Fraction& operator-=(const Fraction& o) { return *this = *this - o; }
    Fraction& operator*=(const Fraction& o) { return *this = *this * o; }
    Fraction& operator/=(const Fraction& o) { return *this = *this / o; }

    // ---- 比较（交叉相乘，分母恒正）----
    int compare(const Fraction& o) const {
        return (num_ * o.den_).compare(o.num_ * den_);
    }
    bool operator==(const Fraction& o) const { return compare(o) == 0; }
    bool operator!=(const Fraction& o) const { return compare(o) != 0; }
    bool operator<(const Fraction& o)  const { return compare(o) <  0; }
    bool operator<=(const Fraction& o) const { return compare(o) <= 0; }
    bool operator>(const Fraction& o)  const { return compare(o) >  0; }
    bool operator>=(const Fraction& o) const { return compare(o) >= 0; }

    // ---- 幂（支持负指数：底数取倒数）----
    Fraction pow(long long exp) const {
        Fraction base = *this;
        unsigned long long e;
        if (exp < 0) {
            base = reciprocal();                 // 0 的负幂会在此抛 domain_error
            e = 0ull - static_cast<unsigned long long>(exp);
        } else {
            e = static_cast<unsigned long long>(exp);
        }
        Fraction result(1);
        while (e > 0) {
            if (e & 1ull) result *= base;
            e >>= 1;
            if (e) base *= base;
        }
        return result;
    }

    // ---- 转换 / 输出 ----
    std::string to_string() const {
        if (den_ == BigInt(1)) return num_.to_string();
        return num_.to_string() + "/" + den_.to_string();
    }

    // 近似 double；超出 double 范围时返回 ±inf
    double to_double() const {
        return big_to_double(num_) / big_to_double(den_);
    }

    friend std::ostream& operator<<(std::ostream& os, const Fraction& v) {
        return os << v.to_string();
    }

private:
    BigInt num_;   // 分子（携带符号）
    BigInt den_;   // 分母（恒为正）

    static BigInt gcd(BigInt a, BigInt b) {
        a = a.abs();
        b = b.abs();
        while (!b.is_zero()) {
            BigInt t = a % b;
            a = b;
            b = t;
        }
        return a;
    }

    // 维持不变式：分母转正、约分到最简、零规约为 0/1
    void normalize() {
        if (den_.is_zero())
            throw std::domain_error("ceh::Fraction: zero denominator");
        if (den_.sign() < 0) { num_ = -num_; den_ = -den_; }
        if (num_.is_zero()) { den_ = BigInt(1); return; }
        BigInt g = gcd(num_, den_);
        if (!(g == BigInt(1))) { num_ = num_ / g; den_ = den_ / g; }
    }

    // 解析 "num/den" 或 "num"
    void parse(const std::string& s) {
        auto slash = s.find('/');
        if (slash == std::string::npos) {
            num_ = BigInt(s);
            den_ = BigInt(1);
        } else {
            num_ = BigInt(s.substr(0, slash));
            den_ = BigInt(s.substr(slash + 1));
        }
        normalize();
    }

    // BigInt -> double（按十进制字符串逐位累加，避免依赖 stod 的异常）
    static double big_to_double(const BigInt& v) {
        std::string s = v.abs().to_string();
        double d = 0.0;
        for (char c : s) d = d * 10.0 + static_cast<double>(c - '0');
        return v.sign() < 0 ? -d : d;
    }
};

}  // namespace ceh

#endif  // CEH_TYPES_FRACTION_HPP
