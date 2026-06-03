// BinarySearchTree 单元测试（零依赖，基于 assert）
#include <ceh/structures/binary_search_tree.hpp>

#include <functional>
#include <iostream>
#include <string>
#include <vector>

using ceh::BinarySearchTree;

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

template <typename V>
static bool same(const std::vector<V>& a, const std::vector<V>& b) {
    return a == b;
}

int main() {
    BinarySearchTree<int> t;

    // 空树
    CHECK_TRUE(t.empty());
    CHECK_TRUE(t.size() == 0);
    CHECK_TRUE(t.height() == -1);
    CHECK_TRUE(!t.contains(1));
    CHECK_THROWS(std::out_of_range, t.min());
    CHECK_THROWS(std::out_of_range, t.max());

    // 插入
    for (int v : {5, 3, 8, 1, 4, 7, 9}) CHECK_TRUE(t.insert(v));
    CHECK_TRUE(t.size() == 7);
    CHECK_TRUE(!t.empty());
    CHECK_TRUE(t.insert(5) == false);   // 重复插入
    CHECK_TRUE(t.size() == 7);

    // 查询
    CHECK_TRUE(t.contains(7));
    CHECK_TRUE(!t.contains(6));
    CHECK_TRUE(t.min() == 1);
    CHECK_TRUE(t.max() == 9);

    // 中序遍历有序
    CHECK_TRUE(same(t.to_vector(), std::vector<int>{1, 3, 4, 5, 7, 8, 9}));

    // 前序 / 后序（按当前插入形成的树形）
    {
        std::vector<int> pre;
        t.preorder([&pre](int v) { pre.push_back(v); });
        CHECK_TRUE(same(pre, std::vector<int>{5, 3, 1, 4, 8, 7, 9}));

        std::vector<int> post;
        t.postorder([&post](int v) { post.push_back(v); });
        CHECK_TRUE(same(post, std::vector<int>{1, 4, 3, 7, 9, 8, 5}));
    }

    // 高度：这棵树根 5，两侧各两层 -> 高度 2
    CHECK_TRUE(t.height() == 2);

    // 删除：叶子
    CHECK_TRUE(t.erase(1));
    CHECK_TRUE(!t.contains(1));
    CHECK_TRUE(t.size() == 6);
    CHECK_TRUE(t.min() == 3);

    // 删除：只有一个孩子（3 现在只有右孩子 4）
    CHECK_TRUE(t.erase(3));
    CHECK_TRUE(same(t.to_vector(), std::vector<int>{4, 5, 7, 8, 9}));

    // 删除：两个孩子（删根 5，应被中序后继 7 替换）
    CHECK_TRUE(t.erase(5));
    CHECK_TRUE(!t.contains(5));
    CHECK_TRUE(same(t.to_vector(), std::vector<int>{4, 7, 8, 9}));

    // 删除不存在的键
    CHECK_TRUE(t.erase(100) == false);
    CHECK_TRUE(t.size() == 4);

    // 深拷贝独立性
    {
        BinarySearchTree<int> copy = t;            // 拷贝构造
        copy.insert(42);
        CHECK_TRUE(copy.contains(42));
        CHECK_TRUE(!t.contains(42));               // 原树不受影响
        CHECK_TRUE(copy.size() == t.size() + 1);

        BinarySearchTree<int> assigned;
        assigned = t;                              // 拷贝赋值
        CHECK_TRUE(same(assigned.to_vector(), t.to_vector()));
        assigned.clear();
        CHECK_TRUE(assigned.empty());
        CHECK_TRUE(!t.empty());                    // 原树不受影响
    }

    // 移动
    {
        BinarySearchTree<int> src;
        for (int v : {2, 1, 3}) src.insert(v);
        BinarySearchTree<int> moved = std::move(src);
        CHECK_TRUE(same(moved.to_vector(), std::vector<int>{1, 2, 3}));
    }

    // 单节点删除
    {
        BinarySearchTree<int> one;
        one.insert(42);
        CHECK_TRUE(one.erase(42));
        CHECK_TRUE(one.empty());
        CHECK_TRUE(one.height() == -1);
    }

    // 自定义比较器：降序
    {
        BinarySearchTree<int, std::greater<int>> desc;
        for (int v : {5, 1, 3, 9, 7}) desc.insert(v);
        CHECK_TRUE(same(desc.to_vector(), std::vector<int>{9, 7, 5, 3, 1}));
        CHECK_TRUE(desc.min() == 9);   // “最小”是比较器意义下最靠前的
        CHECK_TRUE(desc.max() == 1);
    }

    // 字符串类型
    {
        BinarySearchTree<std::string> st;
        for (auto s : {"banana", "apple", "cherry"}) st.insert(s);
        CHECK_TRUE(same(st.to_vector(),
                        std::vector<std::string>{"apple", "banana", "cherry"}));
    }

    std::cout << "All BinarySearchTree tests passed (" << g_checks << " checks).\n";
    return 0;
}
