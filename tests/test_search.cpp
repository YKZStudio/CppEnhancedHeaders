// search 算法单元测试（零依赖，基于 assert）
#include <ceh/algorithms/search.hpp>

#include <functional>
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

int main() {
    std::vector<int> v = {1, 3, 5, 7, 9, 11};

    // binary_search_index：命中返回下标，未命中返回 -1
    CHECK_TRUE(ceh::binary_search_index(v, 1) == 0);
    CHECK_TRUE(ceh::binary_search_index(v, 7) == 3);
    CHECK_TRUE(ceh::binary_search_index(v, 11) == 5);
    CHECK_TRUE(ceh::binary_search_index(v, 4) == -1);
    CHECK_TRUE(ceh::binary_search_index(v, 0) == -1);
    CHECK_TRUE(ceh::binary_search_index(v, 100) == -1);
    {
        std::vector<int> empty;
        CHECK_TRUE(ceh::binary_search_index(empty, 5) == -1);
    }

    // lower_bound_index：第一个 >= target
    CHECK_TRUE(ceh::lower_bound_index(v, 5) == 2);    // 命中，指向 5
    CHECK_TRUE(ceh::lower_bound_index(v, 4) == 2);    // 落在 3 与 5 之间 -> 5
    CHECK_TRUE(ceh::lower_bound_index(v, 0) == 0);    // 比所有都小
    CHECK_TRUE(ceh::lower_bound_index(v, 11) == 5);
    CHECK_TRUE(ceh::lower_bound_index(v, 12) == v.size());  // 比所有都大

    // upper_bound_index：第一个 > target
    CHECK_TRUE(ceh::upper_bound_index(v, 5) == 3);    // 跳过 5，指向 7
    CHECK_TRUE(ceh::upper_bound_index(v, 4) == 2);
    CHECK_TRUE(ceh::upper_bound_index(v, 11) == v.size());
    CHECK_TRUE(ceh::upper_bound_index(v, 0) == 0);

    // 含重复元素时 lower / upper 界定相等区间
    {
        std::vector<int> d = {1, 2, 2, 2, 3};
        CHECK_TRUE(ceh::lower_bound_index(d, 2) == 1);
        CHECK_TRUE(ceh::upper_bound_index(d, 2) == 4);
        CHECK_TRUE(ceh::upper_bound_index(d, 2) - ceh::lower_bound_index(d, 2) == 3);
    }

    // 自定义比较器：降序数组
    {
        std::vector<int> desc = {9, 7, 5, 3, 1};
        CHECK_TRUE(ceh::binary_search_index(desc, 5, std::greater<>{}) == 2);
        CHECK_TRUE(ceh::binary_search_index(desc, 6, std::greater<>{}) == -1);
        CHECK_TRUE(ceh::lower_bound_index(desc, 6, std::greater<>{}) == 2);
    }

    // 二分答案：找满足 x*x >= 100 的最小 x
    CHECK_TRUE(ceh::binary_search_first_true(
                   0, 1000, [](long long x) { return x * x >= 100; }) == 10);
    // 找满足 x >= 7 的最小 x
    CHECK_TRUE(ceh::binary_search_first_true(
                   0, 100, [](long long x) { return x >= 7; }) == 7);
    // 区间内无解：返回 hi + 1
    CHECK_TRUE(ceh::binary_search_first_true(
                   0, 5, [](long long x) { return x > 100; }) == 6);
    // 全部满足：返回 lo
    CHECK_TRUE(ceh::binary_search_first_true(
                   0, 5, [](long long) { return true; }) == 0);

    std::cout << "All search tests passed (" << g_checks << " checks).\n";
    return 0;
}
