// ceh/ui/rule.hpp
//
// Rule —— 水平分隔线（header-only），位于 ceh::ui。
//
// 生成占满指定列宽的横线，可在中间嵌入居中标题。借助 ceh::ui::display_width
// 正确处理 CJK 全角标题的对齐。填充字符默认用 U+2500（─）。
//
// 用法：
//     using namespace ceh::ui;
//     std::cout << horizontal_rule(40) << '\n';            // ────────...
//     std::cout << titled_rule("配置", 40) << '\n';        // ───── 配置 ─────
//     std::cout << titled_rule("日志", 40, "─",
//                              Style().fg(Color::cyan())) << '\n';

#ifndef CEH_UI_RULE_HPP
#define CEH_UI_RULE_HPP

#include <ceh/ui/style.hpp>
#include <ceh/ui/text.hpp>

#include <cstddef>
#include <string>

namespace ceh {
namespace ui {

// 用 fill 重复铺满 cols 个显示列；不足一个 fill 宽的尾部用空格补齐
inline std::string repeat_fill(const std::string& fill, std::size_t cols) {
    if (fill.empty()) return std::string(cols, ' ');
    std::size_t fw = display_width(fill);
    if (fw == 0) fw = 1;
    std::string out;
    std::size_t w = 0;
    while (w + fw <= cols) { out += fill; w += fw; }
    while (w < cols) { out.push_back(' '); ++w; }  // 收尾补齐
    return out;
}

// 纯分隔线：铺满 width 列
inline std::string horizontal_rule(std::size_t width, const std::string& fill = "─") {
    return repeat_fill(fill, width);
}

// 带样式的纯分隔线
inline std::string horizontal_rule(std::size_t width, const Style& style,
                                   const std::string& fill = "─") {
    return style.apply(repeat_fill(fill, width));
}

// 居中标题分隔线：fill… title fill…，总宽 width 列。
// 标题两侧各留一个空格；title_style 只作用于标题文字、不影响填充。
inline std::string titled_rule(const std::string& title, std::size_t width,
                               const std::string& fill = "─",
                               const Style& title_style = Style()) {
    std::size_t tw = display_width(title);
    // 放不下「填充 + 空格 + 标题 + 空格」时，退化为只输出标题
    if (tw + 2 >= width) return title_style.apply(title);
    std::size_t pad = width - tw - 2;
    std::size_t left = pad / 2;
    std::size_t right = pad - left;
    return repeat_fill(fill, left) + ' ' + title_style.apply(title) + ' ' +
           repeat_fill(fill, right);
}

}  // namespace ui
}  // namespace ceh

#endif  // CEH_UI_RULE_HPP
