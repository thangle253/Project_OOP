#ifndef LOAN_H
#define LOAN_H

#include <string>
#include <iostream>
#include <ctime> // Xử lý ngày tháng
#include <sstream> // Dùng cho stringstream
#include <iomanip> // Sử dụng put_time
#include <chrono>  // Dùng cho clock và hệ thống thời gian  
#include <cmath> // Dùng cho pow
using namespace std;

class Loan 
{
private:
    string lenderName;     // Tên người cho vay
    double money;          // Số tiền vay
    double interestRate;   // Lãi suất (%)
    std::tm dueDate;       // Ngày đến hạn (kiểu std::tm)
    std::tm createDate;    // Ngày tạo khoản vay (kiểu std::tm)
    bool status;            // Trạng thái khoản vay, true: Đã trả, false: Chưa trả
    
public:
    // Constructor
    Loan(const string& lender, double amount, double rate, const std::tm& due, const std::tm& create, bool state)
        : lenderName(lender), money(amount), interestRate(rate), dueDate(due), createDate(create), status(state) {}

    // Chuyển đổi std::tm thành chuỗi để hiển thị (dạng yyyy-mm-dd)
    string getDueDateString() const 
    {
        stringstream ss;
        ss << put_time(&dueDate, "%Y-%m-%d"); /// Dạng "yyyy-mm-dd"
        return ss.str();
    }

    string getCreateDateString() const {
        stringstream ss;
        ss << put_time(&createDate, "%Y-%m-%d"); // Dạng "yyyy-mm-dd"
        return ss.str();
    }

    // Lấy chi tiết khoản vay
    string getLoanDetail() const 
    {
        return "Lender: " + lenderName + ", Money: " + to_string(money) + "VND, Rate: " + to_string(interestRate) +
            "%, Due Date: " + getDueDateString() + ", Created On: " + getCreateDateString() + 
            ", Status: " + (status ? "Paid" : "Unpaid");
    }

    //get name
    string getLenderName() const { return lenderName; } // Trả về tên người cho vay
    
    //Get số tiền vay
    double getAmount() const { return money; }  // Trả về số tiền vay

    // Get trạng thái khoản vay
    bool getStatus() const { return status; }

    // Get ngày đến hạn
    std::tm getDueDate() const { return dueDate; }
    // Get lãi suất
    double getInterestRate() const { return interestRate; }


    // Cập nhật thông tin khoản vay
    void update(const std::tm& newDate, double newRate, bool newStatus) 
    {
        dueDate = newDate;
        interestRate = newRate;
        status = newStatus;
    }

    // Tính toán tổng số tiền cần trả dựa trên lãi suất kép (tính theo tháng)
    double calculateCompoundInterest() const 
    {
        // Tính số tháng từ ngày tạo khoản vay đến ngày hiện tại
        auto now = chrono::system_clock::now();
        time_t now_time = chrono::system_clock::to_time_t(now);
        std::tm* today = localtime(&now_time);

        // Tính số tháng giữa ngày hôm nay và ngày tạo khoản vay
        int monthsElapsed = (today->tm_year - createDate.tm_year) * 12 + (today->tm_mon - createDate.tm_mon);

        // Áp dụng công thức lãi suất kép (lãi kép hàng tháng)
        double totalAmount = money * pow(1 + (interestRate / 100 / 12), monthsElapsed);
        return totalAmount;
    }

    // Hàm thông báo về trạng thái khoản vay và tổng số tiền cần trả
    void Notification() const {
        // Lấy ngày hiện tại bằng chrono và system_clock để có thời gian chính xác
        auto now = chrono::system_clock::now();
        time_t now_time = chrono::system_clock::to_time_t(now);
        std::tm* today = localtime(&now_time);

        // Lưu lại ngày đến hạn để tính toán
        std::tm dueDateCopy = dueDate;

        // Tính số ngày còn lại bằng hàm difftime
        double diffInSeconds = difftime(mktime(&dueDateCopy), mktime(today));
        int daysRemaining = static_cast<int>(diffInSeconds / (60 * 60 * 24)); // Chuyển giây thành ngày

        double totalAmount = calculateCompoundInterest(); // Tính tổng số tiền cần trả bằng lãi kép

        // Kiểm tra và hiển thị thông báo
        if (daysRemaining == 0 && !status) {
            cout << "Reminder: Loan from " << lenderName
                 << " is due today (" << getDueDateString() << ") and still unpaid! Total amount to pay( up to the present time): " << totalAmount << " VND" << endl;
        } 
        else if (daysRemaining > 0 && !status) {
            cout << "Loan from " << lenderName
                 << " is still unpaid. Due in " << daysRemaining << " days. Due date: " 
                 << getDueDateString() << ". Total amount to pay (up to the present time): " << totalAmount << " VND" << endl;
        } 
        else if (status) {
            cout << "Loan from " << lenderName << " has been paid. Due date was: " << getDueDateString() << endl;
        }
        else if (daysRemaining < 0) {
            cout << "Loan from " << lenderName
                 << " is overdue. Was due " << -daysRemaining << " days ago. Due date: " 
                 << getDueDateString() << ". Total amount to pay (up to the present time): " << totalAmount << " VND" << endl;
        }
    }
};

#endif
