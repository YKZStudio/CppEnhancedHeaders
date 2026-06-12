// LinkedList 使用示例
#include <ceh/structures/linked_list.hpp>
#include <iostream>

int main() {
    ceh::LinkedList<int> ls;
    for (int v : {1, 2, 3, 4, 5}) ls.push_back(v);
    ls.push_front(0);

    std::cout << "链表: ";
    ls.for_each([](int v) { std::cout << v << ' '; });
    std::cout << "(size=" << ls.size() << ")\n";

    ls.insert(3, 99);   // 在下标 3 前插入
    ls.erase(0);        // 删除头部
    ls.reverse();

    std::cout << "处理后: ";
    for (int v : ls.to_vector()) std::cout << v << ' ';
    std::cout << "\n";
    return 0;
}
