#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <ctime>    // Để sử dụng std::tm
#include <sstream>  // Để sử dụng stringstream
#include <iomanip>  // Để sử dụng put_time

using namespace std;

class Transaction 
{
    private:
        string transID;          // Mã giao dịch
        double money;            // Số tiền giao dịch
        std::tm date;            // Ngày thực hiện giao dịch (sử dụng std::tm)
        string transactionType;  // Loại giao dịch ("Income" hoặc "Expense")
        string category;         // Danh mục (VD: "Food", "Salary", ...)
        string note;             // Ghi chú

    public:
        // Constructor
        Transaction(const string& id, double amount, const std::tm& transDate,
                    const string& type, const string& cat, const string& notes)
            : transID(id), money(amount), date(transDate),
              transactionType(type), category(cat), note(notes) {}

        // Getter
        double getAmount() const { return money; }
        string getType() const { return transactionType; }
        string getID() const { return transID; }

        // Hàm để chuyển std::tm sang định dạng ngày tháng "yyyy-mm-dd"
        string getDateString() const 
        {
            stringstream ss;
            ss << put_time(&date, "%Y-%m-%d"); // Định dạng "yyyy-mm-dd"
            return ss.str();
        }

        string getTransactionDetail() const 
        {
            return "ID: " + transID + ", Amount: " + to_string(money) +
                   ", Date: " + getDateString() +
                   ", Type: " + transactionType +
                   ", Category: " + category + ", Note: " + note;
        }

        string getDate() const 
        {
            return getDateString(); // Trả về ngày dưới dạng chuỗi
        }

        string getCategory() const { return category; }
        string getNote() const { return note; }

        // Setters
        void setAmount(double newAmount) { money = newAmount; }
        void setNote(string newNote) { note = newNote; }

        // Cập nhật giao dịch
        void update(double newAmount, const std::tm& newDate, const string& newType, 
                    const string& newCategory, const string& newNote) 
        {
            money = newAmount;
            date = newDate;
            transactionType = newType;
            category = newCategory;
            note = newNote;
        }
};

#endif
