// Table 使用示例
#include <ceh/ui/table.hpp>
#include <iostream>

int main() {
    using namespace ceh::ui;

    Table t;
    t.set_border(BorderStyle::Rounded);
    t.set_headers({"语言", "诞生", "范式"});
    t.set_header_style(Style().bold().fg(Color::cyan()));
    t.set_align(1, Align::Right);
    t.add_row({"C++", "1985", "多范式"});
    t.add_row({"Python", "1991", "面向对象"});
    t.add_row({"Rust", "2010", "系统级"});

    std::cout << t.render() << "\n";
    return 0;
}
