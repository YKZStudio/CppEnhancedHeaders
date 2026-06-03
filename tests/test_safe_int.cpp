// SafeInt 单元测试（零依赖，基于 assert）
#include <ceh/types/safe_int.hpp>

#include <climits>
#include <iostream>
#include <sstream>
#include <string>

using ceh::SafeInt;

static int g_checks = 0;

#define CHECK_TRUE(expr)                                               \
    do {                                                               \
        ++g_checks;                                                    \
        if (!(expr)) {                                                 \
            std::cerr << "FAIL [" << __LINE__ << "] " #expr "\n";      \
            std::exit(1);                                              \
        }                                                              \
    } while (0)

// 断言表达式抛出 ExType
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
    using S = SafeInt<int>;

    // 基本运算（未溢出）
    CHECK_TRUE((S(2) + S(3)).value() == 5);
    CHECK_TRUE((S(10) - S(4)).value() == 6);
    CHECK_TRUE((S(6) * S(7)).value() == 42);
    CHECK_TRUE((S(20) / S(3)).value() == 6);
    CHECK_TRUE((S(20) % S(3)).value() == 2);
    CHECK_TRUE((-S(5)).value() == -5);
    CHECK_TRUE(S().value() == 0);

    // 与整型混合（隐式构造）
    CHECK_TRUE((S(2) + 3).value() == 5);
    CHECK_TRUE(S(7) == 7);
    CHECK_TRUE(S(3) < 4);

    // 加法溢出 / 下溢
    CHECK_THROWS(std::overflow_error, S(INT_MAX) + S(1));
    CHECK_THROWS(std::overflow_error, S(INT_MIN) + S(-1));
    CHECK_TRUE((S(INT_MAX) + S(0)).value() == INT_MAX);   // 边界不误报

    // 减法溢出 / 下溢
    CHECK_THROWS(std::overflow_error, S(INT_MIN) - S(1));
    CHECK_THROWS(std::overflow_error, S(INT_MAX) - S(-1));
    CHECK_TRUE((S(INT_MIN) - S(0)).value() == INT_MIN);

    // 乘法溢出（四个象限）
    CHECK_THROWS(std::overflow_error, S(INT_MAX) * S(2));
    CHECK_THROWS(std::overflow_error, S(INT_MAX) * S(-2));
    CHECK_THROWS(std::overflow_error, S(INT_MIN) * S(2));
    CHECK_THROWS(std::overflow_error, S(INT_MIN) * S(-1));
    CHECK_THROWS(std::overflow_error, S(-100000) * S(-100000));
    CHECK_TRUE((S(46340) * S(46340)).value() == 46340 * 46340);  // 刚好不溢出
    CHECK_TRUE((S(0) * S(INT_MAX)).value() == 0);

    // 一元负号 / 除法的 INT_MIN 特例
    CHECK_THROWS(std::overflow_error, -S(INT_MIN));
    CHECK_THROWS(std::overflow_error, S(INT_MIN) / S(-1));
    CHECK_TRUE((S(INT_MIN) % S(-1)).value() == 0);

    // 除 / 模零
    CHECK_THROWS(std::domain_error, S(1) / S(0));
    CHECK_THROWS(std::domain_error, S(1) % S(0));

    // 复合赋值 / 自增自减
    {
        S x = 10;
        x += 5; CHECK_TRUE(x.value() == 15);
        x *= 2; CHECK_TRUE(x.value() == 30);
        x -= 1; CHECK_TRUE(x.value() == 29);
        CHECK_TRUE((x++).value() == 29);
        CHECK_TRUE(x.value() == 30);
        CHECK_TRUE((--x).value() == 29);
    }
    {
        S x = INT_MAX;
        CHECK_THROWS(std::overflow_error, ++x);
    }

    // 更宽的类型不溢出
    CHECK_TRUE((SafeInt<long long>(3'000'000'000LL) + SafeInt<long long>(3'000'000'000LL))
                   .value() == 6'000'000'000LL);

    // 输出
    {
        std::ostringstream out;
        out << S(-42);
        CHECK_TRUE(out.str() == "-42");
        CHECK_TRUE(S(123).to_string() == "123");
    }

    std::cout << "All SafeInt tests passed (" << g_checks << " checks).\n";
    return 0;
}
