// ceh/types/big_int.hpp
//
// BigInt —— 任意精度有符号整数（header-only）。
//
// 表示方式：符号-幅值（sign-magnitude）。
//   - mag_ 以 base 1e9 的「肢」（limb）小端存储，无前导零（空 vector 表示 0）。
//   - neg_ 表示符号；0 始终为非负。
//
// 支持：构造（整型 / 字符串）、+ - * / %、比较、自增自减、复合赋值、
//        一元负号、abs、pow（快速幂）、to_string、流输出。
//
// 复杂度：加减 O(n)，乘法 O(n·m)（schoolbook），除法 O(n·m·log B)。
//
// 用法：
//     ceh::BigInt a = "123456789012345678901234567890";
//     ceh::BigInt b = 998244353;
//     std::cout << (a * b) << '\n';
//     std::cout << ceh::BigInt(2).pow(100) << '\n';   // 2^100

#ifndef CEH_TYPES_BIG_INT_HPP
#define CEH_TYPES_BIG_INT_HPP

#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <type_traits>
#include <ostream>
#include <istream>

namespace ceh {

class BigInt {
public:
    // ---- 构造 ----
    BigInt() = default;                       // 0

    // 任意整型（约束为 std::is_integral，避免与 const char* 构造产生歧义，
    // 例如字面量 0 是空指针常量时）
    template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
    BigInt(T v) {
        if constexpr (std::is_signed<T>::value) {
            if (v < 0) {
                neg_ = true;
                // 取绝对值时走 unsigned，避免最小值溢出（如 LLONG_MIN）
                store(0ull - static_cast<unsigned long long>(v));
                return;
            }
        }
        store(static_cast<unsigned long long>(v));
    }

    BigInt(const std::string& s) { parse(s); }
    BigInt(const char* s) { parse(std::string(s)); }

    // ---- 状态查询 ----
    bool is_zero() const { return mag_.empty(); }

    // 返回 -1 / 0 / 1
    int sign() const {
        if (mag_.empty()) return 0;
        return neg_ ? -1 : 1;
    }

    BigInt abs() const {
        BigInt r = *this;
        r.neg_ = false;
        return r;
    }

    // ---- 一元运算 ----
    BigInt operator-() const {
        BigInt r = *this;
        if (!r.mag_.empty()) r.neg_ = !r.neg_;
        return r;
    }

    BigInt operator+() const { return *this; }

    // ---- 加减 ----
    BigInt operator+(const BigInt& o) const {
        if (neg_ == o.neg_) {
            BigInt r;
            r.mag_ = add_mag(mag_, o.mag_);
            r.neg_ = neg_;
            r.trim();
            return r;
        }
        // 异号：转为减法
        int c = cmp_mag(mag_, o.mag_);
        BigInt r;
        if (c == 0) return r;            // 相互抵消为 0
        if (c > 0) {
            r.mag_ = sub_mag(mag_, o.mag_);
            r.neg_ = neg_;
        } else {
            r.mag_ = sub_mag(o.mag_, mag_);
            r.neg_ = o.neg_;
        }
        r.trim();
        return r;
    }

    BigInt operator-(const BigInt& o) const { return *this + (-o); }

    // ---- 乘法 ----
    BigInt operator*(const BigInt& o) const {
        BigInt r;
        if (mag_.empty() || o.mag_.empty()) return r;   // 任一为 0
        r.mag_ = mul_mag(mag_, o.mag_);
        r.neg_ = (neg_ != o.neg_);
        r.trim();
        return r;
    }

    // ---- 除法 / 取模 ----
    // 采用向零截断（truncated division），与 C++ 内建整数一致：
    //   (-7) / 2 == -3,  (-7) % 2 == -1
    BigInt operator/(const BigInt& o) const { return divmod(o).first; }
    BigInt operator%(const BigInt& o) const { return divmod(o).second; }

    std::pair<BigInt, BigInt> divmod(const BigInt& o) const {
        if (o.mag_.empty()) throw std::domain_error("ceh::BigInt: division by zero");
        BigInt q, r;
        divmod_mag(mag_, o.mag_, q.mag_, r.mag_);
        q.neg_ = (neg_ != o.neg_);
        r.neg_ = neg_;                  // 余数符号随被除数
        q.trim();
        r.trim();
        return {q, r};
    }

    // ---- 复合赋值 ----
    BigInt& operator+=(const BigInt& o) { return *this = *this + o; }
    BigInt& operator-=(const BigInt& o) { return *this = *this - o; }
    BigInt& operator*=(const BigInt& o) { return *this = *this * o; }
    BigInt& operator/=(const BigInt& o) { return *this = *this / o; }
    BigInt& operator%=(const BigInt& o) { return *this = *this % o; }

