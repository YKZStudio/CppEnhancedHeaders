// ceh/algorithms/string.hpp
//
// 字符串算法（header-only，自由函数），位于 ceh::。
//
// 提供：前缀函数（KMP 失配数组）、Z 函数、KMP 多模式匹配、最长公共子串、
//        最长公共子序列。
//
// 用法：
//     ceh::kmp_search("ababab", "ab");          // {0, 2, 4}
//     ceh::longest_common_substring("abcde", "zbcdf");  // "bcd"
//     ceh::longest_common_subsequence("abcde", "ace");  // "ace"

#ifndef CEH_ALGORITHMS_STRING_HPP
#define CEH_ALGORITHMS_STRING_HPP

#include <algorithm>
#include <cstddef>
#include <string>
#include <vector>

namespace ceh {

// 前缀函数 pi：pi[i] = s[0..i] 的「真前缀也是后缀」的最大长度（KMP 核心）
inline std::vector<int> prefix_function(const std::string& s) {
    std::size_t n = s.size();
    std::vector<int> pi(n, 0);
    for (std::size_t i = 1; i < n; ++i) {
        int j = pi[i - 1];
        while (j > 0 && s[i] != s[static_cast<std::size_t>(j)]) j = pi[static_cast<std::size_t>(j) - 1];
        if (s[i] == s[static_cast<std::size_t>(j)]) ++j;
        pi[i] = j;
    }
    return pi;
}

// Z 函数：z[i] = 从 i 开始与 s 的最长公共前缀长度（z[0] 约定为 0）
inline std::vector<int> z_function(const std::string& s) {
    std::size_t n = s.size();
    std::vector<int> z(n, 0);
    std::size_t l = 0, r = 0;
    for (std::size_t i = 1; i < n; ++i) {
        if (i < r) z[i] = std::min(static_cast<int>(r - i), z[i - l]);
        while (i + static_cast<std::size_t>(z[i]) < n &&
               s[static_cast<std::size_t>(z[i])] == s[i + static_cast<std::size_t>(z[i])])
            ++z[i];
        if (i + static_cast<std::size_t>(z[i]) > r) { l = i; r = i + static_cast<std::size_t>(z[i]); }
    }
    return z;
}

// KMP：返回 pattern 在 text 中所有出现的起始下标（可重叠）。pattern 为空时返回空。
inline std::vector<std::size_t> kmp_search(const std::string& text, const std::string& pattern) {
    std::vector<std::size_t> result;
    if (pattern.empty() || pattern.size() > text.size()) return result;
    std::vector<int> pi = prefix_function(pattern);
    int j = 0;
    for (std::size_t i = 0; i < text.size(); ++i) {
        while (j > 0 && text[i] != pattern[static_cast<std::size_t>(j)])
            j = pi[static_cast<std::size_t>(j) - 1];
        if (text[i] == pattern[static_cast<std::size_t>(j)]) ++j;
        if (static_cast<std::size_t>(j) == pattern.size()) {
            result.push_back(i + 1 - pattern.size());
            j = pi[static_cast<std::size_t>(j) - 1];
        }
    }
    return result;
}

// 最长公共子串（连续）。多个并列时返回最先出现的那个。
inline std::string longest_common_substring(const std::string& a, const std::string& b) {
    if (a.empty() || b.empty()) return std::string();
    std::vector<std::vector<int>> dp(a.size() + 1, std::vector<int>(b.size() + 1, 0));
    int best = 0;
    std::size_t best_end = 0;   // 在 a 中的结束位置（不含）
    for (std::size_t i = 1; i <= a.size(); ++i) {
        for (std::size_t j = 1; j <= b.size(); ++j) {
            if (a[i - 1] == b[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1;
                if (dp[i][j] > best) { best = dp[i][j]; best_end = i; }
            }
        }
    }
    return a.substr(best_end - static_cast<std::size_t>(best), static_cast<std::size_t>(best));
}

// 最长公共子序列（可不连续），返回其中一个 LCS。
inline std::string longest_common_subsequence(const std::string& a, const std::string& b) {
    std::size_t n = a.size(), m = b.size();
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1, 0));
    for (std::size_t i = 1; i <= n; ++i)
        for (std::size_t j = 1; j <= m; ++j)
            dp[i][j] = (a[i - 1] == b[j - 1]) ? dp[i - 1][j - 1] + 1
                                              : std::max(dp[i - 1][j], dp[i][j - 1]);
    // 回溯重建
    std::string out;
    std::size_t i = n, j = m;
    while (i > 0 && j > 0) {
        if (a[i - 1] == b[j - 1]) { out.push_back(a[i - 1]); --i; --j; }
        else if (dp[i - 1][j] >= dp[i][j - 1]) --i;
        else --j;
    }
    std::reverse(out.begin(), out.end());
    return out;
}

}  // namespace ceh

#endif  // CEH_ALGORITHMS_STRING_HPP
