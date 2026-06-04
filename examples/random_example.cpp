// Random 使用示例
#include <ceh/utils/random.hpp>

#include <iostream>
#include <string>
#include <vector>

int main() {
    using ceh::Random;

    Random rng(2026);   // 固定种子，输出可复现

    std::cout << "掷 5 次骰子: ";
    for (int i = 0; i < 5; ++i) std::cout << rng.next_int(1, 6) << ' ';
    std::cout << "\n";

    std::cout << "3 个 [0,1) 随机数: ";
    for (int i = 0; i < 3; ++i) std::cout << rng.next_double() << ' ';
    std::cout << "\n";

    std::vector<std::string> fruits = {"苹果", "香蕉", "樱桃", "葡萄"};
    std::cout << "随机水果: " << rng.choice(fruits) << "\n";

    std::vector<int> deck = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    rng.shuffle(deck);
    std::cout << "洗牌后: ";
    for (int x : deck) std::cout << x << ' ';
    std::cout << "\n";

    return 0;
}
