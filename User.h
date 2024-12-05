#ifndef USER_H
#define USER_H

#include <algorithm>
#include <string>
#include <vector>
#include "Account.h"
#include "Transaction.h"
#include "Loan.h"
#include "Lend.h"

using namespace std;

class User 
{
    private:
        string ID;                      // Mã người dùng
        string name;                    // Tên người dùng
        string password;                // Mật khẩu
        vector<Account> accounts;       // Danh sách tài khoản
        vector<Loan> loans;             // Danh sách khoản vay
        vector<Lend> lends;             // Danh sách khoản cho vay
        vector<Transaction> transactions; // Danh sách giao dịch
        double money;                   // Tổng số dư

    public:

        // Constructor
        User() : ID(""), name(""), password(""), money(0.0) {}

        // Constructor with parameters
        User(const string& userID, const string& userName, const string& userPassword)
            : ID(userID), name(userName), password(userPassword), money(0.0) {}

        bool authenticate(const string& enteredPassword) const
        {
            return password == enteredPassword;
        }
        // Quản lý tài khoản
        void addAccount(const Account& acc)
        {
            accounts.push_back(acc);
            money += acc.getBalance();
        }
        
        void removeAccount(const string& accID)
        {
            auto it = find_if(accounts.begin(), accounts.end(),
                [accID](const Account& acc) { return acc.getAccountID() == accID; });

            if (it != accounts.end()) {
                // Trước khi xóa, lấy số dư tài khoản
                double balance = it->getBalance();

                // Trừ số dư tài khoản khỏi tổng số dư người dùng
                money -= balance;
                cout << "The account balance of " << accID << ": " << balance << " VND has been deducted from the user's balance";
                // Xóa tài khoản khỏi danh sách
                accounts.erase(it);
                cout << "The account " << accID << " has been deleted.\n";
            } 
            else 
            {
                cout << "Account not found: " << accID << "\n";
            }
        }

        void updateAccount(const string& accID, double newBalance)
        {
            for (auto& acc : accounts) 
            {
                if (acc.getAccountID() == accID) 
                {
                    money -= acc.getBalance();
                    acc.updateBalance(newBalance - acc.getBalance());
                    money += acc.getBalance();
                    cout << "The account: " << accID << " has been successfully updated.\n";
                    return;
                }
            }
            cout << "Account not found: " << accID << "\n";
        }

        void banking(const string& fromAccID, const string& toAccID, double amount)              
        {
            Account *fromAcc = nullptr, *toAcc = nullptr;

            for (auto& acc : accounts) {
                if (acc.getAccountID() == fromAccID) fromAcc = &acc;
                if (acc.getAccountID() == toAccID) toAcc = &acc;
            }

            if (!fromAcc || !toAcc) {
                cout << "Error: One or both account IDs are invalid.\n";
                return;
            }

            if (fromAcc->getBalance() < amount) 
            {
                cout << "Error: The account " << fromAccID << " does not have sufficient balance.\n"; //Nếu số dư tài khoản không đủ
                return;
            }
            fromAcc->updateBalance(-amount);
            toAcc->updateBalance(amount);
            cout << "Transfer successful: " << amount << " VND from " << fromAccID << " to " << toAccID << endl;
        }

        // Quản lý giao dịch
        void addTrans(const Transaction& trans)
        {
            transactions.push_back(trans);
        }

        void removeTrans(const string& transID)
        {
            auto it = find_if(transactions.begin(), transactions.end(),
                            [transID](const Transaction& trans) { return trans.getID() == transID; });
            if (it != transactions.end()) {
                transactions.erase(it);
                cout << "The transaction with ID " << transID << " has been deleted.\n";
            } else {
                cout << "Transaction with the given ID not found: " << transID << ".\n";
            }
        }

        void updateTrans(const string& transID, double newAmount, const string& newNote)
        {
            for (auto& trans : transactions) 
            {
                if (trans.getID() == transID) 
                {
                    double oldAmount = trans.getAmount(); // Lưu lại số tiền cũ
                    trans.setAmount(newAmount); // Cập nhật số tiền mới
                    trans.setNote(newNote); // Cập nhật ghi chú mới

                    // Cập nhật số dư tài khoản nếu số tiền giao dịch thay đổi
                    if (newAmount != oldAmount)
                    {
                        if(trans.getType() == "1") // Nếu là giao dịch thu
                        {
                            double diff = oldAmount - newAmount;  // Tính chênh lệch số tiền 
                            this->updateBalance(-diff);  // Cập nhật số dư tài khoản
                            cout << "The account balance has been updated after the transaction modification.\n";
                        }
                        else //chi
                        {
                            double diff = oldAmount - newAmount ;  // Tính chênh lệch số tiền 
                            this->updateBalance(diff);  // Cập nhật số dư tài khoản
                            cout << "The account balance has been updated after the transaction modification.\n";
                        }
                    }
                    cout << "The transaction " << transID << "  has been updated.\n";
                    return;
                }
            }
            cout << "Transaction with the given ID not found: " << transID << ".\n";
        }

