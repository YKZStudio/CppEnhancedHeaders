// ceh/ui/canvas.hpp
//
// Canvas / BrailleCanvas —— 字符画布与盲文点阵画布（header-only），位于 ceh::ui。
//
// Canvas：一张 w×h 的字符网格，可逐格设字符、画水平/竖直线、任意直线（Bresenham）、
//         矩形与填充矩形，render() 成多行字符串。
// BrailleCanvas：用盲文点阵把分辨率提升到 2w×4h（每个盲文字符含 2×4 个点），
//         适合画折线图/散点；plot(px, py) 点亮一个子像素。
//
// 用法：
//     ceh::ui::Canvas c(10, 5);
//     c.draw_rect(0, 0, 10, 5);
//     c.draw_line(1, 1, 8, 3, '*');
//     std::cout << c.render() << '\n';

#ifndef CEH_UI_CANVAS_HPP
#define CEH_UI_CANVAS_HPP

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <utility>
#include <vector>

namespace ceh {
namespace ui {

class Canvas {
public:
    Canvas(std::size_t width, std::size_t height, const std::string& fill = " ")
        : w_(width), h_(height), fill_(fill), grid_(width * height, fill) {}

    std::size_t width() const { return w_; }
    std::size_t height() const { return h_; }

    void clear() { std::fill(grid_.begin(), grid_.end(), fill_); }

    // 越界坐标静默忽略，便于直接传裁剪外的点
    void set(int x, int y, const std::string& cell) {
        if (x >= 0 && y >= 0 && static_cast<std::size_t>(x) < w_ &&
            static_cast<std::size_t>(y) < h_)
            grid_[static_cast<std::size_t>(y) * w_ + static_cast<std::size_t>(x)] = cell;
    }
    std::string get(int x, int y) const {
        if (x >= 0 && y >= 0 && static_cast<std::size_t>(x) < w_ &&
            static_cast<std::size_t>(y) < h_)
            return grid_[static_cast<std::size_t>(y) * w_ + static_cast<std::size_t>(x)];
        return std::string();
    }

    void draw_hline(int x0, int x1, int y, const std::string& cell = "─") {
        if (x1 < x0) std::swap(x0, x1);
        for (int x = x0; x <= x1; ++x) set(x, y, cell);
    }
    void draw_vline(int y0, int y1, int x, const std::string& cell = "│") {
        if (y1 < y0) std::swap(y0, y1);
        for (int y = y0; y <= y1; ++y) set(x, y, cell);
    }

    // Bresenham 直线
    void draw_line(int x0, int y0, int x1, int y1, const std::string& cell = "*") {
        int dx = std::abs(x1 - x0), dy = -std::abs(y1 - y0);
        int sx = x0 < x1 ? 1 : -1, sy = y0 < y1 ? 1 : -1;
        int err = dx + dy;
        while (true) {
            set(x0, y0, cell);
            if (x0 == x1 && y0 == y1) break;
            int e2 = 2 * err;
            if (e2 >= dy) { err += dy; x0 += sx; }
            if (e2 <= dx) { err += dx; y0 += sy; }
        }
    }

    // 矩形边框（左上角 + 宽高），用制表符画角与边
    void draw_rect(int x, int y, int w, int h, const std::string& cell = "") {
        if (w <= 0 || h <= 0) return;
        int x1 = x + w - 1, y1 = y + h - 1;
        if (cell.empty()) {
            draw_hline(x, x1, y, "─");
            draw_hline(x, x1, y1, "─");
            draw_vline(y, y1, x, "│");
            draw_vline(y, y1, x1, "│");
            set(x, y, "┌"); set(x1, y, "┐"); set(x, y1, "└"); set(x1, y1, "┘");
        } else {
            draw_hline(x, x1, y, cell);
            draw_hline(x, x1, y1, cell);
            draw_vline(y, y1, x, cell);
            draw_vline(y, y1, x1, cell);
        }
    }

