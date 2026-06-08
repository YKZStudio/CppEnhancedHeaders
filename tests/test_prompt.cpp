// Prompt 单元测试（零依赖，基于 assert；用字符串流注入输入/捕获输出）
#include <ceh/ui/prompt.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using ceh::ui::Prompt;

static int g_checks = 0;

#define CHECK_TRUE(expr)                                               \
    do { ++g_checks; if (!(expr)) {                                    \
        std::cerr << "FAIL [" << __LINE__ << "] " #expr "\n"; std::exit(1);} } while (0)

int main() {
    // input：读取一行
    {
        std::istringstream in("Alice\n");
        std::ostringstream out;
        Prompt p(in, out);
        CHECK_TRUE(p.input("名字") == "Alice");
        CHECK_TRUE(out.str().find("名字: ") != std::string::npos);
    }
    // input：EOF 返回空串
    {
        std::istringstream in("");
        std::ostringstream out;
        Prompt p(in, out);
        CHECK_TRUE(p.input("x") == "");
    }

    // confirm：各种输入
    {
        std::istringstream in("y\nn\n\nYes\n");
        std::ostringstream out;
        Prompt p(in, out);
        CHECK_TRUE(p.confirm("a") == true);          // y
        CHECK_TRUE(p.confirm("b") == false);         // n
        CHECK_TRUE(p.confirm("c", true) == true);    // 空 -> 默认 true
        CHECK_TRUE(p.confirm("d") == true);          // Yes
    }
    // confirm：EOF 返回默认值
    {
        std::istringstream in("");
        std::ostringstream out;
        Prompt p(in, out);
        CHECK_TRUE(p.confirm("q", false) == false);
        CHECK_TRUE(p.confirm("q", true) == true);
    }

    // select：合法选择返回 0 基下标
    {
        std::istringstream in("2\n");
        std::ostringstream out;
        Prompt p(in, out);
        CHECK_TRUE(p.select("颜色", {"红", "绿", "蓝"}) == 1);
        CHECK_TRUE(out.str().find("1) 红") != std::string::npos);
        CHECK_TRUE(out.str().find("3) 蓝") != std::string::npos);
    }
    // select：非法输入会重试，直到合法
    {
        std::istringstream in("9\nabc\n3\n");
        std::ostringstream out;
        Prompt p(in, out);
        CHECK_TRUE(p.select("pick", {"a", "b", "c"}) == 2);
        CHECK_TRUE(out.str().find("无效选择") != std::string::npos);
    }
    // select：EOF 返回 size（表示未选择）
    {
        std::istringstream in("");
        std::ostringstream out;
        Prompt p(in, out);
        CHECK_TRUE(p.select("pick", {"a", "b"}) == 2);
    }

    std::cout << "All Prompt tests passed (" << g_checks << " checks).\n";
    return 0;
}
