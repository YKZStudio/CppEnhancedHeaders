// ceh/ui/panel.hpp
//
// Panel —— 带边框与可选标题的面板（header-only），位于 ceh::ui。
//
// 把一段（可多行）文本框起来，自动按最宽行决定面板宽度；标题嵌在顶边框上，
// 可左/中/右对齐。边框与标题可分别着色。
//
// 用法：
//     using namespace ceh::ui;
//     Panel p("Hello\n世界");
//     p.set_title("提示").set_border(BorderStyle::Rounded);
//     std::cout << p.render() << '\n';

#ifndef CEH_UI_PANEL_HPP
#define CEH_UI_PANEL_HPP

#include <ceh/ui/align.hpp>
#include <ceh/ui/border.hpp>
#include <ceh/ui/style.hpp>
#include <ceh/ui/text.hpp>

#include <algorithm>
#include <cstddef>
#include <string>
#include <vector>

namespace ceh {
namespace ui {

class Panel {
public:
    explicit Panel(const std::string& content = "") { set_content(content); }

    Panel& set_content(const std::string& content) {
        lines_ = split_lines(content);
        return *this;
    }
    Panel& set_title(const std::string& title) { title_ = title; return *this; }
    Panel& set_border(BorderStyle style) { border_ = style; return *this; }
    Panel& set_border_style(const Style& s) { border_style_ = s; return *this; }
    Panel& set_title_style(const Style& s) { title_style_ = s; return *this; }
    Panel& set_title_align(Align a) { title_align_ = a; return *this; }
    Panel& set_padding(std::size_t cells) { padding_ = cells; return *this; }
    Panel& set_width(std::size_t content_width) { fixed_width_ = content_width; return *this; }

    std::string render(bool with_style = true) const {
        const BorderChars b = border_chars(border_);
        std::size_t cw = content_width();
        std::size_t inner = cw + 2 * padding_;     // 两条竖线之间的列数

        auto bs = [&](const std::string& s) {
            return (with_style && !border_style_.empty()) ? border_style_.apply(s) : s;
        };
        std::string pad(padding_, ' ');

        std::string out;
        out += top_border(b, inner, with_style, bs);
        out += '\n';
        for (const auto& line : lines_) {
            out += bs(b.vertical);
            out += pad + pad_to(line, cw, Align::Left) + pad;
            out += bs(b.vertical);
            out += '\n';
        }
        out += bs(b.bottom_left) + bs(repeat(b.horizontal, inner)) + bs(b.bottom_right);
        return out;
    }

private:
    std::vector<std::string> lines_{""};
    std::string title_;
    BorderStyle border_ = BorderStyle::Square;
    Style border_style_;
    Style title_style_;
    Align title_align_ = Align::Center;
    std::size_t padding_ = 1;
    std::size_t fixed_width_ = 0;

    static std::vector<std::string> split_lines(const std::string& s) {
        std::vector<std::string> out;
        std::string cur;
        for (char c : s) {
            if (c == '\n') { out.push_back(cur); cur.clear(); }
            else cur.push_back(c);
        }
        out.push_back(cur);
        return out;
    }

    static std::string repeat(const std::string& unit, std::size_t n) {
        std::string out;
        out.reserve(unit.size() * n);
        for (std::size_t i = 0; i < n; ++i) out += unit;
        return out;
    }

    std::size_t content_width() const {
        if (fixed_width_ > 0) return fixed_width_;
        std::size_t w = display_width(title_);   // 保证标题放得下
        for (const auto& line : lines_) w = std::max(w, display_width(line));
        return w;
    }

    template <typename BS>
    std::string top_border(const BorderChars& b, std::size_t inner,
                          bool with_style, BS bs) const {
        if (title_.empty())
            return bs(b.top_left) + bs(repeat(b.horizontal, inner)) + bs(b.top_right);

        std::size_t tw = display_width(title_) + 2;   // 标题两侧各一个空格
        std::size_t fill = (tw <= inner) ? inner - tw : 0;
        std::size_t left = 0, right = 0;
        switch (title_align_) {
            case Align::Left:   left = 0;          right = fill;        break;
            case Align::Right:  left = fill;       right = 0;           break;
            case Align::Center: left = fill / 2;   right = fill - left; break;
        }
        std::string title_piece = " " +
            ((with_style && !title_style_.empty()) ? title_style_.apply(title_) : title_) + " ";
        return bs(b.top_left) + bs(repeat(b.horizontal, left)) + title_piece +
               bs(repeat(b.horizontal, right)) + bs(b.top_right);
    }
};

}  // namespace ui
}  // namespace ceh

#endif  // CEH_UI_PANEL_HPP
