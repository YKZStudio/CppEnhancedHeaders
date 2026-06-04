// ceh/utils/string_utils.hpp
//
// 字符串实用函数（header-only，自由函数，均不修改入参、返回新串）。
//
// 补齐标准库在 C++17 下缺失或写起来啰嗦的常用操作：拆分、连接、修剪、
// 大小写转换、前后缀判断、包含、替换、重复。
//
// 异常：split / replace_all 的分隔串为空时抛 std::invalid_argument（避免歧义/死循环）。
//
// 用法：
//     ceh::split("a,b,,c", ',');                 // {"a","b","","c"}
//     ceh::join({"a","b","c"}, "-");              // "a-b-c"
//     ceh::trim("  hi \n");                       // "hi"
//     ceh::starts_with("filename.txt", "file");  // true

#ifndef CEH_UTILS_STRING_UTILS_HPP
#define CEH_UTILS_STRING_UTILS_HPP

#include <cctype>
#include <stdexcept>
#include <string>
#include <vector>

namespace ceh {

// 按单字符分隔；keep_empty=false 时丢弃空字段
inline std::vector<std::string> split(const std::string& s, char delim,
                                      bool keep_empty = true) {
    std::vector<std::string> out;
    std::string cur;
    for (char c : s) {
        if (c == delim) {
            if (keep_empty || !cur.empty()) out.push_back(cur);
            cur.clear();
        } else {
            cur.push_back(c);
        }
    }
    if (keep_empty || !cur.empty()) out.push_back(cur);
    return out;
}

// 按子串分隔（delim 不能为空）
inline std::vector<std::string> split(const std::string& s, const std::string& delim,
                                      bool keep_empty = true) {
    if (delim.empty())
        throw std::invalid_argument("ceh::split: delimiter must not be empty");
    std::vector<std::string> out;
    std::size_t start = 0;
    while (true) {
        std::size_t pos = s.find(delim, start);
        if (pos == std::string::npos) {
            std::string piece = s.substr(start);
            if (keep_empty || !piece.empty()) out.push_back(piece);
            break;
        }
        std::string piece = s.substr(start, pos - start);
        if (keep_empty || !piece.empty()) out.push_back(piece);
        start = pos + delim.size();
    }
    return out;
}

// 用 sep 连接
inline std::string join(const std::vector<std::string>& parts, const std::string& sep) {
    std::string out;
    for (std::size_t i = 0; i < parts.size(); ++i) {
        if (i != 0) out += sep;
        out += parts[i];
    }
    return out;
}

inline std::string ltrim(const std::string& s, const std::string& chars = " \t\n\r\f\v") {
    std::size_t b = s.find_first_not_of(chars);
    return b == std::string::npos ? std::string() : s.substr(b);
}

inline std::string rtrim(const std::string& s, const std::string& chars = " \t\n\r\f\v") {
    std::size_t e = s.find_last_not_of(chars);
    return e == std::string::npos ? std::string() : s.substr(0, e + 1);
}

inline std::string trim(const std::string& s, const std::string& chars = " \t\n\r\f\v") {
    return rtrim(ltrim(s, chars), chars);
}

inline std::string to_lower(std::string s) {
    for (char& c : s)
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}

inline std::string to_upper(std::string s) {
    for (char& c : s)
        c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return s;
}

inline bool starts_with(const std::string& s, const std::string& prefix) {
    return s.size() >= prefix.size() && s.compare(0, prefix.size(), prefix) == 0;
}

inline bool ends_with(const std::string& s, const std::string& suffix) {
    return s.size() >= suffix.size() &&
           s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
}

inline bool contains(const std::string& s, const std::string& sub) {
    return s.find(sub) != std::string::npos;
}

// 把 s 中所有 from 替换为 to（from 不能为空）
inline std::string replace_all(const std::string& s, const std::string& from,
                               const std::string& to) {
    if (from.empty())
        throw std::invalid_argument("ceh::replace_all: 'from' must not be empty");
    std::string out;
    std::size_t start = 0;
    while (true) {
        std::size_t pos = s.find(from, start);
        if (pos == std::string::npos) {
            out += s.substr(start);
            break;
        }
        out += s.substr(start, pos - start);
        out += to;
        start = pos + from.size();
    }
    return out;
}

inline std::string repeat(const std::string& s, std::size_t n) {
    std::string out;
    out.reserve(s.size() * n);
    for (std::size_t i = 0; i < n; ++i) out += s;
    return out;
}

}  // namespace ceh

#endif  // CEH_UTILS_STRING_UTILS_HPP
