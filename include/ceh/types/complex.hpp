// ceh/types/complex.hpp
//
// Complex<T> —— 增强版复数（header-only），默认 T = double。
//
// 与 std::complex<T> 可互相转换，但提供更友好的字符串输出（"a+bi" 形式）、
// 整数幂、极坐标构造等便捷接口。
//
// 支持：与复数 / 标量的 + - * /、conj（共轭）、norm（模平方）、abs（模）、
//        arg（辐角）、polar（极坐标构造）、pow（整数幂，含负幂）、
//        real/imag 访问、与 std::complex 互转、to_string、流输出。
//
// 用法：
//     ceh::Complex<double> z(3, 4);
//     std::cout << z.abs() << '\n';            // 5
//     std::cout << z * ceh::Complex<double>(0, 1) << '\n';   // -4+3i
//     std::complex<double> s = z;              // 隐式转回标准库

#ifndef CEH_TYPES_COMPLEX_HPP
#define CEH_TYPES_COMPLEX_HPP

#include <cmath>
#include <complex>
#include <string>
#include <sstream>
#include <ostream>

namespace ceh {

template <typename T = double>
class Complex {
public:
    using value_type = T;

    // ---- 构造 ----
    Complex() = default;
    Complex(T re, T im = T(0)) : re_(re), im_(im) {}

    // 与 std::complex 互操作
    Complex(const std::complex<T>& z) : re_(z.real()), im_(z.imag()) {}
    operator std::complex<T>() const { return std::complex<T>(re_, im_); }
    std::complex<T> to_std() const { return std::complex<T>(re_, im_); }

    // 极坐标构造：r·e^{iθ}
    static Complex polar(T r, T theta) {
        return Complex(r * std::cos(theta), r * std::sin(theta));
    }

    // ---- 访问 ----
    T real() const { return re_; }
    T imag() const { return im_; }
    void set_real(T re) { re_ = re; }
    void set_imag(T im) { im_ = im; }

    // ---- 复数性质 ----
    Complex conj() const { return Complex(re_, static_cast<T>(-im_)); }
    T norm() const { return re_ * re_ + im_ * im_; }   // 模的平方
    T abs() const { return std::hypot(re_, im_); }     // 模
    T arg() const { return std::atan2(im_, re_); }     // 辐角

    // ---- 一元 ----
    Complex operator-() const { return Complex(static_cast<T>(-re_), static_cast<T>(-im_)); }
    Complex operator+() const { return *this; }

    // ---- 四则 ----
    Complex operator+(const Complex& o) const { return Complex(re_ + o.re_, im_ + o.im_); }
    Complex operator-(const Complex& o) const { return Complex(re_ - o.re_, im_ - o.im_); }
    Complex operator*(const Complex& o) const {
        return Complex(re_ * o.re_ - im_ * o.im_,
                       re_ * o.im_ + im_ * o.re_);
    }
    Complex operator/(const Complex& o) const {
        T d = o.norm();
        return Complex((re_ * o.re_ + im_ * o.im_) / d,
                       (im_ * o.re_ - re_ * o.im_) / d);
    }

    // ---- 复合赋值 ----
    Complex& operator+=(const Complex& o) { return *this = *this + o; }
    Complex& operator-=(const Complex& o) { return *this = *this - o; }
    Complex& operator*=(const Complex& o) { return *this = *this * o; }
    Complex& operator/=(const Complex& o) { return *this = *this / o; }

    // ---- 比较（精确相等）----
    bool operator==(const Complex& o) const { return re_ == o.re_ && im_ == o.im_; }
    bool operator!=(const Complex& o) const { return !(*this == o); }

    // ---- 整数幂（支持负幂：取倒数）----
    Complex pow(int exp) const {
        Complex base = *this;
        unsigned n;
        if (exp < 0) {
            base = Complex(1) / base;                      // 负幂：底数取倒数
            // |exp| 写成 -(exp+1)+1，避免 exp == INT_MIN 时 -exp 溢出
            n = static_cast<unsigned>(-(exp + 1)) + 1u;
        } else {
            n = static_cast<unsigned>(exp);
        }
        Complex result(1);
        while (n > 0) {
            if (n & 1u) result *= base;
            n >>= 1;
            if (n) base *= base;
        }
        return result;
    }

    // ---- 输出（"a+bi" 形式）----
    std::string to_string() const {
        std::ostringstream os;
        os << re_;
        if (im_ < T(0)) os << "-" << -im_ << "i";
        else            os << "+" << im_ << "i";
        return os.str();
    }

    friend std::ostream& operator<<(std::ostream& os, const Complex& z) {
        return os << z.to_string();
    }

private:
    T re_ = T(0);
    T im_ = T(0);
};

}  // namespace ceh

#endif  // CEH_TYPES_COMPLEX_HPP
