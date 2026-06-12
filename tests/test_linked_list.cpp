// LinkedList 单元测试（零依赖，基于 assert）
#include <ceh/structures/linked_list.hpp>

#include <iostream>
#include <vector>

using ceh::LinkedList;

static int g_checks = 0;

#define CHECK_TRUE(expr)                                               \
    do { ++g_checks; if (!(expr)) {                                    \
        std::cerr << "FAIL [" << __LINE__ << "] " #expr "\n"; std::exit(1);} } while (0)

#define CHECK_THROWS(ExType, expr)                                     \
    do { ++g_checks; bool t = false; try { (void)(expr); }            \
        catch (const ExType&) { t = true; }                           \
        if (!t) { std::cerr << "FAIL [" << __LINE__ << "] expected throw\n"; std::exit(1);} } while (0)

using IV = std::vector<int>;

int main() {
    LinkedList<int> ls;

    // 空表
    CHECK_TRUE(ls.empty() && ls.size() == 0);
    CHECK_THROWS(std::out_of_range, ls.front());
    CHECK_THROWS(std::out_of_range, ls.pop_back());

    // 头尾插入
    ls.push_back(1);
    ls.push_back(2);
    ls.push_front(0);
    CHECK_TRUE(ls.to_vector() == (IV{0, 1, 2}));
    CHECK_TRUE(ls.size() == 3);
    CHECK_TRUE(ls.front() == 0 && ls.back() == 2);

    // 下标访问
    CHECK_TRUE(ls.at(0) == 0 && ls.at(1) == 1 && ls.at(2) == 2);
    CHECK_THROWS(std::out_of_range, ls.at(3));
    ls.at(1) = 11;
    CHECK_TRUE(ls.to_vector() == (IV{0, 11, 2}));
    ls.at(1) = 1;

    // 头尾删除
    ls.pop_front();
    ls.pop_back();
    CHECK_TRUE(ls.to_vector() == (IV{1}));
    ls.push_back(2); ls.push_back(3);          // {1,2,3}

    // 中间插入 / 删除
    ls.insert(1, 9);                            // {1,9,2,3}
    CHECK_TRUE(ls.to_vector() == (IV{1, 9, 2, 3}));
    ls.insert(0, 0);                            // {0,1,9,2,3}
    ls.insert(5, 4);                            // 末尾 {0,1,9,2,3,4}
    CHECK_TRUE(ls.to_vector() == (IV{0, 1, 9, 2, 3, 4}));
    CHECK_THROWS(std::out_of_range, ls.insert(99, 0));
    ls.erase(2);                                // 删 9 -> {0,1,2,3,4}
    CHECK_TRUE(ls.to_vector() == (IV{0, 1, 2, 3, 4}));
    CHECK_THROWS(std::out_of_range, ls.erase(99));

    // 反转
    ls.reverse();
    CHECK_TRUE(ls.to_vector() == (IV{4, 3, 2, 1, 0}));

    // for_each
    {
        int sum = 0;
        ls.for_each([&sum](int v) { sum += v; });
        CHECK_TRUE(sum == 10);
    }

    // 拷贝独立性
    {
        LinkedList<int> copy = ls;
        copy.push_back(99);
        CHECK_TRUE(copy.size() == ls.size() + 1);
        CHECK_TRUE(ls.to_vector() == (IV{4, 3, 2, 1, 0}));   // 原表不变
    }

    // 移动
    {
        LinkedList<int> src;
        src.push_back(7); src.push_back(8);
        LinkedList<int> moved = std::move(src);
        CHECK_TRUE(moved.to_vector() == (IV{7, 8}));
    }

    // clear
    ls.clear();
    CHECK_TRUE(ls.empty());

    // 单元素反转
    {
        LinkedList<int> one;
        one.push_back(42);
        one.reverse();
        CHECK_TRUE(one.to_vector() == (IV{42}));
    }

    std::cout << "All LinkedList tests passed (" << g_checks << " checks).\n";
    return 0;
}
