// math 算法使用示例
#include <ceh/algorithms/math.hpp>
#include <iostream>

int main() {
    std::cout << "gcd(48, 36) = " << ceh::gcd(48, 36) << "\n";
    std::cout << "lcm(4, 6)   = " << ceh::lcm(4, 6) << "\n";
    std::cout << "2^20        = " << ceh::ipow(2, 20) << "\n";
    std::cout << "2^100 mod (1e9+7) = " << ceh::pow_mod(2, 100, 1000000007ull) << "\n";

    std::cout << "100 以内的素数: ";
    for (auto p : ceh::primes_up_to(100)) std::cout << p << ' ';
    std::cout << "\n";

    std::cout << "97 是素数? " << (ceh::is_prime(97) ? "是" : "否") << "\n";
    std::cout << "3 在 mod 11 下的逆元 = " << ceh::mod_inverse(3, 11) << "\n";

    return 0;
}
