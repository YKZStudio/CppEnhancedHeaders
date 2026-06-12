// ceh/structures/red_black_tree.hpp
//
// RedBlackTree<T, Compare> —— 红黑树（header-only），位于 ceh::。
//
// 自平衡 BST，靠红黑着色规则保证最长路径不超过最短路径的两倍，从而 insert /
// contains / erase 最坏 O(log n)。集合语义（不存重复元素），元素按 Compare 升序。
// 采用 CLRS 风格实现：单个 nil 哨兵代表所有空叶子，简化旋转与删除修复。
//
// 异常：对空树取 min()/max() 抛 std::out_of_range。
//
// 用法：
//     ceh::RedBlackTree<int> t;
//     for (int v : {5, 3, 8, 1, 4}) t.insert(v);
//     t.to_vector();        // {1, 3, 4, 5, 8}
//     t.erase(3);

#ifndef CEH_STRUCTURES_RED_BLACK_TREE_HPP
#define CEH_STRUCTURES_RED_BLACK_TREE_HPP

#include <algorithm>
#include <cstddef>
#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>

namespace ceh {

template <typename T, typename Compare = std::less<T>>
class RedBlackTree {
public:
    using size_type = std::size_t;

    RedBlackTree() : nil_(new Node(T(), Color::Black)), root_(nil_), comp_() {
        nil_->left = nil_->right = nil_->parent = nil_;
    }
    explicit RedBlackTree(const Compare& comp)
        : nil_(new Node(T(), Color::Black)), root_(nil_), comp_(comp) {
        nil_->left = nil_->right = nil_->parent = nil_;
    }
    ~RedBlackTree() { destroy(root_); delete nil_; }

    RedBlackTree(const RedBlackTree& o) : RedBlackTree(o.comp_) {
        for (Node* p = minimum(o.root_, o.nil_); p != o.nil_; p = successor(o, p))
            insert(p->value);
    }
    RedBlackTree(RedBlackTree&& o) noexcept
        : nil_(new Node(T(), Color::Black)), root_(nil_), comp_() {
        nil_->left = nil_->right = nil_->parent = nil_;
        swap(o);
    }
    RedBlackTree& operator=(RedBlackTree o) { swap(o); return *this; }

    void swap(RedBlackTree& o) noexcept {
        std::swap(nil_, o.nil_);
        std::swap(root_, o.root_);
        std::swap(size_, o.size_);
        std::swap(comp_, o.comp_);
    }

    bool empty() const { return size_ == 0; }
    size_type size() const { return size_; }
    void clear() { destroy(root_); root_ = nil_; size_ = 0; }

    int height() const { return node_height(root_); }

    bool contains(const T& value) const { return find(value) != nil_; }

    const T& min() const {
        if (root_ == nil_) throw std::out_of_range("ceh::RedBlackTree: min() on empty tree");
        return minimum(root_, nil_)->value;
    }
    const T& max() const {
        if (root_ == nil_) throw std::out_of_range("ceh::RedBlackTree: max() on empty tree");
        Node* n = root_;
        while (n->right != nil_) n = n->right;
        return n->value;
    }

    bool insert(const T& value) {
        Node* y = nil_;
        Node* x = root_;
        while (x != nil_) {
            y = x;
            if (comp_(value, x->value)) x = x->left;
            else if (comp_(x->value, value)) x = x->right;
            else return false;                          // 已存在
        }
        Node* z = new Node(value, Color::Red);
        z->parent = y;
        z->left = z->right = nil_;
        if (y == nil_) root_ = z;
        else if (comp_(value, y->value)) y->left = z;
        else y->right = z;
        ++size_;
        insert_fixup(z);
        return true;
    }

