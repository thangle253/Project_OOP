#ifndef LEND_H
#define LEND_H

#include <string>
#include <iostream>
#include <ctime>    // Dùng cho xử lý ngày tháng
#include <sstream>  // Để sử dụng stringstream
#include <iomanip>  // Để sử dụng put_time
#include <chrono>   // Để sử dụng clock và hệ thống thời gian hiện tại
#include <cmath>    // Để sử dụng pow

using namespace std;

class Lend 
{
private:
    string debtorName;     // Tên người nợ
    double interestRate, money;   // Lãi suất cho vay (%)
    std::tm dueDate;        // Ngày đến hạn (dạng std::tm)
    bool status;           // true: Đã trả, false: Chưa trả
    std::tm createDate;     // Ngày tạo khoản cho vay (dạng std::tm)

public:
    // Constructor
    Lend(const string& debtor, double amount, double rate, const std::tm& due, const std::tm& create, bool state)
        : debtorName(debtor), money(amount), interestRate(rate), dueDate(due), createDate(create), status(state) {}

    // Convert std::tm to string for displaying
    string getDateString(const std::tm& date) const {
        stringstream ss;
        ss << put_time(&date, "%Y-%m-%d"); // Định dạng "yyyy-mm-dd"
        return ss.str();
    }

    // Get loan details
    string getBorrowDetail() const
    {
        return "Debtor: " + debtorName + ", Money: " + to_string(money) + ", Rate: " + to_string(interestRate) +
               "%, Due Date: " + getDateString(dueDate) + ", Status: " + (status ? "Paid" : "Unpaid");
    }

    // Getter for amount
    double getAmount() const
    {
        return money;  // Trả về số tiền đã cho vay
    }

    // Getter for status
    bool getStatus() const
    {
        return status;
    }

    // Update lend information
    void update(const std::tm& newDueDate, double newRate, bool newStatus)
    {
        dueDate = newDueDate;
        interestRate = newRate;
        status = newStatus;  // Cập nhật trạng thái khoản cho vay
    }

    // Tính toán số tiền cần trả theo lãi suất kép (tính theo số tháng)
    double calculateCompoundInterest() const 
    {
        // Tính số tháng từ ngày tạo đến ngày hiện tại
        auto now = chrono::system_clock::now();
        time_t now_time = chrono::system_clock::to_time_t(now);
        std::tm* today = localtime(&now_time);

        // Tính số tháng giữa ngày hiện tại và ngày tạo khoản vay
        int monthsElapsed = (today->tm_year - createDate.tm_year) * 12 + (today->tm_mon - createDate.tm_mon);

        // Áp dụng công thức lãi suất kép
        double totalAmount = money * pow(1 + (interestRate / 100 / 12), monthsElapsed); // Lãi suất kép hàng tháng
        return totalAmount;
    }

    // Hàm thông báo
    void Notification() const
    {
        // Lấy ngày hiện tại (sử dụng chrono và system_clock để có ngày giờ hiện tại chính xác)
        auto now = chrono::system_clock::now();
        time_t now_time = chrono::system_clock::to_time_t(now);
        std::tm* today = localtime(&now_time);

        // Tạo bản sao của ngày đến hạn (dueDate) để tính toán
        std::tm dueDateCopy = dueDate;

        // Tính số ngày còn lại bằng cách sử dụng difftime
        double diffInSeconds = difftime(mktime(&dueDateCopy), mktime(today));
        int daysRemaining = static_cast<int>(diffInSeconds / (60 * 60 * 24)); // Chuyển đổi giây thành ngày

        double totalAmount = calculateCompoundInterest(); // Sử dụng lãi suất kép

        // Kiểm tra và thông báo
        if (daysRemaining == 0 && !status) {
            cout << "Reminder: Lend to " << debtorName
                << " is due today (" << getDateString(dueDate) << ") and still unpaid! "
                << "Total amount to pay: " << totalAmount << " USD" << endl;
        } 
        else if (daysRemaining > 0 && !status) {
            cout << "Lend to " << debtorName
                << " is still unpaid. Due in " << daysRemaining << " days. Due date: " 
                << getDateString(dueDate) << ". Total amount to pay: " << totalAmount << " USD" << endl;
        } 
        else if (status) {
            cout << "Lend to " << debtorName << " has been paid. Due date was: " << getDateString(dueDate) << endl;
        }
        else if (daysRemaining < 0) {
            cout << "Lend to " << debtorName
                << " is overdue. Was due " << -daysRemaining << " days ago. Due date: " 
                << getDateString(dueDate) << ". Total amount to pay: " << totalAmount << " USD" << endl;
        }
    }
};

#endif
