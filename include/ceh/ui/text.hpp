// ceh/ui/text.hpp
//
// Text —— 带样式的富文本片段（header-only），位于 ceh::ui。
//
// 一段 Text 由若干「片段」（Span：一串文字 + 一个 Style）拼接而成，可整体渲染为
// 带 ANSI 的字符串或纯文本。Text 还提供 **显示宽度** 计算：按 UTF-8 解码逐字符
// 累加列宽，CJK 全角字符记 2 列，从而让表格 / 面板等上层组件能正确对齐。
//
// 用法：
//     using namespace ceh::ui;
//     Text t;
//     t.append("错误：", Style().fg(Color::red()).bold());
//     t.append("文件未找到");
//     std::cout << t.render() << '\n';          // 带颜色
//     std::size_t w = t.width();                // 显示列宽（中文按 2 列）

#ifndef CEH_UI_TEXT_HPP
#define CEH_UI_TEXT_HPP

#include <ceh/ui/style.hpp>

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace ceh {
namespace ui {

namespace detail {

// 从 s[i] 处解码一个 UTF-8 码点，返回码点并把 i 推进到下一个字符。
// 遇到非法字节时按单字节处理（宽度按 1 计），保证不会卡死。
inline char32_t decode_utf8(const std::string& s, std::size_t& i) {
    unsigned char c = static_cast<unsigned char>(s[i]);
    std::size_t n = s.size();
    auto cont = [&](std::size_t k) {
        return k < n && (static_cast<unsigned char>(s[k]) & 0xC0) == 0x80;
    };
    if (c < 0x80) { ++i; return c; }
    if ((c & 0xE0) == 0xC0 && cont(i + 1)) {
        char32_t cp = (c & 0x1F) << 6 | (static_cast<unsigned char>(s[i + 1]) & 0x3F);
        i += 2; return cp;
    }
    if ((c & 0xF0) == 0xE0 && cont(i + 1) && cont(i + 2)) {
        char32_t cp = (c & 0x0F) << 12 |
                      (static_cast<unsigned char>(s[i + 1]) & 0x3F) << 6 |
                      (static_cast<unsigned char>(s[i + 2]) & 0x3F);
        i += 3; return cp;
    }
    if ((c & 0xF8) == 0xF0 && cont(i + 1) && cont(i + 2) && cont(i + 3)) {
        char32_t cp = (c & 0x07) << 18 |
                      (static_cast<unsigned char>(s[i + 1]) & 0x3F) << 12 |
                      (static_cast<unsigned char>(s[i + 2]) & 0x3F) << 6 |
                      (static_cast<unsigned char>(s[i + 3]) & 0x3F);
        i += 4; return cp;
    }
    ++i; return c;   // 非法字节
}

// 单个码点的终端显示列宽：控制字符 0，CJK / 全角 / 常见 emoji 2，其余 1
inline int code_point_width(char32_t cp) {
    if (cp == 0) return 0;
    if (cp < 0x20 || (cp >= 0x7F && cp < 0xA0)) return 0;   // 控制字符
    if ((cp >= 0x1100 && cp <= 0x115F) ||    // Hangul Jamo
        cp == 0x2329 || cp == 0x232A ||
        (cp >= 0x2E80 && cp <= 0x303E) ||    // CJK 部首 .. 康熙
        (cp >= 0x3041 && cp <= 0x33FF) ||    // 假名 .. CJK 符号
        (cp >= 0x3400 && cp <= 0x4DBF) ||    // CJK 扩展 A
        (cp >= 0x4E00 && cp <= 0x9FFF) ||    // CJK 基本汉字
        (cp >= 0xA000 && cp <= 0xA4CF) ||    // 彝文
        (cp >= 0xAC00 && cp <= 0xD7A3) ||    // 谚文音节
        (cp >= 0xF900 && cp <= 0xFAFF) ||    // CJK 兼容
        (cp >= 0xFE30 && cp <= 0xFE4F) ||    // CJK 兼容形式
        (cp >= 0xFF00 && cp <= 0xFF60) ||    // 全角 ASCII
        (cp >= 0xFFE0 && cp <= 0xFFE6) ||    // 全角符号
        (cp >= 0x1F300 && cp <= 0x1FAFF) ||  // 常见 emoji（近似）
        (cp >= 0x20000 && cp <= 0x3FFFD)) {  // CJK 扩展 B 及以上
        return 2;
    }
    return 1;
}

}  // namespace detail

// 计算一段 UTF-8 文本的终端显示列宽
inline std::size_t display_width(const std::string& s) {
    std::size_t w = 0;
    for (std::size_t i = 0; i < s.size();) {
        char32_t cp = detail::decode_utf8(s, i);
        w += static_cast<std::size_t>(detail::code_point_width(cp));
    }
    return w;
}

class Text {
public:
    struct Span {
        std::string text;
        Style style;
    };

    Text() = default;
    Text(const std::string& text, const Style& style = Style()) {
        if (!text.empty()) spans_.push_back(Span{text, style});
    }

    Text& append(const std::string& text, const Style& style = Style()) {
        if (!text.empty()) spans_.push_back(Span{text, style});
        return *this;
    }
    Text& append(const Text& other) {
        for (const auto& sp : other.spans_) spans_.push_back(sp);
        return *this;
    }
    Text& operator+=(const Text& other) { return append(other); }

    bool empty() const { return spans_.empty(); }
    const std::vector<Span>& spans() const { return spans_; }

    // 纯文本（无样式、无转义序列）
    std::string plain() const {
        std::string out;
        for (const auto& sp : spans_) out += sp.text;
        return out;
    }

    // 渲染：with_style=true 时带 ANSI 样式，否则等同 plain()
    std::string render(bool with_style = true) const {
        if (!with_style) return plain();
        std::string out;
        for (const auto& sp : spans_) out += sp.style.apply(sp.text);
        return out;
    }

    // 显示列宽（中文等全角字符按 2 列）
    std::size_t width() const {
        std::size_t w = 0;
        for (const auto& sp : spans_) w += display_width(sp.text);
        return w;
    }

private:
    std::vector<Span> spans_;
};

}  // namespace ui
}  // namespace ceh

#endif  // CEH_UI_TEXT_HPP
