// dp 算法使用示例
#include <ceh/algorithms/dp.hpp>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> v{10, 9, 2, 5, 3, 7, 101, 18};
    std::cout << "最长递增子序列: ";
    for (int x : ceh::longest_increasing_subsequence(v)) std::cout << x << ' ';
    std::cout << "(长度 " << ceh::longest_increasing_subsequence_length(v) << ")\n";

    std::cout << "编辑距离(kitten, sitting) = "
              << ceh::edit_distance("kitten", "sitting") << "\n";

    std::cout << "0/1 背包(cap=5) 最大价值 = "
              << ceh::knapsack_01({2, 3, 4, 5}, {3, 4, 5, 6}, 5) << "\n";

    std::cout << "凑出 11 的最少硬币数(1,2,5) = "
              << ceh::coin_change({1, 2, 5}, 11) << "\n";
    return 0;
}
