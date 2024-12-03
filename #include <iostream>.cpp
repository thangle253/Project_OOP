#include <iostream>
#include <matplotlibcpp.h> // Thêm thư viện matplotlibcpp

namespace plt = matplotlibcpp;

int main() {
    // Tạo dữ liệu cho biểu đồ
    std::vector<int> x = {1, 2, 3, 4, 5};
    std::vector<int> y = {1, 4, 9, 16, 25};

    // Vẽ biểu đồ
    plt::plot(x, y);

    // Thêm tiêu đề và nhãn
    plt::title("Biểu đồ đơn giản");
    plt::xlabel("Trục X");
    plt::ylabel("Trục Y");

    // Hiển thị biểu đồ
    plt::show();

    return 0;
}