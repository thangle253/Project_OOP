#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>
#include <iostream>
#include <vector>
#include "Transaction.h" // Mỗi tài khoản có thể gắn với nhiều giao dịch
using namespace std;

class Account 
{
    private:
        string accountID;               // Mã định danh tài khoản
        string accountName;             // Tên tài khoản
        double balance;                 // Số dư tài khoản
        vector<Transaction> history;   // Lịch sử giao dịch

    public:
        // Constructor
        Account(const string& id, const string& name, double initialBalance)
            : accountID(id), accountName(name), balance(initialBalance) {}

        // Getter
        string getAccountID() const { return accountID; }
        string getAccountName() const { return accountName; }
        double getBalance() const { return balance; }

        // Thêm giao dịch vào lịch sử
        void addTransaction(const Transaction& trans) 
        {
            history.push_back(trans);
            if (trans.getType() == "Income") 
            {
                balance += trans.getAmount();
            } 
            else if (trans.getType() == "Expense") 
            {
                balance -= trans.getAmount();
            }
        }

        // Hiển thị lịch sử giao dịch
        void displayHistory() const 
        {
            cout << "Transaction history for account " << accountName << ":\n";
            for (const auto& trans : history) 
            {
                cout << trans.getTransactionDetail() << endl;
            }
        }
        
        // Cập nhật số dư
        void updateBalance(double amount) { balance += amount; }
};

#endif
