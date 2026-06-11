// string 算法单元测试（零依赖，基于 assert）
#include <ceh/algorithms/string.hpp>

#include <iostream>
#include <string>
#include <vector>

static int g_checks = 0;

#define CHECK_TRUE(expr)                                               \
    do { ++g_checks; if (!(expr)) {                                    \
        std::cerr << "FAIL [" << __LINE__ << "] " #expr "\n"; std::exit(1);} } while (0)

using std::string;
using std::vector;
using SizeVec = std::vector<std::size_t>;
using IntVec = std::vector<int>;

int main() {
    // prefix_function
    CHECK_TRUE(ceh::prefix_function("aabaaab") == (IntVec{0, 1, 0, 1, 2, 2, 3}));
    CHECK_TRUE(ceh::prefix_function("abcabc") == (IntVec{0, 0, 0, 1, 2, 3}));
    CHECK_TRUE(ceh::prefix_function("") == IntVec{});

    // z_function
    CHECK_TRUE(ceh::z_function("aaaaa") == (IntVec{0, 4, 3, 2, 1}));
    CHECK_TRUE(ceh::z_function("abacaba") == (IntVec{0, 0, 1, 0, 3, 0, 1}));

    // kmp_search
    CHECK_TRUE(ceh::kmp_search("ababab", "ab") == (SizeVec{0, 2, 4}));
    CHECK_TRUE(ceh::kmp_search("aaaa", "aa") == (SizeVec{0, 1, 2}));   // 可重叠
    CHECK_TRUE(ceh::kmp_search("hello world", "o") == (SizeVec{4, 7}));
    CHECK_TRUE(ceh::kmp_search("abc", "x").empty());
    CHECK_TRUE(ceh::kmp_search("abc", "").empty());                   // 空模式
    CHECK_TRUE(ceh::kmp_search("ab", "abc").empty());                 // 模式更长
    CHECK_TRUE(ceh::kmp_search("aaa", "aaa") == (SizeVec{0}));

    // longest_common_substring（连续）
    CHECK_TRUE(ceh::longest_common_substring("abcde", "zbcdf") == "bcd");
    CHECK_TRUE(ceh::longest_common_substring("abcabc", "bca") == "bca");
    CHECK_TRUE(ceh::longest_common_substring("abc", "xyz") == "");
    CHECK_TRUE(ceh::longest_common_substring("", "abc") == "");

    // longest_common_subsequence（可不连续）
    CHECK_TRUE(ceh::longest_common_subsequence("abcde", "ace") == "ace");
    CHECK_TRUE(ceh::longest_common_subsequence("AGGTAB", "GXTXAYB") == "GTAB");
    CHECK_TRUE(ceh::longest_common_subsequence("abc", "xyz") == "");
    CHECK_TRUE(ceh::longest_common_subsequence("same", "same") == "same");

    std::cout << "All string algo tests passed (" << g_checks << " checks).\n";
    return 0;
}
