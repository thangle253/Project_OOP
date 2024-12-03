#ifndef LEND_H
#define LEND_H

#include <string>
#include <iostream>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <cmath>

using namespace std;

class Lend 
{
private:
    string debtorName;     // Tên người nợ
    double interestRate, money;   // Lãi suất cho vay (%)
    bool status;           // true: Đã trả, false: Chưa trả
    tm dueDate;            // Ngày đến hạn
    tm createDate;         // Ngày tạo khoản cho vay

public:
    Lend(const string& debtor, double amount, double rate, const tm& due, const tm& create, bool state)
        : debtorName(debtor), money(amount), interestRate(rate), dueDate(due), createDate(create), status(state) {}

    tm getCurrentDate() const {
        auto now = chrono::system_clock::now();
        time_t now_time = chrono::system_clock::to_time_t(now);
        tm result{};
        localtime_s(&result, &now_time);
        return result;
    }

    string getDateString(const tm& date) const {
        stringstream ss;
        ss << put_time(&date, "%Y-%m-%d");
        return ss.str();
    }

    string getBorrowDetail() const {
        return "Debtor: " + debtorName + ", Money: " + to_string(money) + ", Rate: " + to_string(interestRate) +
               "%, Due Date: " + getDateString(dueDate) + ", Status: " + (status ? "Paid" : "Unpaid");
    }

    double getAmount() const { return money; }

    bool getStatus() const { return status; }

    void update(const tm& newDueDate, double newRate, bool newStatus) {
        dueDate = newDueDate;
        interestRate = newRate;
        status = newStatus;
    }

    double calculateCompoundInterest() const {
        tm today = getCurrentDate();

        int monthsElapsed = (today.tm_year - createDate.tm_year) * 12 + (today.tm_mon - createDate.tm_mon);

        double totalAmount = money * pow(1 + (interestRate / 100 / 12), monthsElapsed);
        return totalAmount;
    }

    void Notification() const {
        tm today = getCurrentDate();

        tm dueDateCopy = dueDate;
        double diffInSeconds = difftime(mktime(&dueDateCopy), mktime(&today));
        int daysRemaining = static_cast<int>(diffInSeconds / (60 * 60 * 24));

        double totalAmount = calculateCompoundInterest();

        if (daysRemaining == 0 && !status) {
            cout << "Reminder: Lend to " << debtorName
                << " is due today (" << getDateString(dueDate) << ") and still unpaid! "
                << "Total amount to pay: " << totalAmount << " VND" << endl;
        } else if (daysRemaining > 0 && !status) {
            cout << "Lend to " << debtorName
                << " is still unpaid. Due in " << daysRemaining << " days. Due date: " 
                << getDateString(dueDate) << ". Total amount to pay: " << totalAmount << " VND" << endl;
        } else if (status) {
            cout << "Lend to " << debtorName << " has been paid. Due date was: " << getDateString(dueDate) << endl;
        } else if (daysRemaining < 0) {
            cout << "Lend to " << debtorName
                << " is overdue. Was due " << -daysRemaining << " days ago. Due date: " 
                << getDateString(dueDate) << ". Total amount to pay: " << totalAmount << " VND" << endl;
        }
    }
};

#endif
