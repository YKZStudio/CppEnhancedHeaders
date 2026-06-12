// ceh/structures/avl_tree.hpp
//
// AVLTree<T, Compare> —— 自平衡二叉搜索树（header-only），位于 ceh::。
//
// 通过 AVL 旋转保持任意结点左右子树高度差不超过 1，从而保证 insert / contains /
// erase 最坏也是 O(log n)。集合语义（不存重复元素），元素按 Compare 升序。
//
// 异常：对空树取 min()/max() 抛 std::out_of_range。
//
// 用法：
//     ceh::AVLTree<int> t;
//     t.insert(5); t.insert(3); t.insert(8);
//     t.to_vector();        // {3, 5, 8}
//     t.height();           // 平衡高度
//     t.erase(5);

#ifndef CEH_STRUCTURES_AVL_TREE_HPP
#define CEH_STRUCTURES_AVL_TREE_HPP

#include <algorithm>
#include <cstddef>
#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>

namespace ceh {

template <typename T, typename Compare = std::less<T>>
class AVLTree {
public:
    using size_type = std::size_t;

    AVLTree() = default;
    explicit AVLTree(const Compare& comp) : comp_(comp) {}
    ~AVLTree() { destroy(root_); }

    AVLTree(const AVLTree& o) : comp_(o.comp_), size_(o.size_) { root_ = clone(o.root_); }
    AVLTree(AVLTree&& o) noexcept { swap(o); }
    AVLTree& operator=(AVLTree o) { swap(o); return *this; }

    void swap(AVLTree& o) noexcept {
        std::swap(root_, o.root_);
        std::swap(size_, o.size_);
        std::swap(comp_, o.comp_);
    }

    bool empty() const { return size_ == 0; }
    size_type size() const { return size_; }
    void clear() { destroy(root_); root_ = nullptr; size_ = 0; }

    // 空树高度 0，单结点 1
    int height() const { return node_height(root_); }

    bool insert(const T& value) {
        size_type before = size_;
        root_ = insert_node(root_, value);
        return size_ > before;
    }

    bool erase(const T& value) {
        size_type before = size_;
        root_ = erase_node(root_, value);
        return size_ < before;
    }

    bool contains(const T& value) const {
        Node* cur = root_;
        while (cur) {
            if (comp_(value, cur->value)) cur = cur->left;
            else if (comp_(cur->value, value)) cur = cur->right;
            else return true;
        }
        return false;
    }

    const T& min() const {
        if (!root_) throw std::out_of_range("ceh::AVLTree: min() on empty tree");
        Node* cur = root_;
        while (cur->left) cur = cur->left;
        return cur->value;
    }
    const T& max() const {
        if (!root_) throw std::out_of_range("ceh::AVLTree: max() on empty tree");
        Node* cur = root_;
        while (cur->right) cur = cur->right;
        return cur->value;
    }

    template <typename Visitor>
    void inorder(Visitor visit) const { inorder_node(root_, visit); }

    std::vector<T> to_vector() const {
        std::vector<T> out;
        out.reserve(size_);
        inorder([&out](const T& v) { out.push_back(v); });
        return out;
    }

private:
    struct Node {
        T value;
        int height = 1;
        Node* left = nullptr;
        Node* right = nullptr;
        explicit Node(const T& v) : value(v) {}
    };

    Node* root_ = nullptr;
    size_type size_ = 0;
    Compare comp_{};

    static int node_height(Node* n) { return n ? n->height : 0; }
    static int balance_factor(Node* n) { return n ? node_height(n->left) - node_height(n->right) : 0; }
    static void update_height(Node* n) {
        n->height = 1 + std::max(node_height(n->left), node_height(n->right));
    }

    static Node* rotate_right(Node* y) {
        Node* x = y->left;
        y->left = x->right;
        x->right = y;
        update_height(y);
        update_height(x);
        return x;
    }
    static Node* rotate_left(Node* x) {
        Node* y = x->right;
        x->right = y->left;
        y->left = x;
        update_height(x);
        update_height(y);
        return y;
    }

    static Node* rebalance(Node* n) {
        update_height(n);
        int b = balance_factor(n);
        if (b > 1) {                                   // 左重
            if (balance_factor(n->left) < 0) n->left = rotate_left(n->left);   // LR
            return rotate_right(n);
        }
        if (b < -1) {                                  // 右重
            if (balance_factor(n->right) > 0) n->right = rotate_right(n->right); // RL
            return rotate_left(n);
        }
        return n;
    }

    Node* insert_node(Node* n, const T& value) {
        if (!n) { ++size_; return new Node(value); }
        if (comp_(value, n->value)) n->left = insert_node(n->left, value);
        else if (comp_(n->value, value)) n->right = insert_node(n->right, value);
        else return n;                                 // 已存在
        return rebalance(n);
    }

    Node* erase_node(Node* n, const T& value) {
        if (!n) return nullptr;
        if (comp_(value, n->value)) n->left = erase_node(n->left, value);
        else if (comp_(n->value, value)) n->right = erase_node(n->right, value);
        else {
            // 命中
            if (!n->left || !n->right) {
                Node* child = n->left ? n->left : n->right;
                delete n;
                --size_;
                return child ? rebalance(child) : nullptr;
            }
            Node* succ = n->right;                     // 右子树最小（中序后继）
            while (succ->left) succ = succ->left;
            n->value = succ->value;
            n->right = erase_node(n->right, succ->value);  // 该递归负责 --size_
        }
        return rebalance(n);
    }

    static void destroy(Node* n) {
        if (!n) return;
        destroy(n->left);
        destroy(n->right);
        delete n;
    }
    static Node* clone(Node* n) {
        if (!n) return nullptr;
        Node* c = new Node(n->value);
        c->height = n->height;
        c->left = clone(n->left);
        c->right = clone(n->right);
        return c;
    }

    template <typename Visitor>
    static void inorder_node(Node* n, Visitor& visit) {
        if (!n) return;
        inorder_node(n->left, visit);
        visit(n->value);
        inorder_node(n->right, visit);
    }
};

template <typename T, typename Compare>
void swap(AVLTree<T, Compare>& a, AVLTree<T, Compare>& b) noexcept { a.swap(b); }

}  // namespace ceh

#endif  // CEH_STRUCTURES_AVL_TREE_HPP
