// Prompt 使用示例：交互式问答（运行后按提示输入）
#include <ceh/ui/prompt.hpp>
#include <iostream>

int main() {
    using namespace ceh::ui;

    Prompt p;
    std::string name = p.input("你的名字");

    std::size_t color = p.select("喜欢的颜色", {"红", "绿", "蓝"});
    static const char* names[] = {"红", "绿", "蓝"};

    if (p.confirm("确认提交", true)) {
        std::cout << "你好 " << name << "，颜色 = "
                  << (color < 3 ? names[color] : "未选") << "\n";
    } else {
        std::cout << "已取消\n";
    }
    return 0;
}
