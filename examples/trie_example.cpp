// Trie 使用示例：前缀统计（输入联想的雏形）
#include <ceh/structures/trie.hpp>
#include <iostream>

int main() {
    using ceh::Trie;

    Trie dict;
    for (const char* w : {"cat", "car", "card", "care", "dog", "do"})
        dict.insert(w);

    std::cout << "词典里共有 " << dict.size() << " 个词\n";
    std::cout << "包含 'car'? " << (dict.contains("car") ? "是" : "否") << "\n";
    std::cout << "包含 'ca'?  " << (dict.contains("ca") ? "是" : "否")
              << " （是前缀但不是词）\n";

    std::cout << "以 'ca' 开头的词数: " << dict.count_prefix("ca") << "\n";   // cat,car,card,care
    std::cout << "以 'car' 开头的词数: " << dict.count_prefix("car") << "\n";  // car,card,care
    std::cout << "以 'do' 开头的词数: " << dict.count_prefix("do") << "\n";    // dog,do

    dict.erase("car");
    std::cout << "删除 'car' 后，以 'car' 开头的词数: "
              << dict.count_prefix("car") << "\n";   // card,care -> 2

    return 0;
}
