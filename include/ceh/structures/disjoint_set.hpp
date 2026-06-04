// ceh/structures/disjoint_set.hpp
//
// DisjointSet —— 并查集 / 不相交集合（header-only）。
//
// 维护 [0, n) 这 n 个元素的划分，支持高效的「合并两个集合」与「查询是否同集合」。
// 采用 **路径压缩**（find 时路径减半）+ **按大小合并**，单次操作的均摊复杂度
// 接近 O(α(n))（α 为反阿克曼函数，实际可视为常数）。
//
// 异常：下标越界抛 std::out_of_range。
//
// 用法：
//     ceh::DisjointSet ds(5);
//     ds.unite(0, 1);
//     ds.unite(3, 4);
//     ds.connected(0, 1);   // true
//     ds.connected(1, 3);   // false
//     ds.count();           // 3 个集合：{0,1} {2} {3,4}

#ifndef CEH_STRUCTURES_DISJOINT_SET_HPP
#define CEH_STRUCTURES_DISJOINT_SET_HPP

#include <cstddef>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

namespace ceh {

class DisjointSet {
public:
    using size_type = std::size_t;

    // 创建 n 个各自独立的单元素集合：0, 1, ..., n-1
    explicit DisjointSet(size_type n)
        : parent_(n), size_(n, 1), count_(n) {
        std::iota(parent_.begin(), parent_.end(), size_type{0});  // parent_[i] = i
    }

    // 找 x 所在集合的代表元（根），并做路径减半压缩
    size_type find(size_type x) {
        check(x);
        while (parent_[x] != x) {
            parent_[x] = parent_[parent_[x]];   // 路径减半：指向祖父，压扁树
            x = parent_[x];
        }
        return x;
    }

    // 合并 x、y 所在集合；本就同集合返回 false，发生合并返回 true
    bool unite(size_type x, size_type y) {
        size_type rx = find(x), ry = find(y);
        if (rx == ry) return false;
        // 按大小合并：把小树挂到大树下，控制树高
        if (size_[rx] < size_[ry]) std::swap(rx, ry);
        parent_[ry] = rx;
        size_[rx] += size_[ry];
        --count_;
        return true;
    }

    bool connected(size_type x, size_type y) { return find(x) == find(y); }

    // x 所在集合的元素个数
    size_type size_of(size_type x) { return size_[find(x)]; }

    // 当前集合（连通分量）总数
    size_type count() const { return count_; }

    // 元素总数
    size_type size() const { return parent_.size(); }

private:
    std::vector<size_type> parent_;
    std::vector<size_type> size_;   // 仅在「根」处有意义：该集合的元素数
    size_type count_;

    void check(size_type x) const {
        if (x >= parent_.size())
            throw std::out_of_range("ceh::DisjointSet: index out of range");
    }
};

}  // namespace ceh

#endif  // CEH_STRUCTURES_DISJOINT_SET_HPP
