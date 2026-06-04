// Heap 使用示例：优先队列 + 取最小的 k 个
#include <ceh/structures/heap.hpp>
#include <functional>
#include <iostream>
#include <vector>

int main() {
    using ceh::Heap;

    // 大顶堆当优先队列：每次取出当前最大
    Heap<int> pq;
    for (int v : {3, 1, 4, 1, 5, 9, 2, 6}) pq.push(v);
    std::cout << "按优先级（从大到小）弹出: ";
    while (!pq.empty()) {
        std::cout << pq.top() << ' ';
        pq.pop();
    }
    std::cout << "\n";

    // 小顶堆求「最小的 3 个数」
    std::vector<int> data = {8, 3, 7, 1, 9, 2, 5};
    Heap<int, std::greater<int>> mn(data.begin(), data.end());
    std::cout << "最小的 3 个: ";
    for (int i = 0; i < 3 && !mn.empty(); ++i) {
        std::cout << mn.top() << ' ';
        mn.pop();
    }
    std::cout << "\n";

    return 0;
}
