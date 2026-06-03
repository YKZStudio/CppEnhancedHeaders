// ceh/types/big_decimal.hpp
//
// BigDecimal —— 任意精度十进制小数（header-only），基于 ceh::BigInt 实现。
//
// 表示方式：无标度整数 unscaled_ 配一个非负 scale_，满足
//     value = unscaled_ × 10^(-scale_)
// 与 Java BigDecimal / Python Decimal 同思路。例如 unscaled=12345、scale=2
// 表示 123.45；unscaled=5、scale=3 表示 0.005。
//
// 不变式：scale_ >= 0。加减按 scale 对齐，乘法 scale 相加，除法需指定目标
// scale 与舍入模式（结果可能无法精确表示）。相等按「数值」判断，因此
// 1.0 == 1.00。
//
// 支持：整型 / BigInt / 字符串（含科学计数法 "1.5e-3"）构造、+ - * 与
//        divide(o, scale, mode) / operator/、比较、set_scale、
//        stripped_trailing_zeros、pow、to_string、流输出。
//
// 异常：除以零抛 std::domain_error；非法字符串抛 std::invalid_argument。
//
// 用法：
//     ceh::BigDecimal a("0.1"), b("0.2");
//     std::cout << (a + b) << '\n';                       // 0.3（精确，无浮点误差）
//     std::cout << ceh::BigDecimal(1).divide(ceh::BigDecimal(3), 10) << '\n';
//     // 0.3333333333

#ifndef CEH_TYPES_BIG_DECIMAL_HPP
#define CEH_TYPES_BIG_DECIMAL_HPP

#include <ceh/types/big_int.hpp>

#include <algorithm>
#include <string>
#include <stdexcept>
#include <type_traits>
#include <ostream>

namespace ceh {

// 舍入模式（用于除法与 set_scale 降低精度时）
enum class RoundingMode {
    HalfUp,    // 四舍五入：|余数|×2 >= |除数| 则远离零进位（默认）
    Down,      // 向零截断（直接丢弃多余位）
    Floor,     // 向负无穷取整
    Ceiling    // 向正无穷取整
};

class BigDecimal {
public:
    // operator/ 缺省至少保留的小数位（实际取输入 scale 与此值的较大者）
    static constexpr int kDefaultDivScale = 20;

    // ---- 构造 ----
    BigDecimal() : unscaled_(0), scale_(0) {}

    template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
    BigDecimal(T v) : unscaled_(v), scale_(0) {}

    BigDecimal(const BigInt& v) : unscaled_(v), scale_(0) {}

    // 无标度整数 + scale 直接构造
    BigDecimal(const BigInt& unscaled, int scale) : unscaled_(unscaled), scale_(scale) {
        if (scale_ < 0)
            throw std::invalid_argument("ceh::BigDecimal: negative scale");
    }

    BigDecimal(const std::string& s) { parse(s); }
    BigDecimal(const char* s) { parse(std::string(s)); }

    // ---- 访问 ----
    int scale() const { return scale_; }
    const BigInt& unscaled_value() const { return unscaled_; }
    bool is_zero() const { return unscaled_.is_zero(); }
    int sign() const { return unscaled_.sign(); }

    BigDecimal abs() const { return BigDecimal(unscaled_.abs(), scale_); }

    // ---- 一元 ----
    BigDecimal operator-() const { return BigDecimal(-unscaled_, scale_); }
    BigDecimal operator+() const { return *this; }

    // ---- 加减（对齐 scale）----
    BigDecimal operator+(const BigDecimal& o) const {
        int s = std::max(scale_, o.scale_);
        return BigDecimal(rescaled_unscaled(s) + o.rescaled_unscaled(s), s);
    }
    BigDecimal operator-(const BigDecimal& o) const {
        int s = std::max(scale_, o.scale_);
        return BigDecimal(rescaled_unscaled(s) - o.rescaled_unscaled(s), s);
    }

    // ---- 乘法（scale 相加）----
    BigDecimal operator*(const BigDecimal& o) const {
        return BigDecimal(unscaled_ * o.unscaled_, scale_ + o.scale_);
    }

