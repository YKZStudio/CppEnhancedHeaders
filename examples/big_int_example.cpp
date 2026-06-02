// BigInt 使用示例
#include <ceh/types/big_int.hpp>
#include <iostream>

int main() {
    using ceh::BigInt;

    // 1) 计算 100!
    BigInt factorial = 1;
    for (int i = 1; i <= 100; ++i) factorial *= i;
    std::cout << "100! =\n" << factorial << "\n\n";

    // 2) 大数乘法
    BigInt a = "123456789012345678901234567890";
    BigInt b = "987654321098765432109876543210";
    std::cout << "a * b =\n" << a * b << "\n\n";

    // 3) 快速幂 2^256
    std::cout << "2^256 =\n" << BigInt(2).pow(256) << "\n\n";

    // 4) 带余除法
    auto [q, r] = a.divmod(b == 0 ? BigInt(7) : BigInt("998244353"));
    std::cout << "a / 998244353 = " << q << "\n";
    std::cout << "a % 998244353 = " << r << "\n";

    return 0;
}
