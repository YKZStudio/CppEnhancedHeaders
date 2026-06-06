// ceh/ui/tree.hpp
//
// Tree —— 树形结构的缩进渲染（header-only），位于 ceh::ui。
//
// 每个 Tree 既是一个结点也是一棵子树，递归持有子结点。render() 用 ├─ └─ │
// 连接线画出层级。每个结点可带样式。子结点容器用 std::list：它支持不完整类型
// （递归持有自身），且 push_back 不使已有元素引用失效，方便链式构建。
//
// 用法：
//     using namespace ceh::ui;
//     Tree root("项目");
//     auto& src = root.add_child("src");
//     src.add_child("main.cpp");
//     src.add_child("util.hpp");
//     root.add_child("README.md");
//     std::cout << root.render() << '\n';

#ifndef CEH_UI_TREE_HPP
#define CEH_UI_TREE_HPP

#include <ceh/ui/style.hpp>

#include <cstddef>
#include <list>
#include <string>

namespace ceh {
namespace ui {

class Tree {
public:
    explicit Tree(const std::string& label, const Style& style = Style())
        : label_(label), style_(style) {}

    // 追加子结点，返回其引用（可继续在其上添加孙结点）
    Tree& add_child(const std::string& label, const Style& style = Style()) {
        children_.emplace_back(label, style);
        return children_.back();
    }
    Tree& add_child(const Tree& subtree) {
        children_.push_back(subtree);
        return children_.back();
    }

    const std::string& label() const { return label_; }
    std::size_t child_count() const { return children_.size(); }

    std::string render(bool with_style = true) const {
        std::string out = styled_label(with_style);
        render_children(out, "", with_style);
        return out;
    }

private:
    std::string label_;
    Style style_;
    std::list<Tree> children_;

    std::string styled_label(bool with_style) const {
        return with_style ? style_.apply(label_) : label_;
    }

    void render_children(std::string& out, const std::string& prefix, bool with_style) const {
        std::size_t i = 0, n = children_.size();
        for (const Tree& child : children_) {   // std::list 无 operator[]，改用迭代
            const bool last = (++i == n);
            out += '\n';
            out += prefix;
            out += last ? "└─ " : "├─ ";
            out += child.styled_label(with_style);
            // 子树的延续：最后一个孩子下方留空，否则保留竖线
            child.render_children(out, prefix + (last ? "   " : "│  "), with_style);
        }
    }
};

}  // namespace ui
}  // namespace ceh

#endif  // CEH_UI_TREE_HPP
