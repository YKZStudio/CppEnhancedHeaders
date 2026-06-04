// ceh/structures/binary_search_tree.hpp
//
// BinarySearchTree<T, Compare> —— 经典（无自平衡）二叉搜索树（header-only）。
//
// 集合语义：不存储重复元素。自平衡版本（AVL / 红黑树）作为后续独立模块。
// 子节点用 std::unique_ptr 持有，析构自动回收、深拷贝可控、异常安全。
//
// 复杂度：insert / contains / erase 平均 O(log n)，最坏 O(n)（退化为链）。
//
// 异常：对空树取 min() / max() 抛 std::out_of_range。
//
// 用法：
//     ceh::BinarySearchTree<int> t;
//     t.insert(5); t.insert(3); t.insert(8);
//     t.inorder([](int v){ std::cout << v << ' '; });   // 3 5 8
//     t.contains(3);   // true
//     t.erase(5);

#ifndef CEH_STRUCTURES_BINARY_SEARCH_TREE_HPP
#define CEH_STRUCTURES_BINARY_SEARCH_TREE_HPP

#include <algorithm>
#include <cstddef>
#include <functional>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

namespace ceh {

template <typename T, typename Compare = std::less<T>>
class BinarySearchTree {
public:
    using value_type = T;
    using size_type = std::size_t;

    BinarySearchTree() = default;
    explicit BinarySearchTree(const Compare& comp) : comp_(comp) {}

    // 深拷贝
    BinarySearchTree(const BinarySearchTree& o)
        : root_(clone(o.root_.get())), size_(o.size_), comp_(o.comp_) {}

    BinarySearchTree& operator=(const BinarySearchTree& o) {
        if (this != &o) {
            BinarySearchTree tmp(o);
            swap(tmp);
        }
        return *this;
    }

    BinarySearchTree(BinarySearchTree&&) noexcept = default;
    BinarySearchTree& operator=(BinarySearchTree&&) noexcept = default;

    void swap(BinarySearchTree& o) noexcept {
        using std::swap;
        swap(root_, o.root_);
        swap(size_, o.size_);
        swap(comp_, o.comp_);
    }

    // ---- 容量 ----
    bool empty() const { return size_ == 0; }
    size_type size() const { return size_; }
    void clear() { root_.reset(); size_ = 0; }

    // ---- 修改 ----
    // 插入；若元素已存在返回 false（不重复插入）
    bool insert(const T& value) { return insert_node(root_, value); }

    // 删除；若元素不存在返回 false
    bool erase(const T& value) { return erase_node(root_, value); }

    // ---- 查询 ----
    bool contains(const T& value) const {
        const Node* cur = root_.get();
        while (cur) {
            if (comp_(value, cur->value))      cur = cur->left.get();
            else if (comp_(cur->value, value)) cur = cur->right.get();
            else return true;
        }
        return false;
    }

    const T& min() const {
        if (!root_) throw std::out_of_range("ceh::BinarySearchTree: min() on empty tree");
        const Node* cur = root_.get();
        while (cur->left) cur = cur->left.get();
        return cur->value;
    }

    const T& max() const {
        if (!root_) throw std::out_of_range("ceh::BinarySearchTree: max() on empty tree");
        const Node* cur = root_.get();
        while (cur->right) cur = cur->right.get();
        return cur->value;
    }

    // 空树高度 -1，单节点高度 0
    int height() const { return height_node(root_.get()); }

    // ---- 遍历（visitor 接收 const T&）----
    template <typename Visitor>
    void inorder(Visitor visit) const { inorder_node(root_.get(), visit); }

    template <typename Visitor>
    void preorder(Visitor visit) const { preorder_node(root_.get(), visit); }

    template <typename Visitor>
    void postorder(Visitor visit) const { postorder_node(root_.get(), visit); }

    // 中序遍历得到的有序序列
    std::vector<T> to_vector() const {
        std::vector<T> out;
        out.reserve(size_);
        inorder([&out](const T& v) { out.push_back(v); });
        return out;
    }

private:
    struct Node {
        T value;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
        explicit Node(const T& v) : value(v) {}
    };

    std::unique_ptr<Node> root_;
    size_type size_ = 0;
    Compare comp_{};

    bool insert_node(std::unique_ptr<Node>& node, const T& value) {
        if (!node) {
            node = std::unique_ptr<Node>(new Node(value));
            ++size_;
            return true;
        }
        if (comp_(value, node->value)) return insert_node(node->left, value);
        if (comp_(node->value, value)) return insert_node(node->right, value);
        return false;   // 已存在
    }

    bool erase_node(std::unique_ptr<Node>& node, const T& value) {
        if (!node) return false;
        if (comp_(value, node->value)) return erase_node(node->left, value);
        if (comp_(node->value, value)) return erase_node(node->right, value);

        // 命中当前节点
        if (!node->left) {
            node = std::move(node->right);          // 0 个或仅右孩子
            --size_;
        } else if (!node->right) {
            node = std::move(node->left);           // 仅左孩子
            --size_;
        } else {
            // 两个孩子：用右子树最小值（中序后继）替换当前值，再到右子树删掉后继。
            // 后继是右子树的「最左」结点，必无左孩子，故其删除只会落到上面的
            // 叶子 / 单孩子分支，不会无限递归；size_ 也由那次递归统一维护。
            Node* succ = node->right.get();
            while (succ->left) succ = succ->left.get();
            node->value = succ->value;
            erase_node(node->right, succ->value);
        }
        return true;
    }

    static std::unique_ptr<Node> clone(const Node* n) {
        if (!n) return nullptr;
        std::unique_ptr<Node> c(new Node(n->value));
        c->left = clone(n->left.get());
        c->right = clone(n->right.get());
        return c;
    }

    static int height_node(const Node* n) {
        if (!n) return -1;
        return 1 + std::max(height_node(n->left.get()), height_node(n->right.get()));
    }

    template <typename Visitor>
    static void inorder_node(const Node* n, Visitor& visit) {
        if (!n) return;
        inorder_node(n->left.get(), visit);
        visit(n->value);
        inorder_node(n->right.get(), visit);
    }

    template <typename Visitor>
    static void preorder_node(const Node* n, Visitor& visit) {
        if (!n) return;
        visit(n->value);
        preorder_node(n->left.get(), visit);
        preorder_node(n->right.get(), visit);
    }

    template <typename Visitor>
    static void postorder_node(const Node* n, Visitor& visit) {
        if (!n) return;
        postorder_node(n->left.get(), visit);
        postorder_node(n->right.get(), visit);
        visit(n->value);
    }
};

// 自由 swap
template <typename T, typename Compare>
void swap(BinarySearchTree<T, Compare>& a, BinarySearchTree<T, Compare>& b) noexcept {
    a.swap(b);
}

}  // namespace ceh

#endif  // CEH_STRUCTURES_BINARY_SEARCH_TREE_HPP
