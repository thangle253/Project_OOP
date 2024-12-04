#ifndef LEND_H
#define LEND_H

#include <string>        // Thư viện xử lý chuỗi
#include <iostream>      // Thư viện xuất nhập
#include <ctime>         // Thư viện xử lý thời gian
#include <sstream>       // Thư viện xử lý luồng chuỗi
#include <iomanip>       // Thư viện định dạng đầu ra
#include <chrono>        // Thư viện thời gian chính xác
#include <cmath>         // Thư viện xử lý toán học (pow)

using namespace std;

// Class Lend quản lý khoản cho vay
class Lend 
{
private:
    string debtorName;       // Tên người vay nợ
    double interestRate;     // Lãi suất (%)
    double money;            // Số tiền cho vay
    tm dueDate;              // Ngày đến hạn
    bool status;             // Trạng thái: true - đã trả, false - chưa trả
    tm createDate;           // Ngày tạo khoản cho vay

public:
    // Hàm khởi tạo
    Lend(const string& debtor, double amount, double rate, const tm& due, const tm& create, bool state)
        : debtorName(debtor), money(amount), interestRate(rate), dueDate(due), createDate(create), status(state) {} 

    // Chuyển đổi ngày tháng (tm) thành chuỗi định dạng "YYYY-MM-DD"
    string getDateString(const tm& date) const {
        stringstream ss;
        ss << put_time(&date, "%Y-%m-%d");
        return ss.str();
    }

    // Lấy chi tiết khoản vay
    string getBorrowDetail() const {
        return "Debtor: " + debtorName + ", Money: " + to_string(money) + ", Rate: " + to_string(interestRate) +
               "%, Due Date: " + getDateString(dueDate) + ", Status: " + (status ? "Paid" : "Unpaid");
    }

    // Lấy số tiền đã cho vay
    double getAmount() const {
        return money;
    }

    // Lấy trạng thái trả nợ
    bool getStatus() const {
        return status;
    }
    // Kiểm tra ngày hợp lệ
    bool isValidDate(int year, int month, int day) {
        if (year < 0 || month < 1 || month > 12 || day < 1) return false;

        // Số ngày trong từng tháng
        const int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

        // Kiểm tra năm nhuận
        bool isLeapYear = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));

        int maxDay = daysInMonth[month - 1];
        if (month == 2 && isLeapYear) maxDay = 29; // Tháng 2 năm nhuận

        return day <= maxDay;
    }

    // Cập nhật thông tin khoản vay
    void update(const tm& newDueDate, double newRate, bool newStatus) {
        // Kiểm tra tính hợp lệ của ngày
        if (!isValidDate(newDueDate.tm_year + 1900, newDueDate.tm_mon + 1, newDueDate.tm_mday)) {
        throw std::invalid_argument("Invalid due date: The specified date does not exist.");
        }
        dueDate = newDueDate;        // Ngày đến hạn mới
        interestRate = newRate;      // Lãi suất mới
        status = newStatus;          // Trạng thái mới
    }

    // Tính lãi suất kép (theo số tháng)
    double calculateCompoundInterest() const {
        // Tính số tháng từ ngày tạo đến ngày hiện tại
        auto now = chrono::system_clock::now();   // Lấy thời gian hiện tại         
        time_t now_time = chrono::system_clock::to_time_t(now);   // Chuyển đổi thời gian hiện tại thành kiểu time_t
        tm* today = localtime(&now_time);   // Chuyển đổi thời gian hiện tại thành tm

        // Số tháng trôi qua kể từ ngày tạo khoản vay
        int monthsElapsed = (today->tm_year - createDate.tm_year) * 12 + (today->tm_mon - createDate.tm_mon);

        // Công thức tính lãi suất kép
        double totalAmount = money * pow(1 + (interestRate / 100 / 12), monthsElapsed);
        return totalAmount;
    }

    // Thông báo nhắc nhở về khoản vay
    void Notification() const {
        // Lấy ngày hiện tại
        auto now = chrono::system_clock::now();
        time_t now_time = chrono::system_clock::to_time_t(now);
        tm* today = localtime(&now_time);

        // Sao chép ngày đến hạn để tính toán
        tm dueDateCopy = dueDate;

        // Tính số ngày còn lại đến hạn
        double diffInSeconds = difftime(mktime(&dueDateCopy), mktime(today));
        int daysRemaining = static_cast<int>(diffInSeconds / (60 * 60 * 24));

        // Tính tổng số tiền cần trả (bao gồm lãi suất kép)
        double totalAmount = calculateCompoundInterest();

        // Kiểm tra điều kiện và đưa ra thông báo
        // Nếu hôm nay là ngày đến hạn và khoản vay chưa trả
        if (daysRemaining == 0 && !status) {
            cout << "Reminder: Lend to " << debtorName
                << " is due today (" << getDateString(dueDate) << ") and still unpaid! "
                << "Total amount to pay: " << totalAmount << " VND" << endl;
        } 
        // Nếu còn chưa đến hạn và khoản vay chưa trả
        else if (daysRemaining > 0 && !status) {
            cout << "Lend to " << debtorName
                << " is still unpaid. Due in " << daysRemaining << " days. Due date: " 
                << getDateString(dueDate) << ". Total amount to pay: " << totalAmount << " VND" << endl;
        } 
        // Nếu khoản vay đã trả
        else if (status) {
            cout << "Lend to " << debtorName << " has been paid. Due date was: " << getDateString(dueDate) << endl;
        }
        // Nếu quá hạn
        else if (daysRemaining < 0) {
            cout << "Lend to " << debtorName
                << " is overdue. Was due " << -daysRemaining << " days ago. Due date: " 
                << getDateString(dueDate) << ". Total amount to pay: " << totalAmount << " VND" << endl;
        }
    }
};
#endif