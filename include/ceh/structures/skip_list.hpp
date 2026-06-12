// ceh/structures/skip_list.hpp
//
// SkipList<T, Compare> —— 跳表（header-only），位于 ceh::。
//
// 概率型有序集合：用多层链表把查找/插入/删除做到期望 O(log n)，实现比平衡树
// 简单。集合语义（不存重复元素）。元素始终按 Compare 升序。
//
// 用法：
//     ceh::SkipList<int> s;
//     s.insert(3); s.insert(1); s.insert(2);
//     s.contains(2);     // true
//     s.to_vector();     // {1, 2, 3}
//     s.erase(2);

#ifndef CEH_STRUCTURES_SKIP_LIST_HPP
#define CEH_STRUCTURES_SKIP_LIST_HPP

#include <cstddef>
#include <cstdint>
#include <functional>
#include <random>
#include <utility>
#include <vector>

namespace ceh {

template <typename T, typename Compare = std::less<T>>
class SkipList {
public:
    using size_type = std::size_t;

    explicit SkipList(const Compare& comp = Compare{})
        : SkipList(std::random_device{}(), comp) {}

    // 固定种子构造（层数可复现，便于调试/测试）
    SkipList(std::uint64_t seed, const Compare& comp = Compare{})
        : comp_(comp), rng_(seed), head_(new Node(kMaxLevel, T())) {}

    ~SkipList() { if (head_) { clear_nodes(); delete head_; } }   // moved-from 时 head_ 为空

    SkipList(const SkipList& o) : SkipList(o.comp_) {
        for (Node* p = o.head_->forward[0]; p; p = p->forward[0]) insert(p->value);
    }
    SkipList(SkipList&& o) noexcept { swap(o); }
    SkipList& operator=(SkipList o) { swap(o); return *this; }

    void swap(SkipList& o) noexcept {
        std::swap(comp_, o.comp_);
        std::swap(rng_, o.rng_);
        std::swap(head_, o.head_);
        std::swap(level_, o.level_);
        std::swap(size_, o.size_);
    }

    bool empty() const { return size_ == 0; }
    size_type size() const { return size_; }

    void clear() {
        clear_nodes();
        for (int i = 0; i < kMaxLevel; ++i) head_->forward[static_cast<std::size_t>(i)] = nullptr;
        level_ = 1;
        size_ = 0;
    }

    bool contains(const T& value) const {
        Node* p = head_;
        for (int i = level_ - 1; i >= 0; --i)
            while (p->forward[static_cast<std::size_t>(i)] &&
                   comp_(p->forward[static_cast<std::size_t>(i)]->value, value))
                p = p->forward[static_cast<std::size_t>(i)];
        Node* nxt = p->forward[0];
        return nxt && !comp_(value, nxt->value) && !comp_(nxt->value, value);  // 相等
    }

    // 插入；已存在则返回 false
    bool insert(const T& value) {
        std::vector<Node*> update(static_cast<std::size_t>(kMaxLevel));
        Node* p = head_;
        for (int i = level_ - 1; i >= 0; --i) {
            while (p->forward[static_cast<std::size_t>(i)] &&
                   comp_(p->forward[static_cast<std::size_t>(i)]->value, value))
                p = p->forward[static_cast<std::size_t>(i)];
            update[static_cast<std::size_t>(i)] = p;
        }
        Node* nxt = p->forward[0];
        if (nxt && !comp_(value, nxt->value) && !comp_(nxt->value, value)) return false;  // 已存在

        int lvl = random_level();
        if (lvl > level_) {
            for (int i = level_; i < lvl; ++i) update[static_cast<std::size_t>(i)] = head_;
            level_ = lvl;
        }
        Node* node = new Node(lvl, value);
        for (int i = 0; i < lvl; ++i) {
            node->forward[static_cast<std::size_t>(i)] =
                update[static_cast<std::size_t>(i)]->forward[static_cast<std::size_t>(i)];
            update[static_cast<std::size_t>(i)]->forward[static_cast<std::size_t>(i)] = node;
        }
        ++size_;
        return true;
    }

    // 删除；不存在则返回 false
    bool erase(const T& value) {
        std::vector<Node*> update(static_cast<std::size_t>(kMaxLevel));
        Node* p = head_;
        for (int i = level_ - 1; i >= 0; --i) {
            while (p->forward[static_cast<std::size_t>(i)] &&
                   comp_(p->forward[static_cast<std::size_t>(i)]->value, value))
                p = p->forward[static_cast<std::size_t>(i)];
            update[static_cast<std::size_t>(i)] = p;
        }
        Node* node = p->forward[0];
        if (!node || comp_(value, node->value) || comp_(node->value, value)) return false;
        for (int i = 0; i < level_; ++i)
            if (update[static_cast<std::size_t>(i)]->forward[static_cast<std::size_t>(i)] == node)
                update[static_cast<std::size_t>(i)]->forward[static_cast<std::size_t>(i)] =
                    node->forward[static_cast<std::size_t>(i)];
        delete node;
        while (level_ > 1 && head_->forward[static_cast<std::size_t>(level_ - 1)] == nullptr) --level_;
        --size_;
        return true;
    }

    std::vector<T> to_vector() const {
        std::vector<T> out;
        out.reserve(size_);
        for (Node* p = head_->forward[0]; p; p = p->forward[0]) out.push_back(p->value);
        return out;
    }

private:
    struct Node {
        T value;
        std::vector<Node*> forward;
        Node(int level, const T& v) : value(v), forward(static_cast<std::size_t>(level), nullptr) {}
    };

    static constexpr int kMaxLevel = 16;

    Compare comp_;
    std::mt19937_64 rng_;
    Node* head_ = nullptr;   // 默认空，保证移动后源对象与移动目标都处于可析构状态
    int level_ = 1;          // 当前有效层数
    size_type size_ = 0;

    // 以概率 1/2 逐层升高，封顶 kMaxLevel
    int random_level() {
        int lvl = 1;
        while (lvl < kMaxLevel && (rng_() & 1u)) ++lvl;
        return lvl;
    }

    void clear_nodes() {
        Node* p = head_->forward[0];
        while (p) { Node* nx = p->forward[0]; delete p; p = nx; }
    }
};

template <typename T, typename Compare>
void swap(SkipList<T, Compare>& a, SkipList<T, Compare>& b) noexcept { a.swap(b); }

}  // namespace ceh

#endif  // CEH_STRUCTURES_SKIP_LIST_HPP
