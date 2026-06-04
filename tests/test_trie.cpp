// Trie 单元测试（零依赖，基于 assert）
#include <ceh/structures/trie.hpp>

#include <iostream>
#include <string>

using ceh::Trie;

static int g_checks = 0;

#define CHECK_TRUE(expr)                                               \
    do {                                                               \
        ++g_checks;                                                    \
        if (!(expr)) {                                                 \
            std::cerr << "FAIL [" << __LINE__ << "] " #expr "\n";      \
            std::exit(1);                                              \
        }                                                              \
    } while (0)

int main() {
    Trie t;

    // 空树
    CHECK_TRUE(t.empty());
    CHECK_TRUE(t.size() == 0);
    CHECK_TRUE(!t.contains("anything"));
    CHECK_TRUE(!t.starts_with("a"));
    CHECK_TRUE(t.count_prefix("a") == 0);

    // 插入
    CHECK_TRUE(t.insert("apple"));
    CHECK_TRUE(t.insert("app"));
    CHECK_TRUE(t.insert("application"));
    CHECK_TRUE(t.insert("banana"));
    CHECK_TRUE(t.size() == 4);
    CHECK_TRUE(!t.insert("apple"));      // 重复插入
    CHECK_TRUE(t.size() == 4);

    // 精确查找：前缀不是词
    CHECK_TRUE(t.contains("apple"));
    CHECK_TRUE(t.contains("app"));
    CHECK_TRUE(!t.contains("appl"));     // 是前缀但不是已插入的词
    CHECK_TRUE(!t.contains("ban"));

    // 前缀查询
    CHECK_TRUE(t.starts_with("app"));
    CHECK_TRUE(t.starts_with("appl"));
    CHECK_TRUE(!t.starts_with("xyz"));
    CHECK_TRUE(t.count_prefix("app") == 3);   // apple, app, application
    CHECK_TRUE(t.count_prefix("appl") == 2);  // apple, application
    CHECK_TRUE(t.count_prefix("b") == 1);
    CHECK_TRUE(t.count_prefix("") == 4);      // 空前缀 = 全部词数

    // 删除：词存在
    CHECK_TRUE(t.erase("app"));
    CHECK_TRUE(!t.contains("app"));
    CHECK_TRUE(t.contains("apple"));          // 不影响其它词
    CHECK_TRUE(t.size() == 3);
    CHECK_TRUE(t.count_prefix("app") == 2);   // 现在 apple, application

    // 删除不存在的词
    CHECK_TRUE(!t.erase("app"));              // 已删
    CHECK_TRUE(!t.erase("nope"));
    CHECK_TRUE(t.size() == 3);

    // 删除后分支回收：删掉所有 app* 词，前缀应彻底消失
    CHECK_TRUE(t.erase("apple"));
    CHECK_TRUE(t.erase("application"));
    CHECK_TRUE(!t.starts_with("app"));
    CHECK_TRUE(!t.starts_with("a"));
    CHECK_TRUE(t.count_prefix("a") == 0);
    CHECK_TRUE(t.size() == 1);                // 只剩 banana
    CHECK_TRUE(t.contains("banana"));

    // 重新插入此前删空的分支，应正常工作
    CHECK_TRUE(t.insert("apple"));
    CHECK_TRUE(t.contains("apple"));
    CHECK_TRUE(t.starts_with("app"));

    // 空字符串作为一个词
    {
        Trie e;
        CHECK_TRUE(e.insert(""));
        CHECK_TRUE(e.contains(""));
        CHECK_TRUE(e.size() == 1);
        CHECK_TRUE(e.erase(""));
        CHECK_TRUE(!e.contains(""));
    }

    std::cout << "All Trie tests passed (" << g_checks << " checks).\n";
    return 0;
}
