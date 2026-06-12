// AVLTree 使用示例：顺序插入也保持平衡
#include <ceh/structures/avl_tree.hpp>
#include <iostream>

int main() {
    ceh::AVLTree<int> t;
    for (int i = 1; i <= 15; ++i) t.insert(i);   // 升序插入

    std::cout << "中序遍历: ";
    for (int v : t.to_vector()) std::cout << v << ' ';
    std::cout << "\n";
    std::cout << "结点数: " << t.size() << "，树高: " << t.height()
              << "（未平衡时会是 15）\n";

    t.erase(8);
    std::cout << "删除 8 后包含 8? " << (t.contains(8) ? "是" : "否") << "\n";
    return 0;
}
