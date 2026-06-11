// dp 算法单元测试（零依赖，基于 assert）
#include <ceh/algorithms/dp.hpp>

#include <iostream>
#include <string>
#include <vector>

static int g_checks = 0;

#define CHECK_TRUE(expr)                                               \
    do { ++g_checks; if (!(expr)) {                                    \
        std::cerr << "FAIL [" << __LINE__ << "] " #expr "\n"; std::exit(1);} } while (0)

int main() {
    // 最长递增子序列：长度
    CHECK_TRUE(ceh::longest_increasing_subsequence_length(
                   std::vector<int>{10, 9, 2, 5, 3, 7, 101, 18}) == 4);
    CHECK_TRUE(ceh::longest_increasing_subsequence_length(std::vector<int>{}) == 0);
    CHECK_TRUE(ceh::longest_increasing_subsequence_length(std::vector<int>{5}) == 1);
    CHECK_TRUE(ceh::longest_increasing_subsequence_length(
                   std::vector<int>{3, 3, 3}) == 1);                       // 严格递增
    CHECK_TRUE(ceh::longest_increasing_subsequence_length(
                   std::vector<int>{1, 2, 3, 4, 5}) == 5);

    // LIS 实际序列：长度正确、严格递增
    {
        std::vector<int> v{10, 9, 2, 5, 3, 7, 101, 18};
        auto seq = ceh::longest_increasing_subsequence(v);
        CHECK_TRUE(seq.size() == 4);
        for (std::size_t i = 1; i < seq.size(); ++i) CHECK_TRUE(seq[i - 1] < seq[i]);
    }

    // 编辑距离
    CHECK_TRUE(ceh::edit_distance("kitten", "sitting") == 3);
    CHECK_TRUE(ceh::edit_distance("", "abc") == 3);
    CHECK_TRUE(ceh::edit_distance("abc", "") == 3);
    CHECK_TRUE(ceh::edit_distance("abc", "abc") == 0);
    CHECK_TRUE(ceh::edit_distance("sunday", "saturday") == 3);

    // 0/1 背包
    CHECK_TRUE(ceh::knapsack_01({2, 3, 4, 5}, {3, 4, 5, 6}, 5) == 7);
    CHECK_TRUE(ceh::knapsack_01({1, 2, 3}, {6, 10, 12}, 5) == 22);          // 取后两件
    CHECK_TRUE(ceh::knapsack_01({2, 3}, {3, 4}, 0) == 0);
    CHECK_TRUE(ceh::knapsack_01({10}, {100}, 5) == 0);                       // 放不下

    // 硬币找零
    CHECK_TRUE(ceh::coin_change({1, 2, 5}, 11) == 3);                        // 5+5+1
    CHECK_TRUE(ceh::coin_change({2}, 3) == -1);
    CHECK_TRUE(ceh::coin_change({}, 5) == -1);
    CHECK_TRUE(ceh::coin_change({1, 2, 5}, 0) == 0);
    CHECK_TRUE(ceh::coin_change({186, 419, 83, 408}, 6249) == 20);

    std::cout << "All dp tests passed (" << g_checks << " checks).\n";
    return 0;
}
