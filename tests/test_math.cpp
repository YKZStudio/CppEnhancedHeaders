// math 算法单元测试（零依赖，基于 assert）
#include <ceh/algorithms/math.hpp>

#include <cstdint>
#include <iostream>
#include <vector>

static int g_checks = 0;

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
    using std::int64_t;
    using std::uint64_t;

    // gcd / lcm
    CHECK_TRUE(ceh::gcd(12, 18) == 6);
    CHECK_TRUE(ceh::gcd(0, 5) == 5);
    CHECK_TRUE(ceh::gcd(5, 0) == 5);
    CHECK_TRUE(ceh::gcd(0, 0) == 0);
    CHECK_TRUE(ceh::gcd(-12, 18) == 6);            // 负数取绝对值
    CHECK_TRUE(ceh::gcd(17, 13) == 1);             // 互素
    CHECK_TRUE(ceh::lcm(4, 6) == 12);
    CHECK_TRUE(ceh::lcm(0, 5) == 0);
    CHECK_TRUE(ceh::lcm(21, 6) == 42);

    // 整数快速幂
    CHECK_TRUE(ceh::ipow(2, 10) == 1024);
    CHECK_TRUE(ceh::ipow(3, 0) == 1);
    CHECK_TRUE(ceh::ipow<long long>(2, 40) == 1099511627776LL);
    CHECK_TRUE(ceh::ipow(-2, 3) == -8);

    // 模运算
    CHECK_TRUE(ceh::add_mod(7, 8, 10) == 5);
    CHECK_TRUE(ceh::mul_mod(7, 8, 10) == 6);
    CHECK_TRUE(ceh::pow_mod(2, 10, 1000) == 24);   // 1024 % 1000
    CHECK_TRUE(ceh::pow_mod(2, 100, 1000000007ull) == 976371285ull);
    CHECK_TRUE(ceh::pow_mod(123, 0, 7) == 1);
    CHECK_TRUE(ceh::pow_mod(5, 3, 1) == 0);        // mod 1 恒为 0
    // 大模数下不溢出（费马小定理：a^(p-1) ≡ 1 (mod p)）
    {
        uint64_t p = 9223372036854775783ull;      // 接近 2^63 的素数
        CHECK_TRUE(ceh::pow_mod(2, p - 1, p) == 1);
    }
    CHECK_THROWS(std::invalid_argument, ceh::pow_mod(2, 3, 0));

    // 素性判定
    CHECK_TRUE(!ceh::is_prime(0));
    CHECK_TRUE(!ceh::is_prime(1));
    CHECK_TRUE(ceh::is_prime(2));
    CHECK_TRUE(ceh::is_prime(3));
    CHECK_TRUE(!ceh::is_prime(4));
    CHECK_TRUE(ceh::is_prime(97));
    CHECK_TRUE(!ceh::is_prime(100));
    CHECK_TRUE(ceh::is_prime(1000000007ull));
    CHECK_TRUE(!ceh::is_prime(1000000005ull));

    // 埃氏筛
    {
        auto ps = ceh::primes_up_to(30);
        std::vector<std::size_t> expected{2, 3, 5, 7, 11, 13, 17, 19, 23, 29};
        CHECK_TRUE(ps == expected);
        CHECK_TRUE(ceh::primes_up_to(1).empty());
        CHECK_TRUE(ceh::primes_up_to(0).empty());
        CHECK_TRUE(ceh::primes_up_to(100).size() == 25);  // 100 以内 25 个素数
    }

    // 扩展欧几里得：验证 a*x + b*y == gcd
    {
        int64_t x, y;
        int64_t g = ceh::extended_gcd<int64_t>(240, 46, x, y);
        CHECK_TRUE(g == 2);
        CHECK_TRUE(240 * x + 46 * y == g);
    }

    // 模逆元
    CHECK_TRUE(ceh::mod_inverse(3, 11) == 4);      // 3*4 = 12 ≡ 1 (mod 11)
    CHECK_TRUE((3 * ceh::mod_inverse(3, 11)) % 11 == 1);
    CHECK_TRUE(ceh::mod_inverse(10, 17) == 12);
    CHECK_THROWS(std::invalid_argument, ceh::mod_inverse(2, 4));  // gcd != 1，无逆元
    CHECK_THROWS(std::invalid_argument, ceh::mod_inverse(3, 0));

    std::cout << "All math tests passed (" << g_checks << " checks).\n";
    return 0;
}
