// RedBlackTree 使用示例
#include <ceh/structures/red_black_tree.hpp>
#include <iostream>

int main() {
    ceh::RedBlackTree<int> t;
    for (int v : {50, 30, 70, 20, 40, 60, 80, 10}) t.insert(v);

    std::cout << "有序输出: ";
    for (int v : t.to_vector()) std::cout << v << ' ';
    std::cout << "\n";
    std::cout << "最小: " << t.min() << "，最大: " << t.max()
              << "，树高: " << t.height() << "\n";

    t.erase(30);
    t.erase(50);
    std::cout << "删除 30、50 后: ";
    for (int v : t.to_vector()) std::cout << v << ' ';
    std::cout << "\n";
    return 0;
}
