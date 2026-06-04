// ceh/structures/trie.hpp
//
// Trie —— 字典树 / 前缀树（header-only），存储 std::string 的集合（去重）。
//
// 每个节点用哈希表保存子节点，可容纳任意字节字符（不限于小写字母）。
// 每个节点维护两个计数：words（以此结点结尾的词数）与 prefixes（经过此结点的
// 词数），从而 O(L) 支持「有多少词以某前缀开头」。删除会顺带回收变空的分支。
//
// 复杂度：insert / contains / erase / starts_with / count_prefix 均为 O(L)，
// L 为字符串长度。
//
// 用法：
//     ceh::Trie t;
//     t.insert("apple"); t.insert("app");
//     t.contains("app");         // true
//     t.starts_with("ap");        // true
//     t.count_prefix("app");      // 2（apple, app）

#ifndef CEH_STRUCTURES_TRIE_HPP
#define CEH_STRUCTURES_TRIE_HPP

#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>

namespace ceh {

class Trie {
public:
    using size_type = std::size_t;

    Trie() : root_(new Node) {}

    bool empty() const { return size_ == 0; }
    size_type size() const { return size_; }   // 不同单词的个数

    // 插入单词；若该词已存在返回 false
    bool insert(const std::string& word) {
        if (contains(word)) return false;
        Node* node = root_.get();
        for (char c : word) {
            auto& child = node->children[c];
            if (!child) child.reset(new Node);
            node = child.get();
            ++node->prefixes;          // 这个词经过该结点
        }
        ++node->words;
        ++size_;
        return true;
    }

    bool contains(const std::string& word) const {
        const Node* node = find_node(word);
        return node != nullptr && node->words > 0;
    }

    // 是否存在任意单词以 prefix 开头
    bool starts_with(const std::string& prefix) const {
        return count_prefix(prefix) > 0;
    }

    // 有多少单词以 prefix 开头
    size_type count_prefix(const std::string& prefix) const {
        if (prefix.empty()) return size_;
        const Node* node = find_node(prefix);
        return node ? node->prefixes : 0;
    }

    // 删除单词；若该词不存在返回 false。会回收沿途变空的结点
    bool erase(const std::string& word) {
        if (!contains(word)) return false;
        erase_rec(root_.get(), word, 0);
        --size_;
        return true;
    }

private:
    struct Node {
        std::unordered_map<char, std::unique_ptr<Node>> children;
        size_type words = 0;       // 以此结点结尾的单词数
        size_type prefixes = 0;    // 经过此结点的单词数
    };

    std::unique_ptr<Node> root_;
    size_type size_ = 0;

    // 沿 key 走，返回末尾结点；中途断开返回 nullptr
    const Node* find_node(const std::string& key) const {
        const Node* node = root_.get();
        for (char c : key) {
            auto it = node->children.find(c);
            if (it == node->children.end()) return nullptr;
            node = it->second.get();
        }
        return node;
    }

    // 前提：word 一定存在。回溯时递减 prefixes，并删除归零的分支
    void erase_rec(Node* node, const std::string& word, std::size_t i) {
        if (i == word.size()) {
            --node->words;
            return;
        }
        char c = word[i];
        Node* child = node->children[c].get();
        erase_rec(child, word, i + 1);
        if (--child->prefixes == 0) node->children.erase(c);  // 整支已无词，回收
    }
};

}  // namespace ceh

#endif  // CEH_STRUCTURES_TRIE_HPP
