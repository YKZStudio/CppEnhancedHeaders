// ceh/ui/prompt.hpp
//
// Prompt —— 交互式输入（header-only），位于 ceh::ui。
//
// 在终端上提问并读取输入：自由文本、是/否确认、从选项中单选。输入/输出流均可注入
// （默认 std::cin / std::cout），便于测试与重定向。
//
// 用法：
//     ceh::ui::Prompt p;
//     std::string name = p.input("你的名字");
//     if (p.confirm("继续吗", true)) { ... }
//     std::size_t i = p.select("选择颜色", {"红", "绿", "蓝"});

#ifndef CEH_UI_PROMPT_HPP
#define CEH_UI_PROMPT_HPP

#include <cctype>
#include <cstddef>
#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <vector>

namespace ceh {
namespace ui {

class Prompt {
public:
    explicit Prompt(std::istream& in = std::cin, std::ostream& out = std::cout)
        : in_(&in), out_(&out) {}

    // 读取一行自由文本；EOF 时返回空串
    std::string input(const std::string& question) {
        (*out_) << question << ": ";
        out_->flush();
        std::string line;
        if (!std::getline(*in_, line)) return std::string();
        return line;
    }

    // 是/否确认。回车（空输入）采用 default_yes；EOF 同样返回默认值。
    bool confirm(const std::string& question, bool default_yes = false) {
        (*out_) << question << (default_yes ? " [Y/n]: " : " [y/N]: ");
        out_->flush();
        std::string line;
        if (!std::getline(*in_, line)) return default_yes;
        line = trimmed(line);
        if (line.empty()) return default_yes;
        char c = static_cast<char>(std::tolower(static_cast<unsigned char>(line[0])));
        return c == 'y';
    }

    // 从 options 中单选，返回 0 基下标。输入为 1 基序号；非法则重试。
    // 选项为空或 EOF 时返回 options.size()（表示未选择）。
    std::size_t select(const std::string& question, const std::vector<std::string>& options) {
        if (options.empty()) return 0;
        while (true) {
            (*out_) << question << ":\n";
            for (std::size_t i = 0; i < options.size(); ++i)
                (*out_) << "  " << (i + 1) << ") " << options[i] << '\n';
            (*out_) << "> ";
            out_->flush();

            std::string line;
            if (!std::getline(*in_, line)) return options.size();
            line = trimmed(line);
            std::size_t idx = 0;
            if (parse_index(line, idx) && idx >= 1 && idx <= options.size())
                return idx - 1;
            (*out_) << "无效选择，请重试。\n";
        }
    }

private:
    std::istream* in_;
    std::ostream* out_;

    static std::string trimmed(const std::string& s) {
        std::size_t b = s.find_first_not_of(" \t\r\n");
        if (b == std::string::npos) return std::string();
        std::size_t e = s.find_last_not_of(" \t\r\n");
        return s.substr(b, e - b + 1);
    }

    static bool parse_index(const std::string& s, std::size_t& out) {
        if (s.empty()) return false;
        std::size_t v = 0;
        for (char c : s) {
            if (c < '0' || c > '9') return false;
            v = v * 10 + static_cast<std::size_t>(c - '0');
        }
        out = v;
        return true;
    }
};

}  // namespace ui
}  // namespace ceh

#endif  // CEH_UI_PROMPT_HPP
