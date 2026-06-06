// Tree（UI 渲染）使用示例
#include <ceh/ui/tree.hpp>
#include <iostream>

int main() {
    using namespace ceh::ui;

    Tree root("CppEnhancedHeaders", Style().bold());
    auto& inc = root.add_child("include/ceh");
    inc.add_child("types", Style().fg(Color::cyan()));
    inc.add_child("structures", Style().fg(Color::cyan()));
    inc.add_child("ui", Style().fg(Color::cyan()));
    root.add_child("tests");
    root.add_child("README.md");

    std::cout << root.render() << "\n";
    return 0;
}
