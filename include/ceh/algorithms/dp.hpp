// ceh/algorithms/dp.hpp
//
// 常见动态规划模板（header-only，自由函数），位于 ceh::。
//
// 提供：最长递增子序列（长度 O(n log n) 与实际序列）、编辑距离（Levenshtein）、
//        0/1 背包、硬币找零（最少硬币数）。
//
// 用法：
//     ceh::longest_increasing_subsequence_length({10,9,2,5,3,7,101,18}); // 4
//     ceh::edit_distance("kitten", "sitting");                            // 3
//     ceh::knapsack_01({2,3,4,5}, {3,4,5,6}, 5);                          // 7
//     ceh::coin_change({1,2,5}, 11);                                      // 3

#ifndef CEH_ALGORITHMS_DP_HPP
#define CEH_ALGORITHMS_DP_HPP

#include <algorithm>
#include <cstddef>
#include <string>
#include <vector>

namespace ceh {

// 最长严格递增子序列的长度，O(n log n)（耐心排序）
template <typename T>
std::size_t longest_increasing_subsequence_length(const std::vector<T>& v) {
    std::vector<T> tails;   // tails[k] = 长度为 k+1 的递增子序列的最小末尾
    for (const T& x : v) {
        auto it = std::lower_bound(tails.begin(), tails.end(), x);
        if (it == tails.end()) tails.push_back(x);
        else *it = x;
    }
    return tails.size();
}

// 返回一个最长严格递增子序列（实际元素），O(n log n)
template <typename T>
std::vector<T> longest_increasing_subsequence(const std::vector<T>& v) {
    std::size_t n = v.size();
    if (n == 0) return {};
    std::vector<T> tails;
    std::vector<std::size_t> tail_idx;          // tails[k] 对应的原下标
    std::vector<std::size_t> prev(n, n);        // 前驱下标，用于回溯
    for (std::size_t i = 0; i < n; ++i) {
        auto it = std::lower_bound(tails.begin(), tails.end(), v[i]);
        std::size_t pos = static_cast<std::size_t>(it - tails.begin());
        if (it == tails.end()) { tails.push_back(v[i]); tail_idx.push_back(i); }
        else { *it = v[i]; tail_idx[pos] = i; }
        prev[i] = (pos > 0) ? tail_idx[pos - 1] : n;
    }
    std::vector<T> out;
    for (std::size_t i = tail_idx.back(); i != n; i = prev[i]) out.push_back(v[i]);
    std::reverse(out.begin(), out.end());
    return out;
}

// 编辑距离（Levenshtein）：插入/删除/替换各计 1
inline std::size_t edit_distance(const std::string& a, const std::string& b) {
    std::size_t n = a.size(), m = b.size();
    std::vector<std::size_t> dp(m + 1);
    for (std::size_t j = 0; j <= m; ++j) dp[j] = j;
    for (std::size_t i = 1; i <= n; ++i) {
        std::size_t prev = dp[0];               // dp[i-1][j-1]
        dp[0] = i;
        for (std::size_t j = 1; j <= m; ++j) {
            std::size_t cur = dp[j];            // 暂存 dp[i-1][j]
            if (a[i - 1] == b[j - 1]) dp[j] = prev;
            else dp[j] = 1 + std::min({prev, dp[j], dp[j - 1]});
            prev = cur;
        }
    }
    return dp[m];
}

// 0/1 背包：在容量 capacity 内选若干物品使总价值最大。weights 与 values 等长。
inline long long knapsack_01(const std::vector<int>& weights,
                             const std::vector<long long>& values, int capacity) {
    if (capacity <= 0) return 0;
    std::vector<long long> dp(static_cast<std::size_t>(capacity) + 1, 0);
    for (std::size_t i = 0; i < weights.size(); ++i) {
        int w = weights[i];
        long long val = (i < values.size()) ? values[i] : 0;
        if (w < 0) continue;
        for (int c = capacity; c >= w; --c)     // 逆序保证每件只用一次
            dp[static_cast<std::size_t>(c)] =
                std::max(dp[static_cast<std::size_t>(c)],
                         dp[static_cast<std::size_t>(c - w)] + val);
    }
    return dp[static_cast<std::size_t>(capacity)];
}

// 硬币找零：凑出 amount 所需的最少硬币数（硬币可无限使用）；无法凑出返回 -1
inline int coin_change(const std::vector<int>& coins, int amount) {
    if (amount < 0) return -1;
    const int kInf = amount + 1;
    std::vector<int> dp(static_cast<std::size_t>(amount) + 1, kInf);
    dp[0] = 0;
    for (int a = 1; a <= amount; ++a)
        for (int c : coins)
            if (c > 0 && c <= a)
                dp[static_cast<std::size_t>(a)] =
                    std::min(dp[static_cast<std::size_t>(a)],
                             dp[static_cast<std::size_t>(a - c)] + 1);
    return dp[static_cast<std::size_t>(amount)] >= kInf ? -1 : dp[static_cast<std::size_t>(amount)];
}

}  // namespace ceh

#endif  // CEH_ALGORITHMS_DP_HPP
