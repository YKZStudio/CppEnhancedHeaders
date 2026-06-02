// Fraction 单元测试（零依赖，基于 assert）
#include <ceh/types/fraction.hpp>

#include <cmath>
#include <iostream>
#include <sstream>
#include <string>

using ceh::Fraction;
using ceh::BigInt;

static int g_checks = 0;

#define CHECK_STR(expr, expected)                                      \
    do {                                                               \
        ++g_checks;                                                    \
        std::string got = (expr).to_string();                         \
        if (got != (expected)) {                                       \
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

int main() {
    // 构造 + 自动约分 + 规范化
    CHECK_STR(Fraction(), "0");
    CHECK_STR(Fraction(5), "5");
    CHECK_STR(Fraction(2, 4), "1/2");
    CHECK_STR(Fraction(-1, 3), "-1/3");
    CHECK_STR(Fraction(1, -3), "-1/3");          // 符号移到分子
    CHECK_STR(Fraction(-1, -3), "1/3");
    CHECK_STR(Fraction(0, 5), "0");              // 零规约为 0/1
    CHECK_STR(Fraction(6, 3), "2");              // 化为整数
    CHECK_STR(Fraction("22/7"), "22/7");
    CHECK_STR(Fraction("-10/20"), "-1/2");
    CHECK_STR(Fraction("42"), "42");

    // 加减乘除
    CHECK_STR(Fraction(1, 3) + Fraction(1, 6), "1/2");
    CHECK_STR(Fraction(1, 2) - Fraction(1, 3), "1/6");
    CHECK_STR(Fraction(2, 3) * Fraction(3, 4), "1/2");
    CHECK_STR(Fraction(1, 2) / Fraction(3, 4), "2/3");
    CHECK_STR(Fraction(1, 2) + Fraction(1, 2), "1");
    CHECK_STR(Fraction(3) - Fraction(1, 2), "5/2");

    // 与整数混合运算（隐式构造）
    CHECK_STR(Fraction(1, 2) + 1, "3/2");
    CHECK_STR(Fraction(1, 2) * 4, "2");
    CHECK_TRUE(Fraction(1, 2) + Fraction(1, 2) == 1);

    // 大数有理数：约分正确
    CHECK_STR(Fraction(BigInt("1000000000000000000"),
                       BigInt("2000000000000000000")), "1/2");

    // 复合赋值
    {
        Fraction x(1, 2);
        x += Fraction(1, 3);
        CHECK_STR(x, "5/6");
        x *= 6;
        CHECK_STR(x, "5");
    }

    // 一元负号 / abs / sign / reciprocal
    CHECK_STR(-Fraction(3, 4), "-3/4");
    CHECK_STR(Fraction(-3, 4).abs(), "3/4");
    CHECK_TRUE(Fraction(-3, 4).sign() == -1);
    CHECK_TRUE(Fraction(0).sign() == 0);
    CHECK_STR(Fraction(3, 4).reciprocal(), "4/3");
    CHECK_STR(Fraction(-3, 4).reciprocal(), "-4/3");
    CHECK_TRUE(Fraction(5).is_integer());
    CHECK_TRUE(!Fraction(1, 2).is_integer());

    // 比较
    CHECK_TRUE(Fraction(1, 3) < Fraction(1, 2));
    CHECK_TRUE(Fraction(2, 4) == Fraction(1, 2));
    CHECK_TRUE(Fraction(-1, 2) < Fraction(0));
    CHECK_TRUE(Fraction(7, 8) > Fraction(5, 6));
    CHECK_TRUE(Fraction(3) >= Fraction(3));

    // 幂（含负指数与零次幂）
    CHECK_STR(Fraction(2, 3).pow(3), "8/27");
    CHECK_STR(Fraction(2, 3).pow(0), "1");
    CHECK_STR(Fraction(2, 3).pow(-2), "9/4");
    CHECK_STR(Fraction(5).pow(-1), "1/5");

    // to_double
    CHECK_TRUE(std::abs(Fraction(1, 4).to_double() - 0.25) < 1e-12);
    CHECK_TRUE(std::abs(Fraction("22/7").to_double() - (22.0 / 7.0)) < 1e-12);
    CHECK_TRUE(std::abs(Fraction(-3, 2).to_double() + 1.5) < 1e-12);

    // 流输出
    {
        std::ostringstream out;
        out << Fraction(7, 14);
        CHECK_TRUE(out.str() == "1/2");
    }

    // 除零 / 零的倒数抛异常
    {
        bool threw = false;
        try { (void)(Fraction(1, 2) / Fraction(0)); }
        catch (const std::domain_error&) { threw = true; }
        CHECK_TRUE(threw);
    }
    {
        bool threw = false;
        try { (void)Fraction(0).reciprocal(); }
        catch (const std::domain_error&) { threw = true; }
        CHECK_TRUE(threw);
    }
    // 直接构造零分母抛异常
    {
        bool threw = false;
        try { Fraction bad(1, 0); }
        catch (const std::domain_error&) { threw = true; }
        CHECK_TRUE(threw);
    }

    std::cout << "All Fraction tests passed (" << g_checks << " checks).\n";
    return 0;
}
