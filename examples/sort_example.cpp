// sort 算法使用示例
#include <ceh/algorithms/sort.hpp>
#include <functional>
#include <iostream>
#include <vector>

static void print(const char* label, const std::vector<int>& v) {
    std::cout << label;
    for (int x : v) std::cout << x << ' ';
    std::cout << "\n";
}

int main() {
    std::vector<int> base = {5, 2, 8, 1, 9, 3, 7, 4, 6};

    auto v1 = base; ceh::insertion_sort(v1.begin(), v1.end());
    print("插入排序: ", v1);

    auto v2 = base; ceh::merge_sort(v2.begin(), v2.end());
    print("归并排序: ", v2);

    auto v3 = base; ceh::quick_sort(v3.begin(), v3.end());
    print("快速排序: ", v3);

    auto v4 = base; ceh::heap_sort(v4.begin(), v4.end());
    print("堆排序  : ", v4);

    auto v5 = base; ceh::quick_sort(v5.begin(), v5.end(), std::greater<>{});
    print("快排降序: ", v5);

    return 0;
}
