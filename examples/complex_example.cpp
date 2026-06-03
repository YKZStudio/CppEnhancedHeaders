// Complex 使用示例
#include <ceh/types/complex.hpp>
#include <iostream>

int main() {
    using C = ceh::Complex<double>;

    C z(3, 4);
    std::cout << "z       = " << z << "\n";
    std::cout << "|z|     = " << z.abs() << "\n";
    std::cout << "conj(z) = " << z.conj() << "\n";
    std::cout << "z * i   = " << z * C(0, 1) << "\n";

    // 单位根：1 的 4 个四次方根
    std::cout << "\n1 的四次方根:\n";
    for (int k = 0; k < 4; ++k) {
        C root = C::polar(1.0, 2 * M_PI * k / 4);
        std::cout << "  w" << k << " = " << root
                  << "   (w^4 = " << root.pow(4) << ")\n";
    }
    return 0;
}
