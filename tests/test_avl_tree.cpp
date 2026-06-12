// AVLTree 单元测试（零依赖，基于 assert；用 std::set 作为参照）
#include <ceh/structures/avl_tree.hpp>

#include <cmath>
#include <functional>
#include <iostream>
#include <random>
#include <set>
#include <vector>

using ceh::AVLTree;

static int g_checks = 0;

#define CHECK_TRUE(expr)                                               \
    do { ++g_checks; if (!(expr)) {                                    \
        std::cerr << "FAIL [" << __LINE__ << "] " #expr "\n"; std::exit(1);} } while (0)

#define CHECK_THROWS(ExType, expr)                                     \
    do { ++g_checks; bool threw = false; try { (void)(expr); }        \
        catch (const ExType&) { threw = true; }                       \
        if (!threw) { std::cerr << "FAIL [" << __LINE__ << "] expected throw\n"; std::exit(1);} } while (0)

using IV = std::vector<int>;

int main() {
    AVLTree<int> t;

    // 空树
    CHECK_TRUE(t.empty() && t.size() == 0 && t.height() == 0);
    CHECK_TRUE(!t.contains(1));
    CHECK_THROWS(std::out_of_range, t.min());
    CHECK_THROWS(std::out_of_range, t.max());

    // 插入 + 有序 + 去重
    for (int v : {5, 3, 8, 1, 4, 7, 9, 2, 6}) CHECK_TRUE(t.insert(v));
    CHECK_TRUE(!t.insert(5));
    CHECK_TRUE(t.size() == 9);
    CHECK_TRUE(t.to_vector() == (IV{1, 2, 3, 4, 5, 6, 7, 8, 9}));
    CHECK_TRUE(t.min() == 1 && t.max() == 9);
    CHECK_TRUE(t.contains(6) && !t.contains(10));

    // 删除三种情形
    CHECK_TRUE(t.erase(1));                 // 叶子
    CHECK_TRUE(t.erase(8));                 // 含子树
    CHECK_TRUE(t.erase(5));                 // 根 / 两孩子
    CHECK_TRUE(!t.erase(5));
    CHECK_TRUE(t.to_vector() == (IV{2, 3, 4, 6, 7, 9}));

    // 自定义比较器（降序）
    {
        AVLTree<int, std::greater<int>> d;
        for (int v : {3, 1, 4, 1, 5, 9, 2, 6}) d.insert(v);
        CHECK_TRUE(d.to_vector() == (IV{9, 6, 5, 4, 3, 2, 1}));
    }

    // 拷贝 / 移动独立性
    {
        AVLTree<int> a;
        for (int v : {1, 2, 3}) a.insert(v);
        AVLTree<int> b = a;
        b.insert(99);
        CHECK_TRUE(a.to_vector() == (IV{1, 2, 3}) && b.contains(99) && !a.contains(99));
        AVLTree<int> m = std::move(a);
        CHECK_TRUE(m.to_vector() == (IV{1, 2, 3}));
    }

    // 顺序插入仍保持平衡：1..1000 升序插入，高度应 ~log2 而非退化
    {
        AVLTree<int> bal;
        for (int i = 1; i <= 1000; ++i) bal.insert(i);
        CHECK_TRUE(bal.size() == 1000);
        CHECK_TRUE(bal.height() <= 20);     // 退化将是 1000，平衡约 ≤ 15
    }

    // 大规模随机操作，对照 std::set
    {
        std::mt19937 rng(2024);
        std::set<int> ref;
        AVLTree<int> tr;
        for (int i = 0; i < 4000; ++i) {
            int x = static_cast<int>(rng() % 1000);
            CHECK_TRUE(tr.insert(x) == ref.insert(x).second);
        }
        for (int i = 0; i < 2000; ++i) {
            int x = static_cast<int>(rng() % 1000);
            CHECK_TRUE(tr.erase(x) == (ref.erase(x) > 0));
        }
        CHECK_TRUE(tr.size() == ref.size());
        CHECK_TRUE(tr.to_vector() == IV(ref.begin(), ref.end()));
    }

    std::cout << "All AVLTree tests passed (" << g_checks << " checks).\n";
    return 0;
}
