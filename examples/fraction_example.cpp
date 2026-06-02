// Fraction 使用示例
#include <ceh/types/fraction.hpp>
#include <iostream>

int main() {
    using ceh::Fraction;

    // 1) 精确分数运算（无浮点误差）
    Fraction sum;
    for (int n = 1; n <= 10; ++n) sum += Fraction(1, n);
    std::cout << "1/1 + 1/2 + ... + 1/10 = " << sum
              << "  (~" << sum.to_double() << ")\n";

    // 2) 自动约分
    std::cout << "462/1071 = " << Fraction(462, 1071) << "\n";

    // 3) 任意精度分子分母
    Fraction big("123456789012345678901234567890/987654321098765432109876543210");
    std::cout << "big = " << big << "\n";

    // 4) 负指数幂
    std::cout << "(3/4)^-3 = " << Fraction(3, 4).pow(-3) << "\n";

    return 0;
}
