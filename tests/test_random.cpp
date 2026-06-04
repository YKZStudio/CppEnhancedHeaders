// Random 单元测试（零依赖，基于 assert）
//
// 不硬编码具体随机值（分布实现跨标准库可能不同），而是验证：
// 可复现性、取值范围、洗牌保持多重集、原始引擎序列等性质。
#include <ceh/utils/random.hpp>

#include <algorithm>
#include <iostream>
#include <vector>

using ceh::Random;

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
    // 相同种子可复现
    {
        Random a(42), b(42);
        for (int i = 0; i < 100; ++i) CHECK_TRUE(a.next_u64() == b.next_u64());
        Random c(12345), d(12345);
        for (int i = 0; i < 50; ++i) CHECK_TRUE(c.next_int(0, 1000000) == d.next_int(0, 1000000));
    }

    // 不同种子（几乎必然）产生不同序列
    {
        Random a(1), b(2);
        bool any_diff = false;
        for (int i = 0; i < 10; ++i)
            if (a.next_u64() != b.next_u64()) { any_diff = true; break; }
        CHECK_TRUE(any_diff);
    }

    // next_int 落在闭区间内，且退化区间返回唯一值
    {
        Random rng(7);
        for (int i = 0; i < 1000; ++i) {
            int x = rng.next_int(1, 6);
            CHECK_TRUE(x >= 1 && x <= 6);
        }
        CHECK_TRUE(rng.next_int(5, 5) == 5);
        for (int i = 0; i < 100; ++i) {
            int x = rng.next_int(-3, 3);
            CHECK_TRUE(x >= -3 && x <= 3);
        }
    }
    // lo > hi 抛异常
    {
        Random rng(7);
        CHECK_THROWS(std::invalid_argument, rng.next_int(10, 1));
    }

    // next_double 落在 [lo, hi)
    {
        Random rng(99);
        for (int i = 0; i < 1000; ++i) {
            double u = rng.next_double();
            CHECK_TRUE(u >= 0.0 && u < 1.0);
        }
        for (int i = 0; i < 1000; ++i) {
            double u = rng.next_double(-2.0, 2.0);
            CHECK_TRUE(u >= -2.0 && u < 2.0);
        }
    }

    // next_bool 在极端概率下的确定性
    {
        Random rng(3);
        for (int i = 0; i < 50; ++i) CHECK_TRUE(rng.next_bool(1.0) == true);
        for (int i = 0; i < 50; ++i) CHECK_TRUE(rng.next_bool(0.0) == false);
    }

    // choice 返回容器内元素；空容器抛异常
    {
        Random rng(5);
        std::vector<int> v = {10, 20, 30, 40};
        for (int i = 0; i < 100; ++i) {
            int x = rng.choice(v);
            CHECK_TRUE(std::find(v.begin(), v.end(), x) != v.end());
        }
        std::vector<int> empty;
        CHECK_THROWS(std::out_of_range, rng.choice(empty));
    }

    // shuffle 保持多重集（排序后相等），且固定种子可复现
    {
        std::vector<int> original;
        for (int i = 0; i < 50; ++i) original.push_back(i);

        std::vector<int> a = original, b = original;
        Random(123).shuffle(a);
        Random(123).shuffle(b);
        CHECK_TRUE(a == b);                      // 同种子同结果

        std::vector<int> sorted_a = a;
        std::sort(sorted_a.begin(), sorted_a.end());
        CHECK_TRUE(sorted_a == original);        // 元素集合不变
        CHECK_TRUE(a != original);               // 顺序确实被打乱（极大概率）
    }

    std::cout << "All Random tests passed (" << g_checks << " checks).\n";
    return 0;
}
