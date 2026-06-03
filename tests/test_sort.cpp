// sort 算法单元测试（零依赖，基于 assert）
#include <ceh/algorithms/sort.hpp>

#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <utility>
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

// 用 std::sort 的结果作为参照，校验某个排序函数
template <typename SortFn>
static void check_against_std(SortFn sortfn) {
    std::vector<std::vector<int>> cases = {
        {},
        {1},
        {2, 1},
        {1, 2, 3, 4, 5},                 // 已升序
        {5, 4, 3, 2, 1},                 // 逆序
        {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5},
        {7, 7, 7, 7},                    // 全相等
        {-3, 0, -1, 2, -2, 1},
    };
    // 一个较大的逆序数组，触发快排的非小区间分支
    std::vector<int> big;
    for (int i = 200; i > 0; --i) big.push_back(i);
    cases.push_back(big);

    for (auto v : cases) {
        std::vector<int> expected = v;
        std::sort(expected.begin(), expected.end());
        sortfn(v.begin(), v.end());
        ++g_checks;
        if (v != expected) {
            std::cerr << "FAIL: sort result mismatch (size " << v.size() << ")\n";
            std::exit(1);
        }
    }
}

int main() {
    // 四种排序都要与 std::sort 一致（默认升序）
    check_against_std([](auto a, auto b) { ceh::insertion_sort(a, b); });
    check_against_std([](auto a, auto b) { ceh::merge_sort(a, b); });
    check_against_std([](auto a, auto b) { ceh::quick_sort(a, b); });
    check_against_std([](auto a, auto b) { ceh::heap_sort(a, b); });

    // 自定义比较器：降序
    {
        std::vector<int> v = {3, 1, 4, 1, 5, 9, 2, 6};
        ceh::quick_sort(v.begin(), v.end(), std::greater<>{});
        CHECK_TRUE(std::is_sorted(v.begin(), v.end(), std::greater<>{}));
    }

    // merge_sort 稳定性：按 first 排序，相同 first 的相对顺序应保持
    {
        std::vector<std::pair<int, int>> v = {
            {1, 0}, {2, 1}, {1, 2}, {2, 3}, {1, 4}, {3, 5}, {2, 6}};
        ceh::merge_sort(v.begin(), v.end(),
                        [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
                            return a.first < b.first;
                        });
        // 期望：first 升序，second 按原始相对顺序
        std::vector<std::pair<int, int>> expected = {
            {1, 0}, {1, 2}, {1, 4}, {2, 1}, {2, 3}, {2, 6}, {3, 5}};
        CHECK_TRUE(v == expected);
    }

    // 字符串排序
    {
        std::vector<std::string> v = {"banana", "apple", "cherry", "date"};
        ceh::heap_sort(v.begin(), v.end());
        CHECK_TRUE((v == std::vector<std::string>{"apple", "banana", "cherry", "date"}));
    }

    std::cout << "All sort tests passed (" << g_checks << " checks).\n";
    return 0;
}
