// SkipList 使用示例
#include <ceh/structures/skip_list.hpp>
#include <iostream>

int main() {
    ceh::SkipList<int> s;
    for (int v : {42, 7, 19, 3, 25, 11, 50, 1}) s.insert(v);

    std::cout << "有序输出: ";
    for (int v : s.to_vector()) std::cout << v << ' ';
    std::cout << "\n";

    std::cout << "包含 19? " << (s.contains(19) ? "是" : "否") << "\n";
    std::cout << "包含 20? " << (s.contains(20) ? "是" : "否") << "\n";

    s.erase(19);
    std::cout << "删除 19 后: ";
    for (int v : s.to_vector()) std::cout << v << ' ';
    std::cout << "\n";
    return 0;
}
