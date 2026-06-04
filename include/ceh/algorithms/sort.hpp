// ceh/algorithms/sort.hpp
//
// 排序算法合集（header-only，自由函数模板）。
//
// 提供四种排序，均作用于随机访问迭代器区间 [first, last)，并接受可选比较器
// （默认 std::less<>，升序）：
//   - insertion_sort  稳定，O(n^2)，小数组快
//   - merge_sort      稳定，O(n log n)，需 O(n) 额外空间
//   - quick_sort      原地，平均 O(n log n)（三数取中选基准 + 小区间插入排序）
//   - heap_sort       原地，O(n log n)，最坏也有保证
//
// 用法：
//     std::vector<int> v = {5, 2, 8, 1};
//     ceh::merge_sort(v.begin(), v.end());                  // 升序
//     ceh::quick_sort(v.begin(), v.end(), std::greater<>{}); // 降序

#ifndef CEH_ALGORITHMS_SORT_HPP
#define CEH_ALGORITHMS_SORT_HPP

#include <algorithm>
#include <functional>
#include <iterator>
#include <utility>
#include <vector>

namespace ceh {

// ---- 插入排序（稳定）----
template <typename RandomIt, typename Compare = std::less<>>
void insertion_sort(RandomIt first, RandomIt last, Compare comp = Compare{}) {
    for (RandomIt i = first; i != last; ++i) {
        auto key = std::move(*i);
        RandomIt j = i;
        while (j != first && comp(key, *std::prev(j))) {
            *j = std::move(*std::prev(j));
            --j;
        }
        *j = std::move(key);
    }
}

namespace detail {
template <typename RandomIt, typename Compare, typename T>
void merge_sort_rec(RandomIt first, RandomIt last, Compare comp, std::vector<T>& buf) {
    auto n = last - first;
    if (n < 2) return;
    RandomIt mid = first + n / 2;
    merge_sort_rec(first, mid, comp, buf);
    merge_sort_rec(mid, last, comp, buf);
    if (!comp(*mid, *std::prev(mid))) return;   // 已有序，省去归并
    buf.clear();
    std::merge(first, mid, mid, last, std::back_inserter(buf), comp);  // 稳定
    std::move(buf.begin(), buf.end(), first);
}
}  // namespace detail

// ---- 归并排序（稳定）----
template <typename RandomIt, typename Compare = std::less<>>
void merge_sort(RandomIt first, RandomIt last, Compare comp = Compare{}) {
    using T = typename std::iterator_traits<RandomIt>::value_type;
    std::vector<T> buf;
    if (last - first > 1) buf.reserve(static_cast<std::size_t>(last - first));
    detail::merge_sort_rec(first, last, comp, buf);
}

// ---- 快速排序（三数取中基准 + 小区间插入排序）----
template <typename RandomIt, typename Compare = std::less<>>
void quick_sort(RandomIt first, RandomIt last, Compare comp = Compare{}) {
    auto n = last - first;
    if (n < 2) return;
    if (n <= 16) {
        // 小区间插入排序更快，同时充当快排递归的基线，避免对短段过度递归
        insertion_sort(first, last, comp);
        return;
    }

    // 三数取中：让 first / mid / (last-1) 三者有序，中位数落在 mid
    RandomIt mid = first + n / 2;
    RandomIt lastel = last - 1;
    if (comp(*mid, *first))    std::iter_swap(mid, first);
    if (comp(*lastel, *first)) std::iter_swap(lastel, first);
    if (comp(*lastel, *mid))   std::iter_swap(lastel, mid);
    // 把中位数移到末尾作为基准
    std::iter_swap(mid, lastel);
    auto pivot = *lastel;

    // Lomuto 划分：扫描 [first, last-1)，小于基准的换到左侧
    RandomIt store = first;
    for (RandomIt it = first; it != lastel; ++it) {
        if (comp(*it, pivot)) {
            std::iter_swap(it, store);
            ++store;
        }
    }
    std::iter_swap(store, lastel);   // 基准归位

    quick_sort(first, store, comp);
    quick_sort(store + 1, last, comp);
}

// ---- 堆排序（原地）----
template <typename RandomIt, typename Compare = std::less<>>
void heap_sort(RandomIt first, RandomIt last, Compare comp = Compare{}) {
    using Diff = typename std::iterator_traits<RandomIt>::difference_type;
    Diff n = last - first;

    // 自 start 向下调整，使以 start 为根的子树满足大顶堆
    auto sift_down = [&](Diff start, Diff size) {
        Diff root = start;
        while (true) {
            Diff child = 2 * root + 1;
            if (child >= size) break;
            if (child + 1 < size && comp(first[child], first[child + 1])) ++child;
            if (comp(first[root], first[child])) {
                std::iter_swap(first + root, first + child);
                root = child;
            } else {
                break;
            }
        }
    };

    for (Diff i = n / 2; i > 0;) {     // 建堆
        --i;
        sift_down(i, n);
    }
    for (Diff i = n; i > 1;) {         // 逐个把堆顶换到末尾
        --i;
        std::iter_swap(first, first + i);
        sift_down(0, i);
    }
}

}  // namespace ceh

#endif  // CEH_ALGORITHMS_SORT_HPP
