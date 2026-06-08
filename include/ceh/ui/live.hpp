// ceh/ui/live.hpp
//
// Live —— 可刷新的实时显示区域（header-only），位于 ceh::ui。
//
// 每次 update() 用新的一帧覆盖上一帧：先把光标移回区域开头并清除到屏幕末尾，
// 再打印新内容。适合进度条、Spinner、动态表格等需要原地重绘的场景。
//
// 当 enabled=false（例如输出不是终端）时退化为「每帧另起一行打印」，不产生控制序列。
//
// 用法：
//     ceh::ui::Live live;
//     for (int i = 0; i <= 100; i += 10) {
//         live.update("进度: " + std::to_string(i) + "%");
//         // ... sleep ...
//     }
//     live.done();

#ifndef CEH_UI_LIVE_HPP
#define CEH_UI_LIVE_HPP

#include <cstddef>
#include <iostream>
#include <ostream>
#include <string>

namespace ceh {
namespace ui {

namespace detail {
// 统计一帧的行数（'\n' 数 + 1，空串记 1 行）
inline std::size_t count_lines(const std::string& s) {
    std::size_t n = 1;
    for (char c : s) if (c == '\n') ++n;
    return n;
}

// 覆盖上一帧前需要发出的控制序列：回到区域开头并清除到屏幕末尾。
// prev_lines==0（首帧）时为空串。
inline std::string redraw_prefix(std::size_t prev_lines) {
    if (prev_lines == 0) return std::string();
    std::string seq = "\r";                                  // 回到行首
    if (prev_lines > 1) seq += "\x1b[" + std::to_string(prev_lines - 1) + "A";  // 上移
    seq += "\x1b[0J";                                        // 清除到屏幕末尾
    return seq;
}
}  // namespace detail

class Live {
public:
    explicit Live(std::ostream& os = std::cout, bool enabled = true)
        : os_(&os), enabled_(enabled) {}

    bool enabled() const { return enabled_; }
    void set_enabled(bool on) { enabled_ = on; }

    // 用 frame 覆盖上一帧（frame 可多行，末尾不需带换行）
    void update(const std::string& frame) {
        if (enabled_) {
            (*os_) << detail::redraw_prefix(prev_lines_) << frame << std::flush;
            prev_lines_ = detail::count_lines(frame);
        } else {
            (*os_) << frame << '\n';
        }
    }

    // 结束实时区域：补一个换行，后续输出从下一行开始
    void done() {
        if (enabled_ && prev_lines_ > 0) (*os_) << '\n';
        prev_lines_ = 0;
    }

private:
    std::ostream* os_;
    bool enabled_;
    std::size_t prev_lines_ = 0;
};

}  // namespace ui
}  // namespace ceh

#endif  // CEH_UI_LIVE_HPP
