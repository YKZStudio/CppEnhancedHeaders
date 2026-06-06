// Console 使用示例：自动检测终端能力并输出
#include <ceh/ui/console.hpp>
#include <ceh/ui/rule.hpp>
#include <iostream>

int main() {
    using namespace ceh::ui;

    Console con;   // 绑定 stdout，自动决定是否上色（非终端/NO_COLOR 时关闭）

    std::cout << "stdout 是终端吗? " << (stdout_is_terminal() ? "是" : "否") << "\n";
    std::cout << "颜色已启用? " << (con.color_enabled() ? "是" : "否") << "\n";
    std::cout << "终端宽度: " << con.width() << " 列\n";

    con.println(titled_rule("输出演示", 40));
    con.println("普通信息");
    con.println("成功", Style().fg(Color::green()).bold());
    con.println("警告", Style().fg(Color::yellow()));
    con.println("错误", Style().fg(Color::white()).bg(Color::red()).bold());

    // 即便重定向到文件（非终端），上面的彩色行也会自动退化为纯文本
    return 0;
}
