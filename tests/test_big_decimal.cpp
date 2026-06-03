// BigDecimal 单元测试（零依赖，基于 assert）
#include <ceh/types/big_decimal.hpp>

#include <iostream>
#include <string>

using ceh::BigDecimal;
using ceh::BigInt;
using ceh::RoundingMode;

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

#define CHECK_THROWS(ExType, expr)                                     \
    do {                                                               \
        ++g_checks;                                                    \
        bool threw = false;                                            \
        try { (void)(expr); }                                         \
        catch (const ExType&) { threw = true; }                       \
        if (!threw) {                                                  \
            std::cerr << "FAIL [" << __LINE__ << "] expected throw: "  \
                      << #expr "\n";                                   \
            std::exit(1);                                              \
        }                                                              \
    } while (0)

int main() {
    // 构造 / to_string
    CHECK_STR(BigDecimal(), "0");
    CHECK_STR(BigDecimal(10), "10");
    CHECK_STR(BigDecimal("123.45"), "123.45");
    CHECK_STR(BigDecimal("-0.001"), "-0.001");
    CHECK_STR(BigDecimal(".5"), "0.5");
    CHECK_STR(BigDecimal("100"), "100");
    CHECK_STR(BigDecimal(BigInt(5), 3), "0.005");
    CHECK_STR(BigDecimal(BigInt(-12345), 2), "-123.45");
    CHECK_TRUE(BigDecimal("123.45").scale() == 2);
    CHECK_TRUE(BigDecimal("-0.001").sign() == -1);
    CHECK_TRUE(BigDecimal("0.000").is_zero());

    // 科学计数法
    CHECK_STR(BigDecimal("1.5e3"), "1500");
    CHECK_STR(BigDecimal("1.5e-2"), "0.015");
    CHECK_STR(BigDecimal("-2.5E2"), "-250");

    // 加减（对齐 scale），经典浮点会出错的 0.1+0.2
    CHECK_STR(BigDecimal("0.1") + BigDecimal("0.2"), "0.3");
    CHECK_STR(BigDecimal("1.5") + BigDecimal("2.25"), "3.75");
    CHECK_STR(BigDecimal("5") - BigDecimal("0.25"), "4.75");
    CHECK_STR(BigDecimal("0.3") - BigDecimal("0.3"), "0.0");

    // 乘法（scale 相加）
    CHECK_STR(BigDecimal("1.5") * BigDecimal("2"), "3.0");
    CHECK_STR(BigDecimal("1.1") * BigDecimal("1.1"), "1.21");
    CHECK_STR(BigDecimal("-0.5") * BigDecimal("0.5"), "-0.25");

    // 除法 + 舍入模式
    CHECK_STR(BigDecimal(1).divide(BigDecimal(3), 10), "0.3333333333");
    CHECK_STR(BigDecimal(2).divide(BigDecimal(3), 4, RoundingMode::HalfUp), "0.6667");
    CHECK_STR(BigDecimal(2).divide(BigDecimal(3), 4, RoundingMode::Down), "0.6666");
    CHECK_STR(BigDecimal(2).divide(BigDecimal(3), 4, RoundingMode::Ceiling), "0.6667");
    CHECK_STR(BigDecimal(2).divide(BigDecimal(3), 4, RoundingMode::Floor), "0.6666");
    CHECK_STR(BigDecimal(-2).divide(BigDecimal(3), 4, RoundingMode::HalfUp), "-0.6667");
    CHECK_STR(BigDecimal(-2).divide(BigDecimal(3), 4, RoundingMode::Down), "-0.6666");
    CHECK_STR(BigDecimal(-2).divide(BigDecimal(3), 4, RoundingMode::Floor), "-0.6667");
    CHECK_STR(BigDecimal(-2).divide(BigDecimal(3), 4, RoundingMode::Ceiling), "-0.6666");
    // 能整除
    CHECK_STR(BigDecimal(1) / BigDecimal(8), "0.12500000000000000000");  // 默认 scale=20
    CHECK_STR(BigDecimal("2.5").divide(BigDecimal("0.5"), 1), "5.0");

    // set_scale
    CHECK_STR(BigDecimal("1.5").set_scale(3), "1.500");
    CHECK_STR(BigDecimal("1.236").set_scale(2, RoundingMode::HalfUp), "1.24");
    CHECK_STR(BigDecimal("1.236").set_scale(2, RoundingMode::Down), "1.23");
    CHECK_STR(BigDecimal("125").set_scale(0), "125");

    // stripped_trailing_zeros
    CHECK_STR(BigDecimal("1.500").stripped_trailing_zeros(), "1.5");
    CHECK_STR(BigDecimal("0.00").stripped_trailing_zeros(), "0");
    CHECK_STR(BigDecimal("100.00").stripped_trailing_zeros(), "100");

    // 比较（按数值）+ equals_exact（数值且 scale）
    CHECK_TRUE(BigDecimal("1.0") == BigDecimal("1.00"));
    CHECK_TRUE(!BigDecimal("1.0").equals_exact(BigDecimal("1.00")));
    CHECK_TRUE(BigDecimal("1.0").equals_exact(BigDecimal("1.0")));
    CHECK_TRUE(BigDecimal("1.5") > BigDecimal("1.05"));
    CHECK_TRUE(BigDecimal("-0.1") < BigDecimal("0"));
    CHECK_TRUE(BigDecimal("2.50") >= BigDecimal("2.5"));

    // 一元负号 / abs
    CHECK_STR(-BigDecimal("3.14"), "-3.14");
    CHECK_STR(BigDecimal("-3.14").abs(), "3.14");

    // 幂
    CHECK_STR(BigDecimal("1.1").pow(2), "1.21");
    CHECK_STR(BigDecimal("0.1").pow(3), "0.001");
    CHECK_STR(BigDecimal("2").pow(10), "1024");

    // 异常
    CHECK_THROWS(std::domain_error, BigDecimal(1).divide(BigDecimal(0), 5));
    CHECK_THROWS(std::domain_error, BigDecimal(1) / BigDecimal("0.0"));
    CHECK_THROWS(std::invalid_argument, BigDecimal("1.2.3"));
    CHECK_THROWS(std::invalid_argument, BigDecimal("abc"));
    CHECK_THROWS(std::invalid_argument, BigDecimal("1.5e"));
    CHECK_THROWS(std::invalid_argument, BigDecimal(BigInt(1), -1));

    std::cout << "All BigDecimal tests passed (" << g_checks << " checks).\n";
    return 0;
}
