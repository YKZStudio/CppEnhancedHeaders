// ceh/structures/linked_list.hpp
//
// LinkedList<T> —— 双向链表（header-only），位于 ceh::。
//
// 头尾各 O(1) 插入/删除，支持按下标访问/插入/删除（O(n)）、整体反转、遍历。
// 用裸指针手工维护前后向链接，析构自动释放；拷贝为深拷贝，赋值用 copy-and-swap。
//
// 异常：对空链表取 front()/back()/pop_*，或下标越界，抛 std::out_of_range。
//
// 用法：
//     ceh::LinkedList<int> ls;
//     ls.push_back(1); ls.push_front(0); ls.push_back(2);
//     ls.to_vector();      // {0, 1, 2}
//     ls.reverse();        // {2, 1, 0}

#ifndef CEH_STRUCTURES_LINKED_LIST_HPP
#define CEH_STRUCTURES_LINKED_LIST_HPP

#include <cstddef>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace ceh {

template <typename T>
class LinkedList {
public:
    using size_type = std::size_t;

    LinkedList() = default;
    ~LinkedList() { clear(); }

    LinkedList(const LinkedList& o) {
        for (Node* p = o.head_; p; p = p->next) push_back(p->value);
    }
    LinkedList(LinkedList&& o) noexcept { swap(o); }
    // 按值传参 + swap，一举覆盖拷贝赋值与移动赋值
    LinkedList& operator=(LinkedList o) { swap(o); return *this; }

    void swap(LinkedList& o) noexcept {
        std::swap(head_, o.head_);
        std::swap(tail_, o.tail_);
        std::swap(size_, o.size_);
    }

    bool empty() const { return size_ == 0; }
    size_type size() const { return size_; }

    void clear() {
        Node* p = head_;
        while (p) { Node* nx = p->next; delete p; p = nx; }
        head_ = tail_ = nullptr;
        size_ = 0;
    }

    void push_back(const T& value) {
        Node* node = new Node{value, tail_, nullptr};
        if (tail_) tail_->next = node; else head_ = node;
        tail_ = node;
        ++size_;
    }
    void push_front(const T& value) {
        Node* node = new Node{value, nullptr, head_};
        if (head_) head_->prev = node; else tail_ = node;
        head_ = node;
        ++size_;
    }

    void pop_back() {
        if (!tail_) throw std::out_of_range("ceh::LinkedList: pop_back on empty list");
        Node* node = tail_;
        tail_ = node->prev;
        if (tail_) tail_->next = nullptr; else head_ = nullptr;
        delete node;
        --size_;
    }
    void pop_front() {
        if (!head_) throw std::out_of_range("ceh::LinkedList: pop_front on empty list");
        Node* node = head_;
        head_ = node->next;
        if (head_) head_->prev = nullptr; else tail_ = nullptr;
        delete node;
        --size_;
    }

    T& front() { return non_empty(head_, "front")->value; }
    const T& front() const { return non_empty(head_, "front")->value; }
    T& back() { return non_empty(tail_, "back")->value; }
    const T& back() const { return non_empty(tail_, "back")->value; }

    T& at(size_type index) { return node_at(index)->value; }
    const T& at(size_type index) const { return node_at(index)->value; }

    // 在下标 index 之前插入（index == size 时等价 push_back）
    void insert(size_type index, const T& value) {
        if (index > size_) throw std::out_of_range("ceh::LinkedList: insert index out of range");
        if (index == 0) { push_front(value); return; }
        if (index == size_) { push_back(value); return; }
        Node* nxt = node_at(index);
        Node* prv = nxt->prev;
        Node* node = new Node{value, prv, nxt};
        prv->next = node;
        nxt->prev = node;
        ++size_;
    }

    void erase(size_type index) {
        Node* node = node_at(index);
        if (node->prev) node->prev->next = node->next; else head_ = node->next;
        if (node->next) node->next->prev = node->prev; else tail_ = node->prev;
        delete node;
        --size_;
    }

    // 原地反转
    void reverse() {
        Node* p = head_;
        while (p) { std::swap(p->prev, p->next); p = p->prev; }  // 交换后沿 prev 前进
        std::swap(head_, tail_);
    }

    std::vector<T> to_vector() const {
        std::vector<T> out;
        out.reserve(size_);
        for (Node* p = head_; p; p = p->next) out.push_back(p->value);
        return out;
    }

    template <typename F>
    void for_each(F f) const {
        for (Node* p = head_; p; p = p->next) f(p->value);
    }

private:
    struct Node {
        T value;
        Node* prev;
        Node* next;
    };

    Node* head_ = nullptr;
    Node* tail_ = nullptr;
    size_type size_ = 0;

    static Node* non_empty(Node* p, const char* what) {
        if (!p) throw std::out_of_range(std::string("ceh::LinkedList: ") + what + " on empty list");
        return p;
    }

    Node* node_at(size_type index) const {
        if (index >= size_) throw std::out_of_range("ceh::LinkedList: index out of range");
        Node* p;
        if (index < size_ / 2) {                 // 就近从头或从尾走
            p = head_;
            for (size_type i = 0; i < index; ++i) p = p->next;
        } else {
            p = tail_;
            for (size_type i = size_ - 1; i > index; --i) p = p->prev;
        }
        return p;
    }
};

template <typename T>
void swap(LinkedList<T>& a, LinkedList<T>& b) noexcept { a.swap(b); }

}  // namespace ceh

#endif  // CEH_STRUCTURES_LINKED_LIST_HPP
