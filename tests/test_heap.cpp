// Heap 单元测试（零依赖，基于 assert）
#include <ceh/structures/heap.hpp>

#include <functional>
#include <iostream>
#include <string>
#include <vector>

using ceh::Heap;

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

int main() {
    // 空堆
    {
        Heap<int> h;
        CHECK_TRUE(h.empty());
        CHECK_TRUE(h.size() == 0);
        CHECK_THROWS(std::out_of_range, h.top());
        CHECK_THROWS(std::out_of_range, h.pop());
    }

    // 大顶堆（默认）：按降序弹出
    {
        Heap<int> h;
        for (int v : {3, 1, 4, 1, 5, 9, 2, 6}) h.push(v);
        CHECK_TRUE(h.size() == 8);
        CHECK_TRUE(h.top() == 9);
        std::vector<int> out;
        while (!h.empty()) { out.push_back(h.top()); h.pop(); }
        CHECK_TRUE((out == std::vector<int>{9, 6, 5, 4, 3, 2, 1, 1}));
    }

    // 小顶堆：按升序弹出
    {
        Heap<int, std::greater<int>> h;
        for (int v : {3, 1, 4, 1, 5, 9, 2, 6}) h.push(v);
        CHECK_TRUE(h.top() == 1);
        std::vector<int> out;
        while (!h.empty()) { out.push_back(h.top()); h.pop(); }
        CHECK_TRUE((out == std::vector<int>{1, 1, 2, 3, 4, 5, 6, 9}));
    }

    // 区间构造（建堆）
    {
        std::vector<int> data = {7, 2, 9, 4, 1, 8, 3};
        Heap<int> h(data.begin(), data.end());
        CHECK_TRUE(h.size() == 7);
        CHECK_TRUE(h.top() == 9);
        int prev = h.top();
        h.pop();
        while (!h.empty()) {                 // 验证整体有序弹出
            CHECK_TRUE(h.top() <= prev);
            prev = h.top();
            h.pop();
        }
    }

    // clear
    {
        Heap<int> h;
        h.push(1); h.push(2);
        h.clear();
        CHECK_TRUE(h.empty());
    }

    // 字符串 + 单元素
    {
        Heap<std::string> h;
        h.push("banana");
        h.push("apple");
        h.push("cherry");
        CHECK_TRUE(h.top() == "cherry");     // 字典序最大
        h.pop();
        CHECK_TRUE(h.top() == "banana");
    }

    std::cout << "All Heap tests passed (" << g_checks << " checks).\n";
    return 0;
}