    // ---- 自增自减 ----
    BigInt& operator++() { return *this += BigInt(1); }
    BigInt& operator--() { return *this -= BigInt(1); }
    BigInt operator++(int) { BigInt t = *this; ++*this; return t; }
    BigInt operator--(int) { BigInt t = *this; --*this; return t; }

    // ---- 比较 ----
    int compare(const BigInt& o) const {
        if (neg_ != o.neg_) return neg_ ? -1 : 1;
        int c = cmp_mag(mag_, o.mag_);
        return neg_ ? -c : c;          // 同为负数时大小关系翻转
    }
    bool operator==(const BigInt& o) const { return compare(o) == 0; }
    bool operator!=(const BigInt& o) const { return compare(o) != 0; }
    bool operator<(const BigInt& o)  const { return compare(o) <  0; }
    bool operator<=(const BigInt& o) const { return compare(o) <= 0; }
    bool operator>(const BigInt& o)  const { return compare(o) >  0; }
    bool operator>=(const BigInt& o) const { return compare(o) >= 0; }

    // ---- 快速幂（exp 必须非负）----
    // 平方-乘法：按 exp 的二进制位，把对应的 base^(2^k) 乘入结果，共 O(log exp) 次乘法
    BigInt pow(unsigned long long exp) const {
        BigInt result(1), base = *this;
        while (exp > 0) {
            if (exp & 1ull) result *= base;
            exp >>= 1;
            if (exp) base *= base;
        }
        return result;
    }

    // ---- 转字符串 / 输出 ----
    std::string to_string() const {
        if (mag_.empty()) return "0";
        std::string s;
        if (neg_) s.push_back('-');
        s += std::to_string(mag_.back());   // 最高肢：正常输出，不补零
        // 其余每肢必须补满 BASE_DIGITS(9) 位，否则会丢掉肢内的前导零
        // （如肢值 42 实际代表 "000000042"）；i-- > 0 从次高肢遍历到最低肢
        char buf[16];
        for (std::size_t i = mag_.size() - 1; i-- > 0;) {
            std::snprintf(buf, sizeof(buf), "%09u", mag_[i]);
            s += buf;
        }
        return s;
    }

    friend std::ostream& operator<<(std::ostream& os, const BigInt& v) {
        return os << v.to_string();
    }

    friend std::istream& operator>>(std::istream& is, BigInt& v) {
        std::string s;
        is >> s;
        v.parse(s);
        return is;
    }

private:
    static constexpr std::uint32_t BASE = 1000000000u;  // 1e9
    static constexpr int BASE_DIGITS = 9;

    std::vector<std::uint32_t> mag_;   // 小端，base 1e9，无前导零
    bool neg_ = false;

    // 去除高位的零肢；幅值为空时强制 neg_ = false
    void trim() {
        while (!mag_.empty() && mag_.back() == 0) mag_.pop_back();
        if (mag_.empty()) neg_ = false;
    }

    void store(unsigned long long v) {
        mag_.clear();
        while (v > 0) {
            mag_.push_back(static_cast<std::uint32_t>(v % BASE));
            v /= BASE;
        }
    }

    void parse(const std::string& s) {
        mag_.clear();
        neg_ = false;
        std::size_t i = 0;
        while (i < s.size() && (s[i] == ' ' || s[i] == '\t')) ++i;
        bool sign = false;
        if (i < s.size() && (s[i] == '+' || s[i] == '-')) {
            sign = (s[i] == '-');
            ++i;
        }
        std::size_t start = i;
        for (; i < s.size(); ++i) {
            if (s[i] < '0' || s[i] > '9')
                throw std::invalid_argument("ceh::BigInt: invalid digit in \"" + s + "\"");
        }
        if (start == i)
            throw std::invalid_argument("ceh::BigInt: no digits in \"" + s + "\"");

        // 从末尾每 9 位切一肢
        for (std::size_t end = i; end > start;) {
            std::size_t begin = (end >= start + BASE_DIGITS) ? end - BASE_DIGITS : start;
            mag_.push_back(static_cast<std::uint32_t>(
                std::stoul(s.substr(begin, end - begin))));
            end = begin;
        }
        neg_ = sign;
        trim();
    }

    // ---- 幅值层面的静态辅助函数 ----

    static int cmp_mag(const std::vector<std::uint32_t>& a,
                       const std::vector<std::uint32_t>& b) {
        if (a.size() != b.size()) return a.size() < b.size() ? -1 : 1;
        for (std::size_t i = a.size(); i-- > 0;)
            if (a[i] != b[i]) return a[i] < b[i] ? -1 : 1;
        return 0;
    }

