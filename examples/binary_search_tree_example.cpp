// BinarySearchTree 使用示例
#include <ceh/structures/binary_search_tree.hpp>
#include <iostream>

int main() {
    using ceh::BinarySearchTree;

    BinarySearchTree<int> tree;
    for (int v : {5, 3, 8, 1, 4, 7, 9, 2}) tree.insert(v);

    std::cout << "中序遍历（有序）: ";
    tree.inorder([](int v) { std::cout << v << ' '; });
    std::cout << "\n";

    std::cout << "节点数: " << tree.size() << "，高度: " << tree.height() << "\n";
    std::cout << "最小值: " << tree.min() << "，最大值: " << tree.max() << "\n";
    std::cout << "包含 4? " << (tree.contains(4) ? "是" : "否") << "\n";
    std::cout << "包含 6? " << (tree.contains(6) ? "是" : "否") << "\n";

    tree.erase(5);   // 删除根（两个孩子的情况）
    std::cout << "删除 5 后: ";
    tree.inorder([](int v) { std::cout << v << ' '; });
    std::cout << "\n";

    return 0;
}