    // ---- 除法 ----
    // 在指定 scale 上按 mode 舍入求商
    BigDecimal divide(const BigDecimal& o, int target_scale,
                      RoundingMode mode = RoundingMode::HalfUp) const {
        if (o.unscaled_.is_zero())
            throw std::domain_error("ceh::BigDecimal: division by zero");
        if (target_scale < 0)
            throw std::invalid_argument("ceh::BigDecimal: negative scale");

        // 期望 result = round(value(a)/value(b) × 10^target_scale)
        //            = round(ua × 10^(target_scale + sb - sa) / ub)
        int e = target_scale + o.scale_ - scale_;
        BigInt num = unscaled_;
        BigInt den = o.unscaled_;
        if (e >= 0) num = num * pow10(e);
        else        den = den * pow10(-e);

        BigInt q = divide_rounded(num, den, mode);
        return BigDecimal(q, target_scale);
    }

    BigDecimal operator/(const BigDecimal& o) const {
        int s = std::max(scale_, std::max(o.scale_, kDefaultDivScale));
        return divide(o, s, RoundingMode::HalfUp);
    }

    // ---- 复合赋值 ----
    BigDecimal& operator+=(const BigDecimal& o) { return *this = *this + o; }
    BigDecimal& operator-=(const BigDecimal& o) { return *this = *this - o; }
    BigDecimal& operator*=(const BigDecimal& o) { return *this = *this * o; }
    BigDecimal& operator/=(const BigDecimal& o) { return *this = *this / o; }

    // ---- 比较（对齐 scale 后按数值比较）----
    int compare(const BigDecimal& o) const {
        int s = std::max(scale_, o.scale_);
        return rescaled_unscaled(s).compare(o.rescaled_unscaled(s));
    }
    bool operator==(const BigDecimal& o) const { return compare(o) == 0; }
    bool operator!=(const BigDecimal& o) const { return compare(o) != 0; }
    bool operator<(const BigDecimal& o)  const { return compare(o) <  0; }
    bool operator<=(const BigDecimal& o) const { return compare(o) <= 0; }
    bool operator>(const BigDecimal& o)  const { return compare(o) >  0; }
    bool operator>=(const BigDecimal& o) const { return compare(o) >= 0; }

    // 数值且 scale 都相同（比 == 更严格）
    bool equals_exact(const BigDecimal& o) const {
        return scale_ == o.scale_ && unscaled_ == o.unscaled_;
    }

    // ---- 调整 scale ----
    BigDecimal set_scale(int new_scale, RoundingMode mode = RoundingMode::HalfUp) const {
        if (new_scale < 0)
            throw std::invalid_argument("ceh::BigDecimal: negative scale");
        if (new_scale >= scale_)
            return BigDecimal(unscaled_ * pow10(new_scale - scale_), new_scale);
        // 降低精度：除以 10^(scale_-new_scale) 并舍入
        BigInt q = divide_rounded(unscaled_, pow10(scale_ - new_scale), mode);
        return BigDecimal(q, new_scale);
    }

    // 去掉尾部多余的零（不改变数值），如 1.500 -> 1.5、0.00 -> 0
    BigDecimal stripped_trailing_zeros() const {
        if (unscaled_.is_zero()) return BigDecimal(BigInt(0), 0);
        BigInt u = unscaled_;
        int s = scale_;
        BigInt ten(10);
        while (s > 0) {
            auto qr = u.divmod(ten);
            if (!qr.second.is_zero()) break;
            u = qr.first;
            --s;
        }
        return BigDecimal(u, s);
    }

    // ---- 幂（指数非负）----
    BigDecimal pow(unsigned long long exp) const {
        return BigDecimal(unscaled_.pow(exp), static_cast<int>(scale_ * exp));
    }

    // ---- 转字符串 / 输出 ----
    std::string to_string() const {
        if (scale_ == 0) return unscaled_.to_string();
        std::string digits = unscaled_.abs().to_string();
        // 至少需要 scale_+1 位，不足则左补零（保证整数部分至少一位）
        if (static_cast<int>(digits.size()) <= scale_)
            digits = std::string(scale_ + 1 - digits.size(), '0') + digits;
        std::size_t point = digits.size() - static_cast<std::size_t>(scale_);
        std::string out;
        if (unscaled_.sign() < 0) out.push_back('-');
        out += digits.substr(0, point);
        out.push_back('.');
        out += digits.substr(point);
        return out;
    }

