// Complex 单元测试（零依赖，基于 assert）
#include <ceh/types/complex.hpp>

#include <cmath>
#include <complex>
#include <iostream>
#include <sstream>
#include <string>

using ceh::Complex;
using C = ceh::Complex<double>;

static int g_checks = 0;

#define CHECK_TRUE(expr)                                               \
    do {                                                               \
        ++g_checks;                                                    \
        if (!(expr)) {                                                 \
            std::cerr << "FAIL [" << __LINE__ << "] " #expr "\n";      \
            std::exit(1);                                              \
        }                                                              \
    } while (0)

static bool close(double a, double b) { return std::abs(a - b) < 1e-9; }
static bool ceq(const C& z, double re, double im) {
    return close(z.real(), re) && close(z.imag(), im);
}

int main() {
    // 构造 / 访问
    CHECK_TRUE(ceq(C(3, 4), 3, 4));
    CHECK_TRUE(ceq(C(5), 5, 0));          // 纯实数
    CHECK_TRUE(ceq(C(), 0, 0));

    // 四则
    CHECK_TRUE(ceq(C(1, 2) + C(3, 4), 4, 6));
    CHECK_TRUE(ceq(C(5, 6) - C(1, 2), 4, 4));
    CHECK_TRUE(ceq(C(1, 2) * C(3, 4), -5, 10));      // (1+2i)(3+4i)=-5+10i
    CHECK_TRUE(ceq(C(3, 4) * C(0, 1), -4, 3));       // 乘 i 即旋转 90°
    CHECK_TRUE(ceq(C(1, 0) / C(0, 1), 0, -1));       // 1/i = -i
    CHECK_TRUE(ceq(C(-5, 10) / C(1, 2), 3, 4));      // 除法是乘法逆运算

    // 与标量混合（隐式构造）
    CHECK_TRUE(ceq(C(2, 3) + 1.0, 3, 3));
    CHECK_TRUE(ceq(C(2, 3) * 2.0, 4, 6));

    // 共轭 / 模 / 模平方 / 辐角
    CHECK_TRUE(ceq(C(3, 4).conj(), 3, -4));
    CHECK_TRUE(close(C(3, 4).abs(), 5.0));
    CHECK_TRUE(close(C(3, 4).norm(), 25.0));
    CHECK_TRUE(close(C(0, 1).arg(), M_PI / 2));
    CHECK_TRUE(close(C(-1, 0).arg(), M_PI));

    // 一元负号
    CHECK_TRUE(ceq(-C(3, -4), -3, 4));

    // 极坐标构造
    CHECK_TRUE(ceq(C::polar(1.0, M_PI), -1, 0));
    CHECK_TRUE(ceq(C::polar(2.0, M_PI / 2), 0, 2));

    // 整数幂（含 0 次与负幂）
    CHECK_TRUE(ceq(C(0, 1).pow(2), -1, 0));          // i^2 = -1
    CHECK_TRUE(ceq(C(0, 1).pow(4), 1, 0));           // i^4 = 1
    CHECK_TRUE(ceq(C(2, 0).pow(10), 1024, 0));
    CHECK_TRUE(ceq(C(3, 4).pow(0), 1, 0));
    CHECK_TRUE(ceq(C(0, 1).pow(-1), 0, -1));         // i^-1 = -i

    // 复合赋值
    {
        C z(1, 1);
        z *= C(0, 1);
        CHECK_TRUE(ceq(z, -1, 1));
        z += C(1, -1);
        CHECK_TRUE(ceq(z, 0, 0));
    }

    // 比较
    CHECK_TRUE(C(1, 2) == C(1, 2));
    CHECK_TRUE(C(1, 2) != C(1, 3));

    // 与 std::complex 互转
    {
        std::complex<double> s(3, 4);
        C z = s;                          // std -> ceh
        CHECK_TRUE(ceq(z, 3, 4));
        std::complex<double> back = z;    // ceh -> std
        CHECK_TRUE(close(back.real(), 3) && close(back.imag(), 4));
        CHECK_TRUE(close(z.to_std().imag(), 4));
    }

    // 字符串输出
    {
        std::ostringstream a; a << C(3, 4);   CHECK_TRUE(a.str() == "3+4i");
        std::ostringstream b; b << C(3, -4);  CHECK_TRUE(b.str() == "3-4i");
        std::ostringstream c; c << C(0, 0);   CHECK_TRUE(c.str() == "0+0i");
    }

    std::cout << "All Complex tests passed (" << g_checks << " checks).\n";
    return 0;
}
