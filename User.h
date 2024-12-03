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
                cout << "So du tai khoan " << accID << " la " << balance << " USD da duoc tru ra khoi so du cua nguoi dung.\n";
                // Xóa tài khoản khỏi danh sách
                accounts.erase(it);
                cout << "Tai khoan " << accID << " da duoc xoa.\n";
            } 
            else 
            {
                cout << "Khong tim thay tai khoan voi ID " << accID << ".\n";
            }
        }

        void updateAccount(const string& accID, double newBalance)
        {
            for (auto& acc : accounts) 
            {
                if (acc.getAccountID() == accID) {
                    money -= acc.getBalance();
                    acc.updateBalance(newBalance - acc.getBalance());
                    money += acc.getBalance();
                    cout << "Tài khoản " << accID << " đã được cập nhật thành công.\n";
                    return;
                }
            }
            cout << "Không tìm thấy tài khoản với ID " << accID << ".\n";
        }

        void banking(const string& fromAccID, const string& toAccID, double amount)              
        {
            Account *fromAcc = nullptr, *toAcc = nullptr;

            for (auto& acc : accounts) {
                if (acc.getAccountID() == fromAccID) fromAcc = &acc;
                if (acc.getAccountID() == toAccID) toAcc = &acc;
            }

            if (!fromAcc || !toAcc) {
                cout << "Lỗi: Một hoặc cả hai ID tài khoản không hợp lệ.\n";
                return;
            }

            if (fromAcc->getBalance() < amount) {
                cout << "Lỗi: Tài khoản " << fromAccID << " không đủ tiền để chuyển.\n";
                return;
            }

            fromAcc->updateBalance(-amount);
            toAcc->updateBalance(amount);
            cout << "Chuyển khoản thành công: " << amount << " USD từ " 
                << fromAccID << " sang " << toAccID << endl;
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
                cout << "Giao dịch với ID " << transID << " đã được xóa.\n";
            } else {
                cout << "Không tìm thấy giao dịch với ID " << transID << ".\n";
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
                        double diff = newAmount - oldAmount;  // Tính chênh lệch số tiền
                        this->updateBalance(diff);  // Cập nhật số dư tài khoản
                        cout << "So du tai khoan da duoc cap nhat sau khi thay doi giao dich.\n";
                    }

                    cout << "Giao dich " << transID << " da duoc cap nhat.\n";
                    return;
                }
            }
            cout << "Khong tim thay giao dich voi ID " << transID << ".\n";
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
                    if (!it->getStatus()) { // Nếu khoản vay chưa được trả
                        double amount = it->getAmount(); // Lấy số tiền khoản vay
                        this -> money += -amount; // Hoàn trả số tiền cho vay vào tài khoản của người cho vay
                        cout << "Khoan vay cua " << lenderName << " chua duoc tra, so du tai khoan da duoc cap nhat.\n";
                    }  
                    // Xoa khoan vay
                    loans.erase(it);
                    cout << "Khoan vay cua " << lenderName << " da duoc xoa.\n";
                    return;
                }
            }
            cout << "Khong tim thay khoan vay cua nguoi cho vay " << lenderName << ".\n";
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

                    // Cập nhật thông tin khoản vay
                    loan.update(newDueDate, newRate, newStatus);

                    // Nếu trạng thái là "đã trả", cập nhật số dư của người cho vay
                    if (newStatus) 
                    {
                        double amount = loan.getAmount();  // Lấy số tiền đã cho vay
                        this->updateBalance(amount); // Cập nhật số dư của người cho vay
                        cout << "Số dư của người cho vay đã được cập nhật sau khi khoản vay đã trả.\n";
                    }
                    cout << "Khoản vay của " << lenderName << " đã được cập nhật thành công.\n";
                    return;
                }
            }
            cout << "Không tìm thấy khoản vay của người cho vay " << lenderName << ".\n";
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
                    if (!it->getStatus())  // Nếu khoản cho vay chưa được trả
                    {   double amount = it->getAmount(); // Lấy số tiền khoản cho vay
                        this -> updateBalance(amount); // Hoàn lại số tiền cho vay vào số dư tài khoản
                        cout << "Khoan cho vay cua " << debtorName << " chua duoc tra, so du tai khoan da duoc cap nhat.\n";
                    }
                    // Xóa khoản cho vay khỏi danh sách
                    lends.erase(it);
                    cout << "Khoan cho vay cua " << debtorName << " da duoc xoa.\n";
                    return;
                }
            }

            cout << "Khong tim thay khoan cho vay cua nguoi vay " << debtorName << ".\n";
        }


        void updateLend(const string& debtorName, double newRate, const tm& newDueDate, bool newStatus)
        {
            // Tìm khoản cho vay theo tên người vay (debtorName)
            for (auto& lend : lends) 
            {
                if (lend.getBorrowDetail().find(debtorName) != string::npos) 
                {
                    // Cập nhật lãi suất, ngày đến hạn và trạng thái
                    lend.update(newDueDate, newRate, newStatus);

                    // Nếu khoản vay đã trả (newStatus == true), cập nhật số dư tài khoản của người cho vay
                    if (newStatus) 
                    {
                        // Cập nhật số dư của người cho vay
                        double amount = lend.getAmount();  // Số tiền đã cho vay
                        this->money += amount;  // Cộng số tiền đã trả vào số dư tài khoản
                        cout << "Số dư tài khoản của bạn đã được cập nhật sau khi người mượn trả khoản vay.\n";
                    }

                    cout << "Khoản cho vay của " << debtorName << " đã được cập nhật thành công.\n";
                    return;
                }
            }
            cout << "Không tìm thấy khoản cho vay của người vay " << debtorName << ".\n";
        }



        void lendHistory() const
        {
            cout << "Lend History:\n";
            for (const auto& lend : lends) 
            {
                lend.getBorrowDetail();
            }
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
            {
                cout << acc.getAccountName() << ": " << acc.getBalance() << " USD\n";
            }
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
            cout << "\n--- Các khoản thu ---\n";
            for (const auto& trans : transactions) 
            {
                if (trans.getType() == "1") 
                {
                    totalIncome += trans.getAmount();
                    // In ra mỗi khoản thu trong một dòng với thông tin ID, danh mục, ngày tháng, và số tiền
                    cout << "ID: " << trans.getID() 
                        << ", Danh mục: " << trans.getCategory() 
                        << ", Ngày: " << trans.getDate() 
                        << ", Số tiền: " << trans.getAmount() << " USD\n";
                }
            }

            // In ra tổng thu
            cout << "\nTổng thu: " << totalIncome << " USD\n";

            // In ra tất cả các khoản chi
            cout << "\n--- Các khoản chi ---\n";
            for (const auto& trans : transactions) 
            {
                if (trans.getType() == "0") 
                {
                    totalExpense += trans.getAmount();
                    // In ra mỗi khoản chi trong một dòng với thông tin ID, danh mục, ngày tháng, và số tiền
                    cout << "ID: " << trans.getID() 
                        << ", Danh mục: " << trans.getCategory() 
                        << ", Ngày: " << trans.getDate() 
                        << ", Số tiền: " << trans.getAmount() << " USD\n";
                }
            }

            // In ra tổng chi
            cout << "\nTổng chi: " << totalExpense << " USD\n";
            cout << "Tổng thu chi: " << totalIncome - totalExpense << " USD\n";
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
