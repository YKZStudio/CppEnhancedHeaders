// ceh/algorithms/search.hpp
//
// 二分查找及其变体（header-only，自由函数）。
//
// 前三个作用于「已按 comp 升序排好」的 std::vector，返回下标（而非迭代器），
// 更贴合日常使用；最后一个是「二分答案」，标准库没有但极常用。
//
//   - binary_search_index   找到返回下标，否则返回 -1
//   - lower_bound_index     第一个「不小于 target」的下标（找不到返回 size）
//   - upper_bound_index     第一个「大于 target」的下标（找不到返回 size）
//   - binary_search_first_true  在整数区间 [lo, hi] 上找第一个使谓词为真的值
//                                （谓词需单调：false…false,true…true）
//
// 用法：
//     std::vector<int> v = {1, 3, 5, 7, 9};
//     ceh::binary_search_index(v, 5);     // 2
//     ceh::lower_bound_index(v, 4);        // 2（第一个 >= 4 的位置）
//     // 找满足 x*x >= 100 的最小 x：
//     ceh::binary_search_first_true(0, 1000, [](long long x){ return x*x >= 100; }); // 10

#ifndef CEH_ALGORITHMS_SEARCH_HPP
#define CEH_ALGORITHMS_SEARCH_HPP

#include <cstddef>
#include <functional>
#include <vector>

namespace ceh {

// 在升序 vector 中查找 target，返回其下标；找不到返回 -1
template <typename T, typename Compare = std::less<>>
std::ptrdiff_t binary_search_index(const std::vector<T>& v, const T& target,
                                   Compare comp = Compare{}) {
    std::ptrdiff_t lo = 0;
    std::ptrdiff_t hi = static_cast<std::ptrdiff_t>(v.size()) - 1;
    while (lo <= hi) {
        std::ptrdiff_t mid = lo + (hi - lo) / 2;
        if (comp(v[static_cast<std::size_t>(mid)], target)) {
            lo = mid + 1;
        } else if (comp(target, v[static_cast<std::size_t>(mid)])) {
            hi = mid - 1;
        } else {
            return mid;
        }
    }
    return -1;
}

// 第一个「不小于 target」（即 !comp(elem, target)）的下标；都更小则返回 size
template <typename T, typename Compare = std::less<>>
std::size_t lower_bound_index(const std::vector<T>& v, const T& target,
                              Compare comp = Compare{}) {
    std::size_t lo = 0, hi = v.size();
    while (lo < hi) {
        std::size_t mid = lo + (hi - lo) / 2;
        if (comp(v[mid], target)) lo = mid + 1;
        else                      hi = mid;
    }
    return lo;
}

// 第一个「大于 target」（即 comp(target, elem)）的下标；都不大于则返回 size
template <typename T, typename Compare = std::less<>>
std::size_t upper_bound_index(const std::vector<T>& v, const T& target,
                              Compare comp = Compare{}) {
    std::size_t lo = 0, hi = v.size();
    while (lo < hi) {
        std::size_t mid = lo + (hi - lo) / 2;
        if (comp(target, v[mid])) hi = mid;
        else                      lo = mid + 1;
    }
    return lo;
}

// 二分答案：在 [lo, hi] 上找第一个使 pred(x) 为真的 x（pred 关于 x 单调）。
// 若区间内无解，返回 hi + 1。
template <typename Pred>
long long binary_search_first_true(long long lo, long long hi, Pred pred) {
    long long ans = hi + 1;
    while (lo <= hi) {
        long long mid = lo + (hi - lo) / 2;
        if (pred(mid)) {
            ans = mid;
            hi = mid - 1;
        } else {
            lo = mid + 1;
        }
    }
    return ans;
}

}  // namespace ceh

#endif  // CEH_ALGORITHMS_SEARCH_HPP
