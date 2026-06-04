// DisjointSet 使用示例：统计「朋友圈」数量
#include <ceh/structures/disjoint_set.hpp>
#include <iostream>

int main() {
    using ceh::DisjointSet;

    // 6 个人（0..5），下面这些配对互为朋友
    DisjointSet ds(6);
    ds.unite(0, 1);
    ds.unite(1, 2);   // 0-1-2 一个圈
    ds.unite(3, 4);   // 3-4 一个圈
    // 5 自己一个圈

    std::cout << "朋友圈数量: " << ds.count() << "\n";       // 3
    std::cout << "0 和 2 同圈? " << (ds.connected(0, 2) ? "是" : "否") << "\n";
    std::cout << "0 和 3 同圈? " << (ds.connected(0, 3) ? "是" : "否") << "\n";
    std::cout << "0 所在圈人数: " << ds.size_of(0) << "\n";  // 3

    ds.unite(2, 3);   // 两个圈合并
    std::cout << "合并后 0 和 4 同圈? " << (ds.connected(0, 4) ? "是" : "否") << "\n";
    std::cout << "现在朋友圈数量: " << ds.count() << "\n";   // 2

    return 0;
}
