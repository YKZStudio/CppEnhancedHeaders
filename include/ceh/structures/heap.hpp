// ceh/structures/heap.hpp
//
// Heap<T, Compare> —— 二叉堆（header-only）。
//
// 用一个 std::vector 维护完全二叉堆。与 std::priority_queue 约定一致：
// 默认 Compare = std::less<T>，得到 **大顶堆**（top() 为最大元素）；
// 传 std::greater<T> 即得小顶堆。
//
// 复杂度：push / pop 为 O(log n)，top 为 O(1)；区间构造用自底向上建堆，O(n)。
//
// 异常：对空堆调用 top() / pop() 抛 std::out_of_range。
//
// 用法：
//     ceh::Heap<int> h;                 // 大顶堆
//     h.push(3); h.push(1); h.push(5);
//     h.top();                          // 5
//     ceh::Heap<int, std::greater<int>> mn;   // 小顶堆

#ifndef CEH_STRUCTURES_HEAP_HPP
#define CEH_STRUCTURES_HEAP_HPP

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>

namespace ceh {

template <typename T, typename Compare = std::less<T>>
class Heap {
public:
    using value_type = T;
    using size_type = std::size_t;

    Heap() = default;
    explicit Heap(const Compare& comp) : comp_(comp) {}

    // 由区间 [first, last) 构造，自底向上建堆（O(n)）
    template <typename InputIt>
    Heap(InputIt first, InputIt last, const Compare& comp = Compare{})
        : data_(first, last), comp_(comp) {
        for (size_type i = data_.size() / 2; i-- > 0;) sift_down(i);
    }

    bool empty() const { return data_.empty(); }
    size_type size() const { return data_.size(); }
    void clear() { data_.clear(); }

    // 堆顶（大顶堆为最大、小顶堆为最小）
    const T& top() const {
        if (data_.empty()) throw std::out_of_range("ceh::Heap: top() on empty heap");
        return data_.front();
    }

    void push(const T& value) {
        data_.push_back(value);
        sift_up(data_.size() - 1);
    }
    void push(T&& value) {
        data_.push_back(std::move(value));
        sift_up(data_.size() - 1);
    }

    // 弹出堆顶
    void pop() {
        if (data_.empty()) throw std::out_of_range("ceh::Heap: pop() on empty heap");
        data_.front() = std::move(data_.back());
        data_.pop_back();
        if (!data_.empty()) sift_down(0);
    }

private:
    std::vector<T> data_;
    Compare comp_{};

    // comp_(a, b) 为真表示 a 的「优先级低于」b（b 应更靠近堆顶）
    void sift_up(size_type i) {
        while (i > 0) {
            size_type parent = (i - 1) / 2;
            if (comp_(data_[parent], data_[i])) {   // 父亲优先级更低则上浮
                std::swap(data_[parent], data_[i]);
                i = parent;
            } else {
                break;
            }
        }
    }

    void sift_down(size_type i) {
        size_type n = data_.size();
        while (true) {
            size_type left = 2 * i + 1;
            size_type right = 2 * i + 2;
            size_type best = i;   // 在 i、left、right 中找优先级最高者
            if (left < n && comp_(data_[best], data_[left])) best = left;
            if (right < n && comp_(data_[best], data_[right])) best = right;
            if (best == i) break;
            std::swap(data_[i], data_[best]);
            i = best;
        }
    }
};

}  // namespace ceh

#endif  // CEH_STRUCTURES_HEAP_HPP