    friend std::ostream& operator<<(std::ostream& os, const BigDecimal& v) {
        return os << v.to_string();
    }

private:
    BigInt unscaled_;
    int scale_;

    static BigInt pow10(int n) { return BigInt(10).pow(static_cast<unsigned long long>(n)); }

    // 把本数提升到目标 scale（>= 当前 scale）后的无标度整数
    BigInt rescaled_unscaled(int target) const {
        return unscaled_ * pow10(target - scale_);
    }

    // 计算 num/den 并按 mode 舍入到整数（den != 0）。符号独立处理，
    // 因为 BigInt::divmod 向零截断。
    static BigInt divide_rounded(const BigInt& num, const BigInt& den, RoundingMode mode) {
        BigInt abs_num = num.abs();
        BigInt abs_den = den.abs();
        auto qr = abs_num.divmod(abs_den);
        BigInt abs_q = qr.first;
        const BigInt& abs_r = qr.second;
        bool negative = (num.sign() < 0) != (den.sign() < 0);

        bool bump = false;  // 是否令 |q| 进 1
        switch (mode) {
            case RoundingMode::Down:
                bump = false;
                break;
            case RoundingMode::HalfUp:
                bump = (abs_r * BigInt(2)).compare(abs_den) >= 0;
                break;
            case RoundingMode::Floor:    // 向 -inf：负数且有余数则远离零
                bump = negative && !abs_r.is_zero();
                break;
            case RoundingMode::Ceiling:  // 向 +inf：正数且有余数则远离零
                bump = !negative && !abs_r.is_zero();
                break;
        }
        if (bump) abs_q = abs_q + BigInt(1);
        return negative ? -abs_q : abs_q;
    }

    void parse(const std::string& s) {
        std::size_t i = 0, n = s.size();
        while (i < n && (s[i] == ' ' || s[i] == '\t')) ++i;
        std::string sign;
        if (i < n && (s[i] == '+' || s[i] == '-')) {
            if (s[i] == '-') sign = "-";
            ++i;
        }
        std::string int_digits, frac_digits;
        while (i < n && s[i] >= '0' && s[i] <= '9') int_digits.push_back(s[i++]);
        if (i < n && s[i] == '.') {
            ++i;
            while (i < n && s[i] >= '0' && s[i] <= '9') frac_digits.push_back(s[i++]);
        }
        // 可选指数
        long long exp = 0;
        if (i < n && (s[i] == 'e' || s[i] == 'E')) {
            ++i;
            bool eneg = false;
            if (i < n && (s[i] == '+' || s[i] == '-')) { eneg = (s[i] == '-'); ++i; }
            std::string edigits;
            while (i < n && s[i] >= '0' && s[i] <= '9') edigits.push_back(s[i++]);
            if (edigits.empty())
                throw std::invalid_argument("ceh::BigDecimal: malformed exponent in \"" + s + "\"");
            exp = std::stoll(edigits);
            if (eneg) exp = -exp;
        }
        if (i != n)
            throw std::invalid_argument("ceh::BigDecimal: invalid character in \"" + s + "\"");
        if (int_digits.empty() && frac_digits.empty())
            throw std::invalid_argument("ceh::BigDecimal: no digits in \"" + s + "\"");

        std::string all = int_digits + frac_digits;   // 全部有效数字
        long long scale = static_cast<long long>(frac_digits.size()) - exp;

        if (scale >= 0) {
            unscaled_ = BigInt(sign + (all.empty() ? "0" : all));
            scale_ = static_cast<int>(scale);
        } else {
            // 负 scale：把多余的指数并入整数部分
            BigInt u(sign + (all.empty() ? "0" : all));
            unscaled_ = u * pow10(static_cast<int>(-scale));
            scale_ = 0;
        }
    }
};

}  // namespace ceh

#endif  // CEH_TYPES_BIG_DECIMAL_HPP