    static std::vector<std::uint32_t> add_mag(const std::vector<std::uint32_t>& a,
                                              const std::vector<std::uint32_t>& b) {
        std::vector<std::uint32_t> r;
        r.reserve(std::max(a.size(), b.size()) + 1);
        std::uint64_t carry = 0;
        for (std::size_t i = 0; i < a.size() || i < b.size() || carry; ++i) {
            std::uint64_t cur = carry;
            if (i < a.size()) cur += a[i];
            if (i < b.size()) cur += b[i];
            r.push_back(static_cast<std::uint32_t>(cur % BASE));
            carry = cur / BASE;
        }
        return r;
    }

    // 前提：a >= b（幅值）
    static std::vector<std::uint32_t> sub_mag(const std::vector<std::uint32_t>& a,
                                              const std::vector<std::uint32_t>& b) {
        std::vector<std::uint32_t> r;
        r.reserve(a.size());
        std::int64_t borrow = 0;
        for (std::size_t i = 0; i < a.size(); ++i) {
            std::int64_t cur = static_cast<std::int64_t>(a[i]) - borrow
                             - (i < b.size() ? static_cast<std::int64_t>(b[i]) : 0);
            if (cur < 0) { cur += BASE; borrow = 1; }
            else borrow = 0;
            r.push_back(static_cast<std::uint32_t>(cur));
        }
        while (!r.empty() && r.back() == 0) r.pop_back();
        return r;
    }

    // 课本式 O(n·m) 乘法：逐肢相乘累加再统一进位
    static std::vector<std::uint32_t> mul_mag(const std::vector<std::uint32_t>& a,
                                              const std::vector<std::uint32_t>& b) {
        // 结果最多 a.size()+b.size() 肢；acc 用 64 位，足以容纳「肢积(<1e18)+进位」不溢出
        std::vector<std::uint64_t> acc(a.size() + b.size(), 0);
        for (std::size_t i = 0; i < a.size(); ++i) {
            std::uint64_t carry = 0;
            for (std::size_t j = 0; j < b.size() || carry; ++j) {
                std::uint64_t cur = acc[i + j] + carry
                    + (j < b.size() ? static_cast<std::uint64_t>(a[i]) * b[j] : 0);
                acc[i + j] = cur % BASE;
                carry = cur / BASE;
            }
        }
        std::vector<std::uint32_t> r(acc.size());
        for (std::size_t i = 0; i < acc.size(); ++i)
            r[i] = static_cast<std::uint32_t>(acc[i]);
        while (!r.empty() && r.back() == 0) r.pop_back();
        return r;
    }

    // 单肢乘法：mag * d（d < BASE）
    static std::vector<std::uint32_t> mul_scalar(const std::vector<std::uint32_t>& a,
                                                 std::uint32_t d) {
        if (d == 0 || a.empty()) return {};
        std::vector<std::uint32_t> r;
        r.reserve(a.size() + 1);
        std::uint64_t carry = 0;
        for (std::size_t i = 0; i < a.size(); ++i) {
            std::uint64_t cur = static_cast<std::uint64_t>(a[i]) * d + carry;
            r.push_back(static_cast<std::uint32_t>(cur % BASE));
            carry = cur / BASE;
        }
        while (carry) {
            r.push_back(static_cast<std::uint32_t>(carry % BASE));
            carry /= BASE;
        }
        return r;
    }

    // 长除法：a / b -> (q, r)，b 非空。每个商肢用二分搜索确定。
    static void divmod_mag(const std::vector<std::uint32_t>& a,
                           const std::vector<std::uint32_t>& b,
                           std::vector<std::uint32_t>& q,
                           std::vector<std::uint32_t>& r) {
        q.assign(a.size(), 0);
        std::vector<std::uint32_t> cur;        // 当前余数（小端）
        for (std::size_t i = a.size(); i-- > 0;) {
            // cur = cur * BASE + a[i]
            cur.insert(cur.begin(), a[i]);
            while (!cur.empty() && cur.back() == 0) cur.pop_back();

            // 二分找最大的商肢 d（0..BASE-1），使 b*d <= cur。
            // lo/hi 是 uint32，故在两端额外 break，避免 mid±1 发生回绕（上溢/下溢）。
            std::uint32_t lo = 0, hi = BASE - 1, d = 0;
            while (lo <= hi) {
                std::uint32_t mid = lo + (hi - lo) / 2;
                if (cmp_mag(mul_scalar(b, mid), cur) <= 0) {
                    d = mid;
                    if (mid == BASE - 1) break;   // 否则 lo = mid+1 会上溢回 0
                    lo = mid + 1;
                } else {
                    if (mid == 0) break;          // 否则 hi = mid-1 会下溢到 UINT32_MAX
                    hi = mid - 1;
                }
            }
            q[i] = d;
            cur = sub_mag(cur, mul_scalar(b, d));
        }
        while (!q.empty() && q.back() == 0) q.pop_back();
        r = cur;
        while (!r.empty() && r.back() == 0) r.pop_back();
    }
};

}  // namespace ceh

#endif  // CEH_TYPES_BIG_INT_HPP