    void fill_rect(int x, int y, int w, int h, const std::string& cell) {
        for (int yy = y; yy < y + h; ++yy)
            for (int xx = x; xx < x + w; ++xx) set(xx, yy, cell);
    }

    std::string render() const {
        std::string out;
        for (std::size_t y = 0; y < h_; ++y) {
            for (std::size_t x = 0; x < w_; ++x) out += grid_[y * w_ + x];
            if (y + 1 < h_) out += '\n';
        }
        return out;
    }

private:
    std::size_t w_, h_;
    std::string fill_;
    std::vector<std::string> grid_;
};

// 盲文点阵画布：分辨率 2*cols × 4*rows，每个字符承载 2×4 个子像素
class BrailleCanvas {
public:
    // 以「子像素」为单位的宽高（会向上取整到字符格）
    BrailleCanvas(std::size_t px_width, std::size_t px_height)
        : pw_(px_width), ph_(px_height),
          cols_((px_width + 1) / 2), rows_((px_height + 3) / 4),
          cells_(cols_ * rows_, 0) {}

    std::size_t pixel_width() const { return pw_; }
    std::size_t pixel_height() const { return ph_; }

    void clear() { std::fill(cells_.begin(), cells_.end(), std::uint8_t(0)); }

    // 点亮子像素 (px, py)，越界忽略
    void plot(int px, int py) {
        if (px < 0 || py < 0 || static_cast<std::size_t>(px) >= cols_ * 2 ||
            static_cast<std::size_t>(py) >= rows_ * 4)
            return;
        std::size_t cx = static_cast<std::size_t>(px) / 2;
        std::size_t cy = static_cast<std::size_t>(py) / 4;
        int dx = px % 2, dy = py % 4;
        cells_[cy * cols_ + cx] |= dot_bit(dx, dy);
    }

    void plot_line(int x0, int y0, int x1, int y1) {
        int dx = std::abs(x1 - x0), dy = -std::abs(y1 - y0);
        int sx = x0 < x1 ? 1 : -1, sy = y0 < y1 ? 1 : -1;
        int err = dx + dy;
        while (true) {
            plot(x0, y0);
            if (x0 == x1 && y0 == y1) break;
            int e2 = 2 * err;
            if (e2 >= dy) { err += dy; x0 += sx; }
            if (e2 <= dx) { err += dx; y0 += sy; }
        }
    }

    std::string render() const {
        std::string out;
        for (std::size_t cy = 0; cy < rows_; ++cy) {
            for (std::size_t cx = 0; cx < cols_; ++cx)
                out += braille_char(cells_[cy * cols_ + cx]);
            if (cy + 1 < rows_) out += '\n';
        }
        return out;
    }

private:
    std::size_t pw_, ph_, cols_, rows_;
    std::vector<std::uint8_t> cells_;

    // 盲文点位 -> bit。Unicode 盲文按列优先编号：
    // (0,0)=0x01 (0,1)=0x02 (0,2)=0x04 (1,0)=0x08 (1,1)=0x10 (1,2)=0x20
    // (0,3)=0x40 (1,3)=0x80
    static std::uint8_t dot_bit(int dx, int dy) {
        static const std::uint8_t map[2][4] = {
            {0x01, 0x02, 0x04, 0x40},
            {0x08, 0x10, 0x20, 0x80}
        };
        return map[dx][dy];
    }

    // 把 8 位点阵编码成一个 U+2800 起的盲文字符（UTF-8）
    static std::string braille_char(std::uint8_t bits) {
        char32_t cp = 0x2800u + bits;
        std::string s;
        s += static_cast<char>(0xE0 | (cp >> 12));
        s += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
        s += static_cast<char>(0x80 | (cp & 0x3F));
        return s;
    }
};

}  // namespace ui
}  // namespace ceh

#endif  // CEH_UI_CANVAS_HPP