        void transHistory() const
        {
            cout << "Transaction History:\n";
            for (const auto& trans : transactions) 
            {
                cout << trans.getTransactionDetail() << endl;
            }
        }

        // Quản lý khoản vay 
        void addLoan(const Loan& loan)
        {
            loans.push_back(loan);
        }

        void removeLoan(const string& lenderName)
        {
            // Duyet danh sach khoan vay de xoa
            for (auto it = loans.begin(); it != loans.end(); ++it) 
            {
                if (it->getLoanDetail().find(lenderName) != string::npos) 
                {
                    // Kiểm tra xem khoản vay có được trả chưa
                    if (it->getStatus() == false)   
                    { // Nếu khoản vay chưa được trả
                        double amount = it->getAmount(); // Lấy số tiền khoản vay
                        this -> money += amount; // Hoàn trả số tiền cho vay vào tài khoản của người cho vay
                        cout << "The loan of " << lenderName << " has not been repaid, and the account balance has been updated.\n";
                    }  
                    // Xoa khoan vay
                    loans.erase(it);
                    cout << "The loan of " << lenderName << " has been deleted.\n";
                    return;
                }
            }
            cout << "Loan from the lender not found: " << lenderName << endl;
        }
    
        void updateLoan(const string& lenderName, double newRate, const std::tm& newDueDate, bool newStatus)
        {
            // Tìm khoản vay theo tên người cho vay (lenderName)
            for (auto& loan : loans) 
            {
                if (loan.getLoanDetail().find(lenderName) != string::npos) 
                {
                    // Cập nhật lãi suất, ngày đến hạn và trạng thái
                    // Không cần chuyển đổi ngày, vì newDueDate đã là kiểu std::tm
                    if (newStatus != loan.getStatus()) 
                    {
                        // Nếu trạng thái là "đã trả", cập nhật số dư của người cho vay
                        if (newStatus) 
                        {
                            double amount = loan.calculateCompoundInterest();  // Lấy số tiền đã cho vay
                            this->updateBalance(-amount); // Cập nhật số dư của người cho vay
                            cout << "The lender's balance has been updated after the loan repayment.\n";
                        }
                        else
                        {
                            double amount = loan.calculateCompoundInterest();  // Lấy số tiền đã cho vay
                            this->updateBalance(amount); // Cập nhật số dư của người cho vay
                            cout << "The lender's balance has been updated after the loan repayment.\n";
                        }
                        return;
                    }
                    // Cập nhật thông tin khoản vay
                    loan.update(newDueDate, newRate, newStatus);
                }
            }
            cout << "Loan from the lender not found: " << lenderName << endl;
        }


        // Quản lý khoản cho vay
        void addLend(const Lend& lend)
        {
            lends.push_back(lend);
        }

        void removeLend(const string& debtorName)
        {
            // Duyet danh sach khoan cho vay de xoa
            for (auto it = lends.begin(); it != lends.end(); ++it) 
            {
                if (it->getBorrowDetail().find(debtorName) != string::npos) 
                {
                    // Kiểm tra trạng thái khoản cho vay
                    if (it->getStatus() == 0)  // Nếu khoản cho vay chưa được trả
                    {   double amount = it->getAmount(); // Lấy số tiền khoản cho vay
                        this -> updateBalance(amount); // Hoàn lại số tiền cho vay vào số dư tài khoản
                        cout << "The debtor " << debtorName << " has not repaid the money, and the account balance has been updated.\n";
                    }
                    // Xóa khoản cho vay khỏi danh sách
                    lends.erase(it);
                    cout << "The borrower's loan: " << debtorName << " has been deleted.\n";
                    return;
                }
            }\
            cout << "The borrower's loan not found: " << debtorName << ".\n";
        }


