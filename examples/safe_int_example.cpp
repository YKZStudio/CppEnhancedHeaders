// SafeInt 使用示例
#include <ceh/types/safe_int.hpp>
#include <iostream>

int main() {
    using ceh::SafeInt;

    SafeInt<int> a = 2'000'000'000;
    std::cout << "a = " << a << "\n";

    try {
        auto b = a + a;            // 触发溢出
        std::cout << "a + a = " << b << "\n";
    } catch (const std::overflow_error& e) {
        std::cout << "捕获溢出: " << e.what() << "\n";
    }

    // 用更宽的类型则安全
    SafeInt<long long> c = 2'000'000'000LL;
    std::cout << "(long long) c + c = " << c + c << "\n";

    // 阶乘溢出检测：找出 int 能容纳的最大 n!
    SafeInt<int> fact = 1;
    int n = 1;
    try {
        for (; n <= 20; ++n) fact *= n;
    } catch (const std::overflow_error&) {
        std::cout << (n) << "! 时 int 溢出；最后安全值 " << (n - 1) << "! = "
                  << fact << "\n";
    }
    return 0;
}
