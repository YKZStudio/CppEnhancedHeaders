// ceh/ui/console.hpp
//
// Console —— 终端输出封装与能力探测（header-only），位于 ceh::ui。
//
// 负责回答「现在该不该输出颜色」并据此打印：检测输出是否连接到终端（TTY）、
// 探测终端宽度、遵守 NO_COLOR 约定（https://no-color.org）。Console 把样式逻辑
// 与 Color/Style/Text 串起来——启用颜色时输出 ANSI，否则退化为纯文本。
//
// 跨平台：TTY 检测与宽度探测在 Windows 用 <io.h>/<windows.h>，POSIX 用
// <unistd.h>/<sys/ioctl.h>；其余为标准 C++。
//
// 用法：
//     ceh::ui::Console con;                 // 绑定 stdout，自动决定是否上色
//     con.println("普通");
//     con.println("错误", ceh::ui::Style().fg(ceh::ui::Color::red()).bold());

#ifndef CEH_UI_CONSOLE_HPP
#define CEH_UI_CONSOLE_HPP

#include <ceh/ui/style.hpp>
#include <ceh/ui/text.hpp>

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <string>

#if defined(_WIN32)
#  include <io.h>
#  include <windows.h>
#else
#  include <unistd.h>
#  include <sys/ioctl.h>
#endif

namespace ceh {
namespace ui {

namespace detail {
inline bool fd_is_tty(std::FILE* stream) {
#if defined(_WIN32)
    return _isatty(_fileno(stream)) != 0;
#else
    return isatty(fileno(stream)) != 0;
#endif
}
}  // namespace detail

// 标准输出 / 标准错误是否连接到终端
inline bool stdout_is_terminal() { return detail::fd_is_tty(stdout); }
inline bool stderr_is_terminal() { return detail::fd_is_tty(stderr); }

// 纯逻辑：根据「是否 TTY」与「是否设置了 NO_COLOR」决定是否上色（便于单测）
inline bool resolve_color_enabled(bool is_tty, bool no_color_set) {
    if (no_color_set) return false;   // NO_COLOR 优先级最高
    return is_tty;
}

// 针对 stdout 的颜色启用判定（遵守 NO_COLOR 约定）
inline bool color_enabled_for_stdout() {
    return resolve_color_enabled(stdout_is_terminal(), std::getenv("NO_COLOR") != nullptr);
}

// 探测终端列宽；探测失败时回退到 COLUMNS 环境变量，再回退到 fallback
inline std::size_t terminal_width(std::size_t fallback = 80) {
#if defined(_WIN32)
    CONSOLE_SCREEN_BUFFER_INFO info;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info)) {
        int w = info.srWindow.Right - info.srWindow.Left + 1;
        if (w > 0) return static_cast<std::size_t>(w);
    }
#else
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_col > 0)
        return static_cast<std::size_t>(ws.ws_col);
#endif
    if (const char* col = std::getenv("COLUMNS")) {
        int w = std::atoi(col);
        if (w > 0) return static_cast<std::size_t>(w);
    }
    return fallback;
}

class Console {
public:
    // 默认绑定 std::cout，并自动判定是否上色
    explicit Console(std::ostream& os = std::cout,
                     bool enable_color = color_enabled_for_stdout())
        : os_(&os), color_(enable_color) {}

    bool color_enabled() const { return color_; }
    void set_color_enabled(bool on) { color_ = on; }

    std::size_t width(std::size_t fallback = 80) const { return terminal_width(fallback); }

    // 纯文本
    Console& print(const std::string& text) {
        (*os_) << text;
        return *this;
    }
    // 带样式文本（未启用颜色则退化为纯文本）
    Console& print(const std::string& text, const Style& style) {
        (*os_) << (color_ ? style.apply(text) : text);
        return *this;
    }
    // 富文本
    Console& print(const Text& text) {
        (*os_) << text.render(color_);
        return *this;
    }

    Console& println(const std::string& text = "") {
        (*os_) << text << '\n';
        return *this;
    }
    Console& println(const std::string& text, const Style& style) {
        (*os_) << (color_ ? style.apply(text) : text) << '\n';
        return *this;
    }
    Console& println(const Text& text) {
        (*os_) << text.render(color_) << '\n';
        return *this;
    }

private:
    std::ostream* os_;
    bool color_;
};

}  // namespace ui
}  // namespace ceh

#endif  // CEH_UI_CONSOLE_HPP
