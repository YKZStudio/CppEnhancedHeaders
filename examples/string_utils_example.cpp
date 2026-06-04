// StringUtils 使用示例
#include <ceh/utils/string_utils.hpp>

#include <iostream>
#include <string>
#include <vector>

int main() {
    std::string csv = "  Alice , Bob ,, Carol ";
    std::cout << "原始: [" << csv << "]\n";

    // 拆分 + 逐项修剪
    std::vector<std::string> fields;
    for (const auto& f : ceh::split(csv, ',')) fields.push_back(ceh::trim(f));

    std::cout << "拆分并修剪: ";
    for (const auto& f : fields) std::cout << "[" << f << "] ";
    std::cout << "\n";

    std::cout << "用 | 连接: " << ceh::join(fields, " | ") << "\n";
    std::cout << "转大写: " << ceh::to_upper(csv) << "\n";
    std::cout << "替换空格: " << ceh::replace_all("a b c", " ", "_") << "\n";
    std::cout << "以 .txt 结尾? "
              << (ceh::ends_with("report.txt", ".txt") ? "是" : "否") << "\n";
    std::cout << "分隔线: " << ceh::repeat("=", 20) << "\n";

    return 0;
}
