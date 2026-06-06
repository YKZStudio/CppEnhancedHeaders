// ceh/ui/progress_bar.hpp
//
// ProgressBar —— 进度条（header-only），位于 ceh::ui。
//
// 把 [0,1] 的进度渲染成一行字符条，可附带百分比。填充/空白字符与样式可定制。
// 只负责生成字符串，刷新（回到行首重画）交给调用方或后续的 Live 组件。
//
// 用法：
//     using namespace ceh::ui;
//     ProgressBar bar;
//     bar.set_width(30).set_fraction(3, 4);
//     std::cout << '\r' << bar.render() << std::flush;

#ifndef CEH_UI_PROGRESS_BAR_HPP
#define CEH_UI_PROGRESS_BAR_HPP

#include <ceh/ui/style.hpp>

#include <cmath>
#include <cstddef>
#include <string>

namespace ceh {
namespace ui {

class ProgressBar {
public:
    ProgressBar& set_progress(double p) {
        progress_ = p < 0.0 ? 0.0 : (p > 1.0 ? 1.0 : p);   // 夹到 [0,1]
        return *this;
    }
    ProgressBar& set_fraction(double done, double total) {
        return set_progress(total > 0.0 ? done / total : 0.0);
    }
    ProgressBar& set_width(std::size_t cells) { width_ = cells; return *this; }
    ProgressBar& set_chars(const std::string& fill, const std::string& empty) {
        fill_ = fill; empty_ = empty; return *this;
    }
    ProgressBar& set_fill_style(const Style& s) { fill_style_ = s; return *this; }
    ProgressBar& set_empty_style(const Style& s) { empty_style_ = s; return *this; }
    ProgressBar& set_show_percent(bool on) { show_percent_ = on; return *this; }

    double progress() const { return progress_; }
    int percent() const { return static_cast<int>(std::lround(progress_ * 100.0)); }

    std::string render(bool with_style = true) const {
        std::size_t filled = static_cast<std::size_t>(std::lround(progress_ * static_cast<double>(width_)));
        if (filled > width_) filled = width_;

        std::string fpart = repeat(fill_, filled);
        std::string epart = repeat(empty_, width_ - filled);
        std::string bar;
        if (with_style) {
            bar = fill_style_.apply(fpart) + empty_style_.apply(epart);
        } else {
            bar = fpart + epart;
        }
        if (show_percent_) bar += " " + std::to_string(percent()) + "%";
        return bar;
    }

private:
    double progress_ = 0.0;
    std::size_t width_ = 30;
    std::string fill_ = "█";
    std::string empty_ = "░";
    Style fill_style_;
    Style empty_style_;
    bool show_percent_ = true;

    static std::string repeat(const std::string& unit, std::size_t n) {
        std::string out;
        out.reserve(unit.size() * n);
        for (std::size_t i = 0; i < n; ++i) out += unit;
        return out;
    }
};

}  // namespace ui
}  // namespace ceh

#endif  // CEH_UI_PROGRESS_BAR_HPP
