// DisjointSet 单元测试（零依赖，基于 assert）
#include <ceh/structures/disjoint_set.hpp>

#include <iostream>

using ceh::DisjointSet;

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
    DisjointSet ds(6);

    // 初始：6 个独立集合
    CHECK_TRUE(ds.size() == 6);
    CHECK_TRUE(ds.count() == 6);
    CHECK_TRUE(!ds.connected(0, 1));
    CHECK_TRUE(ds.size_of(0) == 1);

    // 合并
    CHECK_TRUE(ds.unite(0, 1));
    CHECK_TRUE(ds.connected(0, 1));
    CHECK_TRUE(ds.count() == 5);
    CHECK_TRUE(ds.size_of(0) == 2 && ds.size_of(1) == 2);

    // 重复合并返回 false，集合数不变
    CHECK_TRUE(!ds.unite(0, 1));
    CHECK_TRUE(!ds.unite(1, 0));
    CHECK_TRUE(ds.count() == 5);

    // 链式合并：0-1-2-3 连成一个集合
    ds.unite(1, 2);
    ds.unite(2, 3);
    CHECK_TRUE(ds.connected(0, 3));
    CHECK_TRUE(ds.size_of(3) == 4);
    CHECK_TRUE(ds.count() == 3);          // {0,1,2,3} {4} {5}

    // 另一个集合
    CHECK_TRUE(ds.unite(4, 5));
    CHECK_TRUE(ds.connected(4, 5));
    CHECK_TRUE(!ds.connected(3, 4));
    CHECK_TRUE(ds.count() == 2);

    // 合并两个多元素集合
    CHECK_TRUE(ds.unite(3, 5));
    CHECK_TRUE(ds.connected(0, 4));
    CHECK_TRUE(ds.size_of(0) == 6);
    CHECK_TRUE(ds.count() == 1);          // 全部连通

    // find 的代表元一致性
    CHECK_TRUE(ds.find(0) == ds.find(5));

    // 越界
    CHECK_THROWS(std::out_of_range, ds.find(6));
    CHECK_THROWS(std::out_of_range, ds.unite(0, 100));

    // 单元素并查集
    {
        DisjointSet one(1);
        CHECK_TRUE(one.count() == 1);
        CHECK_TRUE(one.connected(0, 0));
        CHECK_TRUE(!one.unite(0, 0));
    }

    std::cout << "All DisjointSet tests passed (" << g_checks << " checks).\n";
    return 0;
}
