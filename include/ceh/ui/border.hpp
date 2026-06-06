// ceh/ui/border.hpp
//
// 边框字符集（header-only），位于 ceh::ui。供 Table / Panel 等需要画框的组件复用。
//
// 提供若干预设风格（方角 / 圆角 / 双线 / ASCII / 无边框），每种风格给出
// 画一个矩形框 + 内部分隔所需的全部制表符。
//
// 用法：
//     auto b = ceh::ui::border_chars(ceh::ui::BorderStyle::Rounded);
//     std::cout << b.top_left << b.horizontal << b.top_right << '\n';

#ifndef CEH_UI_BORDER_HPP
#define CEH_UI_BORDER_HPP

#include <string>

namespace ceh {
namespace ui {

enum class BorderStyle {
    Square,    // ┌─┐ │ └─┘
    Rounded,   // ╭─╮ │ ╰─╯
    Double,    // ╔═╗ ║ ╚═╝
    Ascii,     // +-+ | +-+
    None       // 不画线（全部空白）
};

struct BorderChars {
    std::string top_left, top_right, bottom_left, bottom_right;
    std::string horizontal, vertical;
    std::string tee_down, tee_up, tee_left, tee_right, cross;  // ┬ ┴ ┤ ├ ┼
};

inline BorderChars border_chars(BorderStyle style) {
    switch (style) {
        case BorderStyle::Square:
            return {"┌", "┐", "└", "┘", "─", "│", "┬", "┴", "┤", "├", "┼"};
        case BorderStyle::Rounded:
            return {"╭", "╮", "╰", "╯", "─", "│", "┬", "┴", "┤", "├", "┼"};
        case BorderStyle::Double:
            return {"╔", "╗", "╚", "╝", "═", "║", "╦", "╩", "╣", "╠", "╬"};
        case BorderStyle::Ascii:
            return {"+", "+", "+", "+", "-", "|", "+", "+", "+", "+", "+"};
        case BorderStyle::None:
            return {" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "};
    }
    return {"+", "+", "+", "+", "-", "|", "+", "+", "+", "+", "+"};
}

}  // namespace ui
}  // namespace ceh

#endif  // CEH_UI_BORDER_HPP
