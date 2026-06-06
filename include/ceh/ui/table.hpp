// ceh/ui/table.hpp
//
// Table —— 终端表格（header-only），位于 ceh::ui。
//
// 自动按内容计算每列宽度（用 display_width，故中英文混排也能对齐），支持表头、
// 每列对齐方式、多种边框风格与表头样式。render() 输出多行字符串。
//
// 用法：
//     using namespace ceh::ui;
//     Table t;
//     t.set_headers({"名字", "年龄"});
//     t.set_align(1, Align::Right);
//     t.add_row({"Alice", "30"});
//     t.add_row({"鲍勃", "7"});
//     std::cout << t.render() << '\n';

#ifndef CEH_UI_TABLE_HPP
#define CEH_UI_TABLE_HPP

#include <ceh/ui/align.hpp>
#include <ceh/ui/border.hpp>
#include <ceh/ui/style.hpp>
#include <ceh/ui/text.hpp>

#include <algorithm>
#include <cstddef>
#include <string>
#include <vector>

namespace ceh {
namespace ui {

class Table {
public:
    Table& set_headers(const std::vector<std::string>& headers) {
        headers_ = headers;
        ensure_columns(headers_.size());
        return *this;
    }

    Table& add_row(const std::vector<std::string>& row) {
        rows_.push_back(row);
        ensure_columns(row.size());
        return *this;
    }

    Table& set_border(BorderStyle style) { border_ = style; return *this; }

    Table& set_align(std::size_t column, Align align) {
        ensure_columns(column + 1);
        aligns_[column] = align;
        return *this;
    }

    Table& set_header_style(const Style& style) { header_style_ = style; return *this; }
    Table& set_padding(std::size_t cells) { padding_ = cells; return *this; }

    std::size_t columns() const { return num_columns_; }
    std::size_t rows() const { return rows_.size(); }

    std::string render(bool with_style = true) const {
        std::vector<std::size_t> widths = column_widths();
        const BorderChars b = border_chars(border_);
        const bool has_header = !headers_.empty();

        std::string out;
        out += border_line(b.top_left, b.tee_down, b.top_right, b.horizontal, widths);
        out += '\n';

        if (has_header) {
            out += data_line(headers_, widths, b.vertical, with_style, true);
            out += '\n';
            out += border_line(b.tee_right, b.cross, b.tee_left, b.horizontal, widths);
            out += '\n';
        }

        for (const auto& row : rows_) {
            out += data_line(row, widths, b.vertical, with_style, false);
            out += '\n';
        }

        out += border_line(b.bottom_left, b.tee_up, b.bottom_right, b.horizontal, widths);
        return out;
    }

private:
    std::vector<std::string> headers_;
    std::vector<std::vector<std::string>> rows_;
    std::vector<Align> aligns_;
    BorderStyle border_ = BorderStyle::Square;
    Style header_style_;
    std::size_t num_columns_ = 0;
    std::size_t padding_ = 1;   // 单元格内容两侧的空格数

    void ensure_columns(std::size_t n) {
        if (n > num_columns_) {
            num_columns_ = n;
            aligns_.resize(n, Align::Left);
        }
    }

    std::string cell_at(const std::vector<std::string>& row, std::size_t col) const {
        return col < row.size() ? row[col] : std::string();
    }

    // 每列宽度 = 该列所有单元格（含表头）的最大显示宽
    std::vector<std::size_t> column_widths() const {
        std::vector<std::size_t> widths(num_columns_, 0);
        for (std::size_t c = 0; c < num_columns_; ++c) {
            if (c < headers_.size()) widths[c] = display_width(headers_[c]);
            for (const auto& row : rows_)
                widths[c] = std::max(widths[c], display_width(cell_at(row, c)));
        }
        return widths;
    }

    // 拼一条横向边框线（left + 每列横线 + 连接 + right）
    std::string border_line(const std::string& left, const std::string& mid,
                            const std::string& right, const std::string& h,
                            const std::vector<std::size_t>& widths) const {
        std::string out = left;
        for (std::size_t c = 0; c < widths.size(); ++c) {
            for (std::size_t i = 0; i < widths[c] + 2 * padding_; ++i) out += h;
            out += (c + 1 == widths.size()) ? right : mid;
        }
        return out;
    }

    // 拼一条数据行
    std::string data_line(const std::vector<std::string>& row,
                          const std::vector<std::size_t>& widths,
                          const std::string& v, bool with_style, bool is_header) const {
        std::string pad(padding_, ' ');
        std::string out = v;
        for (std::size_t c = 0; c < widths.size(); ++c) {
            std::string content = pad_to(cell_at(row, c), widths[c], aligns_[c]);
            std::string cell = pad + content + pad;
            if (is_header && with_style && !header_style_.empty())
                cell = header_style_.apply(cell);
            out += cell;
            out += v;
        }
        return out;
    }
};

}  // namespace ui
}  // namespace ceh

#endif  // CEH_UI_TABLE_HPP
