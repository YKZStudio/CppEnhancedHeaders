// ceh/ui/layout.hpp
//
// Layout —— 盒式布局（header-only），位于 ceh::ui。
//
// 把若干「多行文本块」横向（hbox）或纵向（vbox）拼接，类似 FTXUI 的 hbox/vbox。
// 横向拼接时会把每块补齐到等高、每行补齐到块宽，从而整齐并排——并且宽度计算
// 会跳过 ANSI 转义序列，因此可以正确并排带颜色的 Panel / Table 等渲染结果。
//
// 用法：
//     using namespace ceh::ui;
//     std::string a = Panel("左").render();
//     std::string b = Panel("右").render();
//     std::cout << hbox({a, b}, 2) << '\n';   // 两个面板并排，间隔 2 列

#ifndef CEH_UI_LAYOUT_HPP
#define CEH_UI_LAYOUT_HPP

#include <ceh/ui/text.hpp>

#include <algorithm>
#include <cstddef>
#include <string>
#include <vector>

namespace ceh {
namespace ui {

namespace detail {

// 显示列宽，但跳过 ANSI CSI 序列（ESC [ … 字母），用于对齐带样式的文本块
inline std::size_t visual_width(const std::string& s) {
    std::size_t w = 0;
    for (std::size_t i = 0; i < s.size();) {
        if (static_cast<unsigned char>(s[i]) == 0x1b && i + 1 < s.size() && s[i + 1] == '[') {
            i += 2;
            while (i < s.size() && !(s[i] >= '@' && s[i] <= '~')) ++i;  // 参数字节
            if (i < s.size()) ++i;                                      // 结尾字母
        } else {
            char32_t cp = decode_utf8(s, i);
            w += static_cast<std::size_t>(code_point_width(cp));
        }
    }
    return w;
}

inline std::vector<std::string> to_lines(const std::string& block) {
    std::vector<std::string> out;
    std::string cur;
    for (char c : block) {
        if (c == '\n') { out.push_back(cur); cur.clear(); }
        else cur.push_back(c);
    }
    out.push_back(cur);
    return out;
}

}  // namespace detail

// 横向并排多个块；gap 为块之间的空格列数
inline std::string hbox(const std::vector<std::string>& blocks, std::size_t gap = 0) {
    if (blocks.empty()) return std::string();

    std::vector<std::vector<std::string>> cols;
    std::vector<std::size_t> widths;
    std::size_t height = 0;
    for (const auto& b : blocks) {
        auto lines = detail::to_lines(b);
        std::size_t w = 0;
        for (const auto& ln : lines) w = std::max(w, detail::visual_width(ln));
        height = std::max(height, lines.size());
        widths.push_back(w);
        cols.push_back(std::move(lines));
    }

    std::string gap_str(gap, ' ');
    std::string out;
    for (std::size_t r = 0; r < height; ++r) {
        for (std::size_t c = 0; c < cols.size(); ++c) {
            const std::string& line = (r < cols[c].size()) ? cols[c][r] : std::string();
            out += line;
            std::size_t pad = widths[c] - detail::visual_width(line);
            out += std::string(pad, ' ');
            if (c + 1 < cols.size()) out += gap_str;
        }
        if (r + 1 < height) out += '\n';
    }
    return out;
}

// 纵向堆叠多个块（按出现顺序，块之间换行）
inline std::string vbox(const std::vector<std::string>& blocks) {
    std::string out;
    for (std::size_t i = 0; i < blocks.size(); ++i) {
        if (i != 0) out += '\n';
        out += blocks[i];
    }
    return out;
}

}  // namespace ui
}  // namespace ceh

#endif  // CEH_UI_LAYOUT_HPP