        void updateLend(const string& debtorName, double newRate, const tm& newDueDate, bool newStatus)
        {
            // Tìm khoản cho vay theo tên người vay (debtorName)
            for (auto& lend : lends) 
            {
                if (lend.getBorrowDetail().find(debtorName) != string::npos) 
                {
                   
                    if (newStatus != lend.getStatus()) 
                    {// Nếu khoản vay đã trả (newStatus == true), cập nhật số dư tài khoản của người cho vay
                        if (newStatus == 1)  
                        {
                            // Cập nhật số dư của người cho vay
                            double amount = lend.calculateCompoundInterest();  // Số tiền đã cho vay + tiền lãi
                            this->money += amount;  // Cộng số tiền đã trả vào số dư tài khoản
                            cout << "Your account balance has been updated after the borrower repaid the loan.\n";
                        }
                        else{
                            // Cập nhật số dư của người cho vay
                            double amount = lend.calculateCompoundInterest();  // Số tiền đã cho vay + tiền lãi
                            this->money -= amount;  // Cộng số tiền đã trả vào số dư tài khoản
                            cout << "Your account balance has been updated after the borrower repaid the loan.\n";
                        }
                        return;
                    }
                     // Cập nhật lãi suất, ngày đến hạn và trạng thái
                    lend.update(newDueDate, newRate, newStatus);
                }
            }
            cout << "The borrower's loan not found " << debtorName << ".\n";
        }


        void lendHistory() const
        {
            cout << "Lend History:\n";
            for (const auto& lend : lends) 
                lend.getBorrowDetail();
        }

        // Báo cáo
        void reportUser() const
        {
            cout << "User ID: " << ID << ", Name: " << name << "\nUser Balances:" << this -> money << endl;
        }

        void reportAcc() const
        {
            cout << "Account Balances:\n";
            for (const auto& acc : accounts) 
                cout << acc.getAccountName() << ": " << acc.getBalance() << " VND\n";
        }

        void reportLoanAndLend() const
        {
            // Báo cáo các khoản vay (Loans)
            cout << "Loans:\n";
            for (const auto& loan : loans) 
            {
                loan.Notification();  // Thông báo nếu khoản vay chưa trả hoặc sắp đến hạn
            }

            cout << "\nLends:\n";
            for (const auto& lend : lends) 
            {
                lend.Notification();  // Thông báo nếu khoản cho vay chưa trả hoặc sắp đến hạn
            } 
        }


        // Báo cáo thu chi
        void reportInAndOut() const
        {
            double totalIncome = 0.0;
            double totalExpense = 0.0;

            // In ra tất cả các khoản thu
            cout << "\n--- Income  ---\n";
            for (const auto& trans : transactions) 
            {
                if (trans.getType() == "1") 
                {
                    totalIncome += trans.getAmount();
                    // In ra mỗi khoản thu trong một dòng với thông tin ID, danh mục, ngày tháng, và số tiền
                    cout << "ID: " << trans.getID() 
                        << ", Amount: " << trans.getAmount() << " VND\n"
                        << ", Date: " << trans.getDate() 
                        << ", Category: " << trans.getCategory() 
                        << ", Note: " << trans.getNote() << "\n";
                }
            }

            // In ra tổng thu
            cout << "\nTotal income: " << totalIncome << " VND\n";

            // In ra tất cả các khoản chi
            cout << "\n--- Expenses ---\n";
            for (const auto& trans : transactions) 
            {
                if (trans.getType() == "0") 
                {
                    totalExpense += trans.getAmount();
                    // In ra mỗi khoản chi trong một dòng với thông tin ID, danh mục, ngày tháng, và số tiền
                    cout << "ID: " << trans.getID() 
                        << ", Category: " << trans.getCategory() 
                        << ", Date: " << trans.getDate() 
                        << ", Amount: " << trans.getAmount() << " VND\n";
                }
            }

            // In ra tổng chi
            cout << "\nTotal expenses: " << totalExpense << " VND\n";
            cout << "Total income and expenses: " << totalIncome - totalExpense << " VND\n";
        }
        // Tổng số dư
        double getBalance() const 
        { 
            return this -> money; 
        }

        double updateBalance(double amount)
        {
            return this -> money += amount;
        }
        string getID() const
        {
            return this->ID;
        }
        // Trong lớp User.h, thêm phương thức getAccounts() để trả về danh sách tài khoản
        vector<Account>& getAccounts()
        {
            return accounts;
        }
        vector<Loan>& getLoans() 
        {
            return loans;
        }
        // Thêm phương thức getLends để lấy danh sách khoản cho vay
        vector<Lend>& getLends() 
        {
            return lends;
        }
        // Thêm phương thức getTransactions để lấy danh sách giao dịch
        vector<Transaction>& getTransactions() 
        {
            return transactions;
        }

    };

#endif
