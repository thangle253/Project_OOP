#ifndef LOAN_H
#define LOAN_H

#include <string>
#include <iostream>
#include <ctime> // Dùng cho xử lý ngày tháng
#include <sstream> // Để chuyển đổi giữa string và std::tm
#include <iomanip> // Để sử dụng put_time
#include <chrono>  // Để sử dụng clock và hệ thống thời gian hiện tại
#include <cmath> // Để sử dụng sqrt
#include <math.h> // Để sử dụng pow


using namespace std;

class Loan 
{
private:
    string lenderName;     // Tên người cho vay
    double money;          // Số tiền vay
    double interestRate;   // Lãi suất (%)
    std::tm dueDate;       // Ngày đến hạn (dạng std::tm)
    std::tm createDate;    // Ngày tạo khoản vay (dạng std::tm)
    bool status;           // true: Đã trả, false: Chưa trả
    
public:
    // Constructor
    Loan(const string& lender, double amount, double rate, const std::tm& due, const std::tm& create, bool state)
        : lenderName(lender), money(amount), interestRate(rate), dueDate(due), createDate(create), status(state) {}

    // Convert std::tm to string for display (in yyyy-mm-dd format)
    string getDueDateString() const 
    {
        stringstream ss;
        ss << put_time(&dueDate, "%Y-%m-%d"); // Định dạng "yyyy-mm-dd"
        return ss.str();
    }

    string getCreateDateString() const {
        stringstream ss;
        ss << put_time(&createDate, "%Y-%m-%d"); // Định dạng "yyyy-mm-dd"
        return ss.str();
    }

    // Get loan details
    string getLoanDetail() const 
    {
        return "Lender: " + lenderName + ", Money: " + to_string(money) + ", Rate: " + to_string(interestRate) +
            "%, Due Date: " + getDueDateString() + ", Created On: " + getCreateDateString() + 
            ", Status: " + (status ? "Paid" : "Unpaid");
    }

    // Getter for amount
    double getAmount() const { return money; }  // Trả về số tiền đã cho vay

    // Getter for status
    bool getStatus() const { return status; }

    // Update loan information
    void update(const std::tm& newDate, double newRate, bool newStatus) 
    {
        dueDate = newDate;
        interestRate = newRate;
        status = newStatus;
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
    void Notification() const {
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
            cout << "Reminder: Loan from " << lenderName
                 << " is due today (" << getDueDateString() << ") and still unpaid!" << "T. otal amount to pay: " << totalAmount << " VND" << endl;
        } 
        else if (daysRemaining > 0 && !status) {
            cout << "Loan from " << lenderName
                 << " is still unpaid. Due in " << daysRemaining << " days. Due date: " 
                 << getDueDateString() << ". Total amount to pay: " << totalAmount << " VND" << endl;
        } 
        else if (status) {
            cout << "Loan from " << lenderName << " has been paid. Due date was: " << getDueDateString() << endl;
        }
        else if (daysRemaining < 0) {
            cout << "Loan from " << lenderName
                 << " is overdue. Was due " << -daysRemaining << " days ago. Due date: " 
                 << getDueDateString() << ". Total amount to pay: " << totalAmount << " VND" << endl;
        }
    }
};
#endif
