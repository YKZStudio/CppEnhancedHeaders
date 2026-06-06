// Table 单元测试（零依赖，基于 assert）
#include <ceh/ui/table.hpp>

#include <iostream>
#include <string>

using namespace ceh::ui;

static int g_checks = 0;

#define CHECK_EQ(expr, expected)                                       \
    do {                                                               \
        ++g_checks;                                                    \
        std::string got = (expr);                                     \
        std::string exp = (expected);                                 \
        if (got != exp) {                                             \
            std::cerr << "FAIL [" << __LINE__ << "]\n--- got ---\n"   \
                      << got << "\n--- expected ---\n" << exp << "\n";\
            std::exit(1);                                             \
        }                                                            \
    } while (0)

#define CHECK_TRUE(expr)                                               \
    do { ++g_checks; if (!(expr)) {                                    \
        std::cerr << "FAIL [" << __LINE__ << "] " #expr "\n"; std::exit(1);} } while (0)

int main() {
    // ASCII 边框 + 右对齐数字列
    {
        Table t;
        t.set_border(BorderStyle::Ascii);
        t.set_headers({"Name", "Age"});
        t.set_align(1, Align::Right);
        t.add_row({"Bob", "30"});
        t.add_row({"Alice", "7"});
        CHECK_EQ(t.render(false),
                 "+-------+-----+\n"
                 "| Name  | Age |\n"
                 "+-------+-----+\n"
                 "| Bob   |  30 |\n"
                 "| Alice |   7 |\n"
                 "+-------+-----+");
        CHECK_TRUE(t.columns() == 2);
        CHECK_TRUE(t.rows() == 2);
    }

    // 无表头
    {
        Table t;
        t.set_border(BorderStyle::Ascii);
        t.add_row({"a", "bb"});
        t.add_row({"ccc", "d"});
        CHECK_EQ(t.render(false),
                 "+-----+----+\n"
                 "| a   | bb |\n"
                 "| ccc | d  |\n"
                 "+-----+----+");
    }

    // CJK 列宽（中文按 2 列对齐）
    {
        Table t;
        t.set_border(BorderStyle::Ascii);
        t.set_headers({"名字", "v"});
        t.add_row({"a", "x"});
        // “名字” 宽 4，故第一列内容区宽 4
        CHECK_EQ(t.render(false),
                 "+------+---+\n"
                 "| 名字 | v |\n"
                 "+------+---+\n"
                 "| a    | x |\n"
                 "+------+---+");
    }

    // 表头样式：启用后该行应含 ANSI 序列，关闭样式则没有
    {
        Table t;
        t.set_border(BorderStyle::Ascii);
        t.set_headers({"H"});
        t.set_header_style(Style().bold());
        CHECK_TRUE(t.render(true).find("\x1b[1m") != std::string::npos);
        CHECK_TRUE(t.render(false).find("\x1b[") == std::string::npos);
    }

    std::cout << "All Table tests passed (" << g_checks << " checks).\n";
    return 0;
}
