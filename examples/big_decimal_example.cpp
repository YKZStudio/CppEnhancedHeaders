// BigDecimal 使用示例
#include <ceh/types/big_decimal.hpp>
#include <iostream>

int main() {
    using ceh::BigDecimal;
    using ceh::RoundingMode;

    // 1) 精确的十进制运算（浮点 0.1+0.2 != 0.3，这里精确等于 0.3）
    std::cout << "0.1 + 0.2 = " << (BigDecimal("0.1") + BigDecimal("0.2")) << "\n";

    // 2) 货币计算：单价 × 数量，保留两位
    BigDecimal price("19.99");
    BigDecimal qty(7);
    std::cout << "7 件单价 19.99 合计 = " << (price * qty) << "\n";

    // 3) 除法需指定精度与舍入
    BigDecimal one(1), three(3);
    std::cout << "1 / 3 (20 位, 四舍五入) = " << one.divide(three, 20) << "\n";
    std::cout << "2 / 3 (4 位, 截断)      = "
              << BigDecimal(2).divide(three, 4, RoundingMode::Down) << "\n";

    // 4) 设定 scale 与去尾零
    std::cout << "1.236 -> 2 位四舍五入 = " << BigDecimal("1.236").set_scale(2) << "\n";
    std::cout << "1.500 去尾零          = "
              << BigDecimal("1.500").stripped_trailing_zeros() << "\n";

    return 0;
}
