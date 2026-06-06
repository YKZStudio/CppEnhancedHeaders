// ceh/ui/style.hpp
//
// Color / Style —— 终端 ANSI 颜色与文本样式（header-only），位于 ceh::ui。
//
// 这是终端 UI 线的地基：上层组件（Table / Panel / ProgressBar …）都会用它来
// 给文本着色与加粗。Style 只负责「生成 ANSI 转义序列」，本身不关心终端是否支持
// （是否真正启用由后续的 Console 组件按 TTY / NO_COLOR 决定）。
//
// Color 支持三种来源：
//   - 16 色：black()..white()（标准）、bright_black()..bright_white()（高亮）
//   - 256 色：palette(index)
//   - 真彩色：rgb(r, g, b)
//
// Style 组合前景色、背景色与若干属性（粗体 / 暗淡 / 斜体 / 下划线 / 闪烁 /
// 反显 / 隐藏 / 删除线），通过链式接口构建，再用 apply() 包裹文本。
//
// 用法：
//     using namespace ceh::ui;
//     std::cout << Style().fg(Color::red()).bold().apply("错误") << '\n';
//     std::cout << Style().fg(Color::rgb(255,128,0)).underline().apply("橙色下划线") << '\n';
//     std::cout << bold("加粗") << ' ' << colored("绿字", Color::green()) << '\n';

#ifndef CEH_UI_STYLE_HPP
#define CEH_UI_STYLE_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace ceh {
namespace ui {

// 重置所有样式的 ANSI 序列
inline const char* reset_sequence() { return "\x1b[0m"; }

class Color {
public:
    // 默认色（终端缺省前景/背景）——不输出任何颜色参数
    Color() = default;

    // ---- 16 色（标准）----
    static Color black()   { return palette(0); }
    static Color red()     { return palette(1); }
    static Color green()   { return palette(2); }
    static Color yellow()  { return palette(3); }
    static Color blue()    { return palette(4); }
    static Color magenta() { return palette(5); }
    static Color cyan()    { return palette(6); }
    static Color white()   { return palette(7); }

    // ---- 16 色（高亮）----
    static Color bright_black()   { return palette(8); }
    static Color bright_red()     { return palette(9); }
    static Color bright_green()   { return palette(10); }
    static Color bright_yellow()  { return palette(11); }
    static Color bright_blue()    { return palette(12); }
    static Color bright_magenta() { return palette(13); }
    static Color bright_cyan()    { return palette(14); }
    static Color bright_white()   { return palette(15); }

    // 终端缺省色
    static Color default_color() { return Color(); }

    // 256 色调色板（0-255）
    static Color palette(std::uint8_t index) {
        Color c;
        c.kind_ = Kind::Palette;
        c.index_ = index;
        return c;
    }

    // 真彩色
    static Color rgb(std::uint8_t r, std::uint8_t g, std::uint8_t b) {
        Color c;
        c.kind_ = Kind::Rgb;
        c.r_ = r; c.g_ = g; c.b_ = b;
        return c;
    }

    bool is_default() const { return kind_ == Kind::Default; }

    // 作为前景色的 SGR 参数（如 "31" / "38;5;200" / "38;2;255;0;0"）；默认色返回空串
    std::string fg_code() const { return code(false); }
    // 作为背景色的 SGR 参数（如 "41" / "48;5;200" / "48;2;255;0;0"）；默认色返回空串
    std::string bg_code() const { return code(true); }

private:
    enum class Kind { Default, Palette, Rgb };

    Kind kind_ = Kind::Default;
    std::uint8_t index_ = 0;
    std::uint8_t r_ = 0, g_ = 0, b_ = 0;

