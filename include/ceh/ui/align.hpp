// ceh/ui/align.hpp
//
// 文本对齐工具（header-only），位于 ceh::ui。按「显示列宽」对齐，正确处理 CJK 全角。
//
// 用法：
//     ceh::ui::pad_to("hi", 6);                       // "hi    "
//     ceh::ui::pad_to("hi", 6, ceh::ui::Align::Right); // "    hi"
//     ceh::ui::pad_to("中", 6, ceh::ui::Align::Center);// "  中  "

#ifndef CEH_UI_ALIGN_HPP
#define CEH_UI_ALIGN_HPP

#include <ceh/ui/text.hpp>

#include <cstddef>
#include <string>

namespace ceh {
namespace ui {

enum class Align { Left, Right, Center };

// 用 fill 把 text 填充到显示列宽 width（按 display_width 计）。
// text 本身已达到或超过 width 时原样返回（不截断）。
inline std::string pad_to(const std::string& text, std::size_t width,
                          Align align = Align::Left, char fill = ' ') {
    std::size_t w = display_width(text);
    if (w >= width) return text;
    std::size_t diff = width - w;
    switch (align) {
        case Align::Left:
            return text + std::string(diff, fill);
        case Align::Right:
            return std::string(diff, fill) + text;
        case Align::Center: {
            std::size_t left = diff / 2;
            std::size_t right = diff - left;
            return std::string(left, fill) + text + std::string(right, fill);
        }
    }
    return text;
}

}  // namespace ui
}  // namespace ceh

#endif  // CEH_UI_ALIGN_HPP
