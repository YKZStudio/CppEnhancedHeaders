// StringUtils 单元测试（零依赖，基于 assert）
#include <ceh/utils/string_utils.hpp>

#include <iostream>
#include <string>
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

#define CHECK_THROWS(ExType, expr)                                     \
    do {                                                               \
        ++g_checks;                                                    \
        bool threw = false;                                            \
        try { (void)(expr); }                                         \
        catch (const ExType&) { threw = true; }                       \
        if (!threw) {                                                  \
            std::cerr << "FAIL [" << __LINE__ << "] expected throw: "  \
                      << #expr "\n";                                   \
            std::exit(1);                                              \
        }                                                              \
    } while (0)

using V = std::vector<std::string>;

int main() {
    // split（字符分隔）
    CHECK_TRUE((ceh::split("a,b,c", ',') == V{"a", "b", "c"}));
    CHECK_TRUE((ceh::split("a,b,,c", ',') == V{"a", "b", "", "c"}));      // 保留空字段
    CHECK_TRUE((ceh::split("a,b,,c", ',', false) == V{"a", "b", "c"}));   // 丢弃空字段
    CHECK_TRUE((ceh::split("", ',') == V{""}));
    CHECK_TRUE((ceh::split("nodelim", ',') == V{"nodelim"}));
    CHECK_TRUE((ceh::split(",", ',') == V{"", ""}));

    // split（子串分隔）
    CHECK_TRUE((ceh::split("a::b::c", "::") == V{"a", "b", "c"}));
    CHECK_TRUE((ceh::split("a->b", "->") == V{"a", "b"}));
    CHECK_THROWS(std::invalid_argument, ceh::split("abc", std::string("")));

    // join
    CHECK_TRUE(ceh::join({"a", "b", "c"}, "-") == "a-b-c");
    CHECK_TRUE(ceh::join({"x"}, ",") == "x");
    CHECK_TRUE(ceh::join({}, ",") == "");
    // split 与 join 互逆（无空字段时）
    CHECK_TRUE(ceh::join(ceh::split("1,2,3", ','), ",") == "1,2,3");

    // trim 系列
    CHECK_TRUE(ceh::trim("  hello  ") == "hello");
    CHECK_TRUE(ceh::trim("\t\n hi \r\n") == "hi");
    CHECK_TRUE(ceh::ltrim("  hi  ") == "hi  ");
    CHECK_TRUE(ceh::rtrim("  hi  ") == "  hi");
    CHECK_TRUE(ceh::trim("") == "");
    CHECK_TRUE(ceh::trim("    ") == "");
    CHECK_TRUE(ceh::trim("xxhixx", "x") == "hi");   // 自定义修剪字符

    // 大小写
    CHECK_TRUE(ceh::to_lower("Hello World 123") == "hello world 123");
    CHECK_TRUE(ceh::to_upper("Hello World 123") == "HELLO WORLD 123");

    // 前后缀 / 包含
    CHECK_TRUE(ceh::starts_with("filename.txt", "file"));
    CHECK_TRUE(!ceh::starts_with("filename.txt", "name"));
    CHECK_TRUE(ceh::starts_with("abc", ""));        // 空前缀恒真
    CHECK_TRUE(!ceh::starts_with("ab", "abc"));     // 前缀比串长
    CHECK_TRUE(ceh::ends_with("filename.txt", ".txt"));
    CHECK_TRUE(!ceh::ends_with("filename.txt", ".md"));
    CHECK_TRUE(ceh::contains("hello world", "o w"));
    CHECK_TRUE(!ceh::contains("hello", "xyz"));

    // replace_all
    CHECK_TRUE(ceh::replace_all("a.b.c", ".", "/") == "a/b/c");
    CHECK_TRUE(ceh::replace_all("aaa", "a", "bb") == "bbbbbb");
    CHECK_TRUE(ceh::replace_all("no match", "x", "y") == "no match");
    CHECK_TRUE(ceh::replace_all("ababab", "ab", "X") == "XXX");
    CHECK_THROWS(std::invalid_argument, ceh::replace_all("abc", "", "x"));

    // repeat
    CHECK_TRUE(ceh::repeat("ab", 3) == "ababab");
    CHECK_TRUE(ceh::repeat("x", 0) == "");

    std::cout << "All StringUtils tests passed (" << g_checks << " checks).\n";
    return 0;
}