    bool erase(const T& value) {
        Node* z = find(value);
        if (z == nil_) return false;
        delete_node(z);
        --size_;
        return true;
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
    enum class Color { Red, Black };
    struct Node {
        T value;
        Color color;
        Node* parent = nullptr;
        Node* left = nullptr;
        Node* right = nullptr;
        Node(const T& v, Color c) : value(v), color(c) {}
    };

    Node* nil_;
    Node* root_;
    size_type size_ = 0;
    Compare comp_;

    Node* find(const T& value) const {
        Node* cur = root_;
        while (cur != nil_) {
            if (comp_(value, cur->value)) cur = cur->left;
            else if (comp_(cur->value, value)) cur = cur->right;
            else return cur;
        }
        return nil_;
    }

    static Node* minimum(Node* n, Node* nil) {
        if (n == nil) return nil;
        while (n->left != nil) n = n->left;
        return n;
    }

    // 中序后继（用于拷贝时按序遍历）
    static Node* successor(const RedBlackTree& t, Node* n) {
        if (n->right != t.nil_) return minimum(n->right, t.nil_);
        Node* p = n->parent;
        while (p != t.nil_ && n == p->right) { n = p; p = p->parent; }
        return p;
    }

    void left_rotate(Node* x) {
        Node* y = x->right;
        x->right = y->left;
        if (y->left != nil_) y->left->parent = x;
        y->parent = x->parent;
        if (x->parent == nil_) root_ = y;
        else if (x == x->parent->left) x->parent->left = y;
        else x->parent->right = y;
        y->left = x;
        x->parent = y;
    }
    void right_rotate(Node* x) {
        Node* y = x->left;
        x->left = y->right;
        if (y->right != nil_) y->right->parent = x;
        y->parent = x->parent;
        if (x->parent == nil_) root_ = y;
        else if (x == x->parent->right) x->parent->right = y;
        else x->parent->left = y;
        y->right = x;
        x->parent = y;
    }

    void insert_fixup(Node* z) {
        while (z->parent->color == Color::Red) {
            if (z->parent == z->parent->parent->left) {
                Node* y = z->parent->parent->right;        // 叔结点
                if (y->color == Color::Red) {              // 情况 1
                    z->parent->color = Color::Black;
                    y->color = Color::Black;
                    z->parent->parent->color = Color::Red;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->right) {           // 情况 2
                        z = z->parent;
                        left_rotate(z);
                    }
                    z->parent->color = Color::Black;        // 情况 3
                    z->parent->parent->color = Color::Red;
                    right_rotate(z->parent->parent);
                }
            } else {                                       // 镜像
                Node* y = z->parent->parent->left;
                if (y->color == Color::Red) {
                    z->parent->color = Color::Black;
                    y->color = Color::Black;
                    z->parent->parent->color = Color::Red;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->left) {
                        z = z->parent;
                        right_rotate(z);
                    }
                    z->parent->color = Color::Black;
                    z->parent->parent->color = Color::Red;
                    left_rotate(z->parent->parent);
                }
            }
        }
        root_->color = Color::Black;
    }

    void transplant(Node* u, Node* v) {
        if (u->parent == nil_) root_ = v;
        else if (u == u->parent->left) u->parent->left = v;
        else u->parent->right = v;
        v->parent = u->parent;
    }

    void delete_node(Node* z) {
        Node* y = z;
        Color y_original = y->color;
        Node* x;
        if (z->left == nil_) {
            x = z->right;
            transplant(z, z->right);
        } else if (z->right == nil_) {
            x = z->left;
            transplant(z, z->left);
        } else {
            y = minimum(z->right, nil_);
            y_original = y->color;
            x = y->right;
            if (y->parent == z) {
                x->parent = y;                              // 即便 x==nil_ 也要设置（用于 fixup）
            } else {
                transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }
            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }
        delete z;
        if (y_original == Color::Black) delete_fixup(x);
    }

    void delete_fixup(Node* x) {
        while (x != root_ && x->color == Color::Black) {
            if (x == x->parent->left) {
                Node* w = x->parent->right;                 // 兄弟
                if (w->color == Color::Red) {               // 情况 1
                    w->color = Color::Black;
                    x->parent->color = Color::Red;
                    left_rotate(x->parent);
                    w = x->parent->right;
                }
                if (w->left->color == Color::Black && w->right->color == Color::Black) {
                    w->color = Color::Red;                   // 情况 2
                    x = x->parent;
                } else {
                    if (w->right->color == Color::Black) {   // 情况 3
                        w->left->color = Color::Black;
                        w->color = Color::Red;
                        right_rotate(w);
                        w = x->parent->right;
                    }
                    w->color = x->parent->color;             // 情况 4
                    x->parent->color = Color::Black;
                    w->right->color = Color::Black;
                    left_rotate(x->parent);
                    x = root_;
                }
            } else {                                        // 镜像
                Node* w = x->parent->left;
                if (w->color == Color::Red) {
                    w->color = Color::Black;
                    x->parent->color = Color::Red;
                    right_rotate(x->parent);
                    w = x->parent->left;
                }
                if (w->right->color == Color::Black && w->left->color == Color::Black) {
                    w->color = Color::Red;
                    x = x->parent;
                } else {
                    if (w->left->color == Color::Black) {
                        w->right->color = Color::Black;
                        w->color = Color::Red;
                        left_rotate(w);
                        w = x->parent->left;
                    }
                    w->color = x->parent->color;
                    x->parent->color = Color::Black;
                    w->left->color = Color::Black;
                    right_rotate(x->parent);
                    x = root_;
                }
            }
        }
        x->color = Color::Black;
    }

    int node_height(Node* n) const {
        if (n == nil_) return 0;
        return 1 + std::max(node_height(n->left), node_height(n->right));
    }

    void destroy(Node* n) {
        if (n == nil_) return;
        destroy(n->left);
        destroy(n->right);
        delete n;
    }

    template <typename Visitor>
    void inorder_node(Node* n, Visitor& visit) const {
        if (n == nil_) return;
        inorder_node(n->left, visit);
        visit(n->value);
        inorder_node(n->right, visit);
    }
};

template <typename T, typename Compare>
void swap(RedBlackTree<T, Compare>& a, RedBlackTree<T, Compare>& b) noexcept { a.swap(b); }

}  // namespace ceh

#endif  // CEH_STRUCTURES_RED_BLACK_TREE_HPP
