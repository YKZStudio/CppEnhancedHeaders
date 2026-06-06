// ceh/ui/markdown.hpp
//
// Markdown —— 简易 Markdown 渲染（header-only），位于 ceh::ui。
//
// 把常见 Markdown 子集渲染成可在终端显示的（可选带 ANSI 样式）文本：
//   - 标题 # ~ ######
//   - 无序列表 - / * / +        -> • 项
//   - 引用 > quote              -> │ 前缀
//   - 分隔线 --- / *** / ___    -> 横线
//   - 围栏代码块 ``` ... ```
//   - 行内：**粗体**、*斜体*、`代码`
//
// 不追求完整 CommonMark，只覆盖日常够用的部分。复用 ceh 的字符串工具与 Rule。
//
// 用法：
//     std::cout << ceh::ui::Markdown().render("# 标题\n- 项目 **重点**\n") << '\n';

#ifndef CEH_UI_MARKDOWN_HPP
#define CEH_UI_MARKDOWN_HPP

#include <ceh/ui/rule.hpp>
#include <ceh/ui/style.hpp>
#include <ceh/utils/string_utils.hpp>

#include <cstddef>
#include <string>
#include <vector>

namespace ceh {
namespace ui {

class Markdown {
public:
    Markdown& set_width(std::size_t w) { width_ = w; return *this; }
    Markdown& set_heading_style(const Style& s) { heading_style_ = s; return *this; }

    std::string render(const std::string& md, bool with_style = true) const {
        std::vector<std::string> lines = ceh::split(md, '\n');
        std::vector<std::string> out;
        bool in_code = false;

        for (const std::string& line : lines) {
            std::string trimmed = ceh::trim(line);

            // 代码块围栏开关
            if (ceh::starts_with(trimmed, "```")) {
                in_code = !in_code;
                continue;   // 围栏行本身不输出
            }
            if (in_code) {
                out.push_back(with_style ? code_block_style_.apply(line) : line);
                continue;
            }

            // 分隔线
            if (is_rule(trimmed)) {
                out.push_back(horizontal_rule(width_));
                continue;
            }

            // 标题
            std::size_t h = heading_level(trimmed);
            if (h > 0) {
                std::string txt = trimmed.substr(h + 1);
                if (with_style) {
                    Style hs = heading_style_;
                    if (h == 1) hs.underline();
                    out.push_back(hs.apply(txt));
                } else {
                    out.push_back(txt);
                }
                continue;
            }

            // 引用
            if (ceh::starts_with(trimmed, "> ")) {
                out.push_back("│ " + inline_format(trimmed.substr(2), with_style));
                continue;
            }

            // 无序列表
            if (is_unordered_item(trimmed)) {
                out.push_back("  • " + inline_format(trimmed.substr(2), with_style));
                continue;
            }

            // 普通段落（保留原始缩进）
            out.push_back(inline_format(line, with_style));
        }

        return ceh::join(out, "\n");
    }

private:
    std::size_t width_ = 60;
    Style heading_style_ = Style().bold().fg(Color::cyan());
    Style bold_style_ = Style().bold();
    Style italic_style_ = Style().italic();
    Style code_inline_style_ = Style().fg(Color::yellow());
    Style code_block_style_ = Style().fg(Color::bright_black());

    static bool is_rule(const std::string& t) {
        if (t.size() < 3) return false;
        char c = t[0];
        if (c != '-' && c != '*' && c != '_') return false;
        for (char ch : t) if (ch != c) return false;
        return true;
    }

    static std::size_t heading_level(const std::string& t) {
        std::size_t h = 0;
        while (h < t.size() && t[h] == '#') ++h;
        if (h >= 1 && h <= 6 && h < t.size() && t[h] == ' ') return h;
        return 0;
    }

    static bool is_unordered_item(const std::string& t) {
        return t.size() >= 2 && (t[0] == '-' || t[0] == '*' || t[0] == '+') && t[1] == ' ';
    }

    // 处理行内 `代码` / **粗体** / *斜体*（顺序很重要：先 code，再 ** 后 *）
    std::string inline_format(const std::string& line, bool with_style) const {
        std::string r = line;
        r = wrap_pairs(r, "`", code_inline_style_, with_style);
        r = wrap_pairs(r, "**", bold_style_, with_style);
        r = wrap_pairs(r, "*", italic_style_, with_style);
        return r;
    }

    static std::string wrap_pairs(const std::string& s, const std::string& marker,
                                 const Style& style, bool with_style) {
        std::string out;
        std::size_t i = 0, ml = marker.size();
        while (true) {
            std::size_t p = s.find(marker, i);
            if (p == std::string::npos) { out += s.substr(i); break; }
            std::size_t q = s.find(marker, p + ml);
            if (q == std::string::npos) { out += s.substr(i); break; }  // 无配对，原样
            out += s.substr(i, p - i);
            std::string inner = s.substr(p + ml, q - (p + ml));
            out += with_style ? style.apply(inner) : inner;
            i = q + ml;
        }
        return out;
    }
};

}  // namespace ui
}  // namespace ceh

#endif  // CEH_UI_MARKDOWN_HPP
