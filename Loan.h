#ifndef LOAN_H
#define LOAN_H

#include <string>
#include <iostream>
#include <ctime> // For date handling
#include <sstream> // For stringstream usage
#include <iomanip> // For using put_time
#include <chrono>  // For using clock and system time

using namespace std;

class Loan 
{
private:
    string lenderName;     // Lender's name
    double money;          // Loan amount
    double interestRate;   // Interest rate (%)
    std::tm dueDate;       // Due date (std::tm format)
    std::tm createDate;    // Loan creation date (std::tm format)
    bool status;           // true: Paid, false: Unpaid
    
public:
    // Constructor
    Loan(const string& lender, double amount, double rate, const std::tm& due, const std::tm& create, bool state)
        : lenderName(lender), money(amount), interestRate(rate), dueDate(due), createDate(create), status(state) {}

    // Convert std::tm to string for display (in yyyy-mm-dd format)
    string getDueDateString() const 
    {
        stringstream ss;
        ss << put_time(&dueDate, "%Y-%m-%d"); // "yyyy-mm-dd" format
        return ss.str();
    }

    string getCreateDateString() const {
        stringstream ss;
        ss << put_time(&createDate, "%Y-%m-%d"); // "yyyy-mm-dd" format
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
    double getAmount() const { return money; }  // Return the loan amount

    // Getter for status
    bool getStatus() const { return status; }

    // Update loan information
    void update(const std::tm& newDate, double newRate, bool newStatus) 
    {
        dueDate = newDate;
        interestRate = newRate;
        status = newStatus;
    }

    // Calculate the total amount to pay based on compound interest (calculated by months)
    double calculateCompoundInterest() const 
    {
        // Calculate months elapsed from creation date to current date
        auto now = chrono::system_clock::now();
        time_t now_time = chrono::system_clock::to_time_t(now);
        std::tm* today = localtime(&now_time);

        // Calculate the number of months between today and loan creation date
        int monthsElapsed = (today->tm_year - createDate.tm_year) * 12 + (today->tm_mon - createDate.tm_mon);

        // Apply compound interest formula (monthly compounding)
        double totalAmount = money * pow(1 + (interestRate / 100 / 12), monthsElapsed);
        return totalAmount;
    }

    // Notification function to notify the loan status and total amount to pay
    void Notification() const {
        // Get the current date using chrono and system_clock for accurate current time
        auto now = chrono::system_clock::now();
        time_t now_time = chrono::system_clock::to_time_t(now);
        std::tm* today = localtime(&now_time);

        // Make a copy of the due date for calculations
        std::tm dueDateCopy = dueDate;

        // Calculate remaining days using difftime
        double diffInSeconds = difftime(mktime(&dueDateCopy), mktime(today));
        int daysRemaining = static_cast<int>(diffInSeconds / (60 * 60 * 24)); // Convert seconds to days

        double totalAmount = calculateCompoundInterest(); // Calculate total amount using compound interest

        // Check and display notification
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
