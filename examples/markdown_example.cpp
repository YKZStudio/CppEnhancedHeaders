// Markdown 使用示例
#include <ceh/ui/markdown.hpp>
#include <iostream>

int main() {
    const std::string doc =
        "# CppEnhancedHeaders\n"
        "\n"
        "一套 **header-only** 的 C++ 增强库。\n"
        "\n"
        "## 特性\n"
        "- 零依赖，`#include` 即用\n"
        "- 现代 C++17\n"
        "\n"
        "> 提示：详见 `README.md`\n"
        "\n"
        "---\n";

    ceh::ui::Markdown md;
    md.set_width(40);
    std::cout << md.render(doc) << "\n";
    return 0;
}
