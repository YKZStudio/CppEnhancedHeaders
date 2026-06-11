// string 算法使用示例
#include <ceh/algorithms/string.hpp>
#include <iostream>

int main() {
    std::string text = "ababcabababc";
    std::cout << "在 \"" << text << "\" 中查找 \"abab\": ";
    for (std::size_t pos : ceh::kmp_search(text, "abab")) std::cout << pos << ' ';
    std::cout << "\n";

    std::cout << "最长公共子串(abcde, zbcdf) = "
              << ceh::longest_common_substring("abcde", "zbcdf") << "\n";
    std::cout << "最长公共子序列(AGGTAB, GXTXAYB) = "
              << ceh::longest_common_subsequence("AGGTAB", "GXTXAYB") << "\n";
    return 0;
}