    std::string code(bool background) const {
        switch (kind_) {
            case Kind::Default:
                return std::string();
            case Kind::Palette:
                if (index_ < 8) {
                    // 标准色：前景 30-37，背景 40-47
                    return std::to_string((background ? 40 : 30) + index_);
                } else if (index_ < 16) {
                    // 高亮色：前景 90-97，背景 100-107
                    return std::to_string((background ? 100 : 90) + (index_ - 8));
                }
                // 扩展 256 色
                return std::string(background ? "48;5;" : "38;5;") + std::to_string(index_);
            case Kind::Rgb:
                return std::string(background ? "48;2;" : "38;2;") +
                       std::to_string(r_) + ';' + std::to_string(g_) + ';' + std::to_string(b_);
        }
        return std::string();
    }
};

class Style {
public:
    Style() = default;

    // ---- 颜色 ----
    Style& fg(const Color& c) { fg_ = c; return *this; }
    Style& bg(const Color& c) { bg_ = c; return *this; }

    // ---- 文本属性（链式开关，默认开）----
    Style& bold(bool on = true)      { return set(kBold, on); }
    Style& dim(bool on = true)       { return set(kDim, on); }
    Style& italic(bool on = true)    { return set(kItalic, on); }
    Style& underline(bool on = true) { return set(kUnderline, on); }
    Style& blink(bool on = true)     { return set(kBlink, on); }
    Style& reverse(bool on = true)   { return set(kReverse, on); }
    Style& hidden(bool on = true)    { return set(kHidden, on); }
    Style& strike(bool on = true)    { return set(kStrike, on); }

    // 是否为「空样式」（无颜色、无属性）
    bool empty() const { return attrs_ == 0 && fg_.is_default() && bg_.is_default(); }

    // 生成开启样式的 SGR 序列（如 "\x1b[1;31m"）；空样式返回空串
    std::string sgr() const {
        std::vector<std::string> parts;
        // 属性按固定顺序，保证输出可预测
        if (attrs_ & kBold)      parts.emplace_back("1");
        if (attrs_ & kDim)       parts.emplace_back("2");
        if (attrs_ & kItalic)    parts.emplace_back("3");
        if (attrs_ & kUnderline) parts.emplace_back("4");
        if (attrs_ & kBlink)     parts.emplace_back("5");
        if (attrs_ & kReverse)   parts.emplace_back("7");
        if (attrs_ & kHidden)    parts.emplace_back("8");
        if (attrs_ & kStrike)    parts.emplace_back("9");
        std::string f = fg_.fg_code();
        if (!f.empty()) parts.push_back(f);
        std::string b = bg_.bg_code();
        if (!b.empty()) parts.push_back(b);
        if (parts.empty()) return std::string();

        std::string out = "\x1b[";
        for (std::size_t i = 0; i < parts.size(); ++i) {
            if (i != 0) out.push_back(';');
            out += parts[i];
        }
        out.push_back('m');
        return out;
    }

    // 用本样式包裹文本：前置 SGR、后接 reset；空样式则原样返回
    std::string apply(const std::string& text) const {
        std::string seq = sgr();
        if (seq.empty()) return text;
        return seq + text + reset_sequence();
    }

private:
    enum Attr : unsigned {
        kBold = 1u, kDim = 2u, kItalic = 4u, kUnderline = 8u,
        kBlink = 16u, kReverse = 32u, kHidden = 64u, kStrike = 128u
    };

    Style& set(unsigned bit, bool on) {
        if (on) attrs_ |= bit;
        else    attrs_ &= ~bit;
        return *this;
    }

    Color fg_;
    Color bg_;
    unsigned attrs_ = 0;
};

// ---- 便捷自由函数 ----
inline std::string stylize(const std::string& text, const Style& s) { return s.apply(text); }
inline std::string colored(const std::string& text, const Color& fg) {
    return Style().fg(fg).apply(text);
}
inline std::string bold(const std::string& text)      { return Style().bold().apply(text); }
inline std::string italic(const std::string& text)    { return Style().italic().apply(text); }
inline std::string underline(const std::string& text) { return Style().underline().apply(text); }

}  // namespace ui
}  // namespace ceh

#endif  // CEH_UI_STYLE_HPP
