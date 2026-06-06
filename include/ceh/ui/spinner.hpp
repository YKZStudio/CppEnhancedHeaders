// ceh/ui/spinner.hpp
//
// Spinner —— 加载动画（header-only），位于 ceh::ui。
//
// 持有一组帧，next() 取下一帧（到末尾回卷）。配合定时重画即可形成转动效果。
// 内置若干常见帧集（盲文点阵 / 直线 / 箭头 / 时钟）。
//
// 用法：
//     using namespace ceh::ui;
//     Spinner sp = Spinner::dots();
//     for (int i = 0; i < 20; ++i)
//         std::cout << '\r' << sp.next() << " 加载中..." << std::flush /* + sleep */;

#ifndef CEH_UI_SPINNER_HPP
#define CEH_UI_SPINNER_HPP

#include <ceh/ui/style.hpp>

#include <cstddef>
#include <string>
#include <vector>

namespace ceh {
namespace ui {

class Spinner {
public:
    explicit Spinner(std::vector<std::string> frames, const Style& style = Style())
        : frames_(std::move(frames)), style_(style) {
        if (frames_.empty()) frames_.push_back("");
    }

    // ---- 预设帧集 ----
    static Spinner dots() {
        return Spinner({"⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"});
    }
    static Spinner line()  { return Spinner({"-", "\\", "|", "/"}); }
    static Spinner arrow() { return Spinner({"←", "↖", "↑", "↗", "→", "↘", "↓", "↙"}); }
    static Spinner clock() {
        return Spinner({"🕐", "🕑", "🕒", "🕓", "🕔", "🕕",
                        "🕖", "🕗", "🕘", "🕙", "🕚", "🕛"});
    }

    Spinner& set_style(const Style& s) { style_ = s; return *this; }

    std::size_t frame_count() const { return frames_.size(); }
    std::size_t index() const { return index_; }
    void reset() { index_ = 0; }

    // 当前帧（不前进）
    std::string current(bool with_style = true) const {
        return with_style ? style_.apply(frames_[index_]) : frames_[index_];
    }

    // 前进到下一帧并返回它（到末尾回卷到开头）
    std::string next(bool with_style = true) {
        index_ = (index_ + 1) % frames_.size();
        return current(with_style);
    }

private:
    std::vector<std::string> frames_;
    Style style_;
    std::size_t index_ = 0;
};

}  // namespace ui
}  // namespace ceh

#endif  // CEH_UI_SPINNER_HPP
