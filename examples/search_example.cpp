// search 算法使用示例
#include <ceh/algorithms/search.hpp>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> v = {1, 3, 5, 7, 9, 11, 13};

    std::cout << "数组: ";
    for (int x : v) std::cout << x << ' ';
    std::cout << "\n";

    std::cout << "查找 7 的下标   = " << ceh::binary_search_index(v, 7) << "\n";
    std::cout << "查找 8 的下标   = " << ceh::binary_search_index(v, 8)
              << " (-1 表示未找到)\n";
    std::cout << "第一个 >= 8 的位置 = " << ceh::lower_bound_index(v, 8) << "\n";
    std::cout << "第一个 > 7 的位置  = " << ceh::upper_bound_index(v, 7) << "\n";

    // 二分答案：求 50 的整数平方根（最大的 x 使 x*x <= 50）
    long long first = ceh::binary_search_first_true(
        0, 100, [](long long x) { return x * x > 50; });
    std::cout << "floor(sqrt(50)) = " << (first - 1) << "\n";

    return 0;
}
