// BigInt 单元测试（零依赖，基于 assert）
#include <ceh/types/big_int.hpp>

#include <cassert>
#include <iostream>
#include <sstream>
#include <string>

using ceh::BigInt;

static int g_checks = 0;

#define CHECK_EQ(expr, expected)                                       \
    do {                                                               \
        ++g_checks;                                                    \
        auto got = (expr);                                            \
        if (!(got == (expected))) {                                    \
            std::cerr << "FAIL [" << __LINE__ << "] " #expr            \
                      << " => " << got << " (expected " << (expected)  \
                      << ")\n";                                        \
            std::exit(1);                                              \
        }                                                              \
    } while (0)

#define CHECK_TRUE(expr)                                               \
    do {                                                               \
        ++g_checks;                                                    \
        if (!(expr)) {                                                 \
            std::cerr << "FAIL [" << __LINE__ << "] " #expr "\n";      \
            std::exit(1);                                              \
        }                                                              \
    } while (0)

static std::string str(const BigInt& v) { return v.to_string(); }

int main() {
    // 构造 / to_string
    CHECK_TRUE(str(BigInt()) == "0");
    CHECK_TRUE(str(BigInt(0)) == "0");
    CHECK_TRUE(str(BigInt(12345)) == "12345");
    CHECK_TRUE(str(BigInt(-12345)) == "-12345");
    CHECK_TRUE(str(BigInt("000123")) == "123");
    CHECK_TRUE(str(BigInt("-000")) == "0");
    CHECK_TRUE(str(BigInt("+42")) == "42");
    CHECK_TRUE(str(BigInt(-9223372036854775807LL - 1)) == "-9223372036854775808");

    // 加减法（含跨符号、进位/借位）
    CHECK_EQ(BigInt(999999999) + BigInt(1), BigInt(1000000000));
    CHECK_EQ(BigInt("99999999999999999999") + BigInt(1), BigInt("100000000000000000000"));
    CHECK_EQ(BigInt(5) - BigInt(8), BigInt(-3));
    CHECK_EQ(BigInt(-5) - BigInt(-8), BigInt(3));
    CHECK_EQ(BigInt(-5) + BigInt(5), BigInt(0));
    CHECK_EQ(BigInt("1000000000000000000000") - BigInt("1"), BigInt("999999999999999999999"));

    // 乘法
    CHECK_EQ(BigInt(123456789) * BigInt(987654321), BigInt("121932631112635269"));
    CHECK_EQ(BigInt(-7) * BigInt(6), BigInt(-42));
    CHECK_EQ(BigInt(0) * BigInt("9999999999999"), BigInt(0));

    // 100! 的已知值
    BigInt fact(1);
    for (int i = 1; i <= 100; ++i) fact *= i;
    CHECK_TRUE(str(fact) ==
        "933262154439441526816992388562667004907159682643816214685929"
        "638952175999932299156089414639761565182862536979208272237582"
        "51185210916864000000000000000000000000");

    // 快速幂
    CHECK_TRUE(str(BigInt(2).pow(100)) == "1267650600228229401496703205376");
    CHECK_EQ(BigInt(10).pow(0), BigInt(1));
    CHECK_EQ(BigInt(-3).pow(3), BigInt(-27));

    // 除法 / 取模（向零截断，与内建 long long 一致）
    CHECK_EQ(BigInt(17) / BigInt(5), BigInt(3));
    CHECK_EQ(BigInt(17) % BigInt(5), BigInt(2));
    CHECK_EQ(BigInt(-17) / BigInt(5), BigInt(-3));
    CHECK_EQ(BigInt(-17) % BigInt(5), BigInt(-2));
    CHECK_EQ(BigInt(17) / BigInt(-5), BigInt(-3));
    CHECK_EQ(BigInt(17) % BigInt(-5), BigInt(2));
    CHECK_EQ(BigInt("100000000000000000000") / BigInt(7), BigInt("14285714285714285714"));
    CHECK_EQ(BigInt("100000000000000000000") % BigInt(7), BigInt(2));

    // 除法自洽性：a == (a/b)*b + a%b
    {
        BigInt a("123456789012345678901234567890");
        BigInt b("987654321098765");
        auto qr = a.divmod(b);
        CHECK_EQ(qr.first * b + qr.second, a);
        CHECK_TRUE(qr.second.abs() < b.abs());
    }

    // 比较
    CHECK_TRUE(BigInt(-1) < BigInt(0));
    CHECK_TRUE(BigInt("1000000000000") > BigInt(999999999999LL));
    CHECK_TRUE(BigInt(-100) < BigInt(-99));
    CHECK_TRUE(BigInt(42) == BigInt("42"));
    CHECK_TRUE(BigInt(42) >= BigInt(42));

    // 自增自减 / 一元负号 / abs / sign
    {
        BigInt x(999999999);
        CHECK_EQ(++x, BigInt(1000000000));
        CHECK_EQ(x--, BigInt(1000000000));
        CHECK_EQ(x, BigInt(999999999));
        CHECK_EQ(-BigInt(5), BigInt(-5));
        CHECK_EQ(BigInt(-5).abs(), BigInt(5));
        CHECK_TRUE(BigInt(-5).sign() == -1);
        CHECK_TRUE(BigInt(0).sign() == 0);
        CHECK_TRUE(BigInt(5).sign() == 1);
    }

    // 流输入输出
    {
        std::istringstream in("-123456789012345678901234567890");
        BigInt v;
        in >> v;
        std::ostringstream out;
        out << v;
        CHECK_TRUE(out.str() == "-123456789012345678901234567890");
    }

    // 除零抛异常
    {
        bool threw = false;
        try { (void)(BigInt(1) / BigInt(0)); }
        catch (const std::domain_error&) { threw = true; }
        CHECK_TRUE(threw);
    }

    // 非法字符抛异常
    {
        bool threw = false;
        try { BigInt bad("12a3"); }
        catch (const std::invalid_argument&) { threw = true; }
        CHECK_TRUE(threw);
    }

    std::cout << "All BigInt tests passed (" << g_checks << " checks).\n";
    return 0;
}
