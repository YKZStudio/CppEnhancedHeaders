// SkipList 单元测试（零依赖，基于 assert）
#include <ceh/structures/skip_list.hpp>

#include <functional>
#include <iostream>
#include <vector>

using ceh::SkipList;

static int g_checks = 0;

#define CHECK_TRUE(expr)                                               \
    do { ++g_checks; if (!(expr)) {                                    \
        std::cerr << "FAIL [" << __LINE__ << "] " #expr "\n"; std::exit(1);} } while (0)

using IV = std::vector<int>;

int main() {
    SkipList<int> s(12345);   // 固定种子，结果与随机层数无关（始终有序集合）

    // 空
    CHECK_TRUE(s.empty() && s.size() == 0);
    CHECK_TRUE(!s.contains(1));

    // 乱序插入 -> 有序输出
    for (int v : {5, 2, 8, 1, 9, 3, 7, 4, 6, 0}) CHECK_TRUE(s.insert(v));
    CHECK_TRUE(s.size() == 10);
    CHECK_TRUE(s.to_vector() == (IV{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));

    // 重复插入返回 false
    CHECK_TRUE(!s.insert(5));
    CHECK_TRUE(s.size() == 10);

    // 查找
    CHECK_TRUE(s.contains(0) && s.contains(9) && s.contains(5));
    CHECK_TRUE(!s.contains(10) && !s.contains(-1));

    // 删除
    CHECK_TRUE(s.erase(0));        // 删最小
    CHECK_TRUE(s.erase(9));        // 删最大
    CHECK_TRUE(s.erase(5));        // 删中间
    CHECK_TRUE(!s.contains(5));
    CHECK_TRUE(s.to_vector() == (IV{1, 2, 3, 4, 6, 7, 8}));
    CHECK_TRUE(s.size() == 7);

    // 删除不存在
    CHECK_TRUE(!s.erase(5));
    CHECK_TRUE(!s.erase(100));

    // 删空再用
    for (int v : {1, 2, 3, 4, 6, 7, 8}) CHECK_TRUE(s.erase(v));
    CHECK_TRUE(s.empty());
    CHECK_TRUE(s.insert(42));
    CHECK_TRUE(s.contains(42));

    // 自定义比较器：降序
    {
        SkipList<int, std::greater<int>> desc(7);
        for (int v : {3, 1, 4, 1, 5, 9, 2, 6}) desc.insert(v);
        CHECK_TRUE(desc.to_vector() == (IV{9, 6, 5, 4, 3, 2, 1}));
    }

    // 拷贝独立性
    {
        SkipList<int> a(1);
        for (int v : {3, 1, 2}) a.insert(v);
        SkipList<int> b = a;
        b.insert(99);
        CHECK_TRUE(a.to_vector() == (IV{1, 2, 3}));
        CHECK_TRUE(b.contains(99) && !a.contains(99));
    }

    // 移动
    {
        SkipList<int> src(2);
        src.insert(1); src.insert(2);
        SkipList<int> moved = std::move(src);
        CHECK_TRUE(moved.to_vector() == (IV{1, 2}));
    }

    // 较大规模有序性
    {
        SkipList<int> big(99);
        for (int i = 1000; i > 0; --i) big.insert(i);
        CHECK_TRUE(big.size() == 1000);
        auto v = big.to_vector();
        bool ok = (v.size() == 1000);
        for (std::size_t i = 1; ok && i < v.size(); ++i) ok = (v[i - 1] < v[i]);
        CHECK_TRUE(ok);
    }

    std::cout << "All SkipList tests passed (" << g_checks << " checks).\n";
    return 0;
}
