#include <iostream>
#include <unordered_map> // Luu tru danh sach nguoi dung
#include "User.h"        // Tep tieu de chua lop User va cac lop lien quan
using namespace std;

// Hien thi menu dang nhap
void showLoginMenu() 
{
    cout << "\n--- Quan ly Tai chinh ---\n";
    cout << "1. Dang nhap\n";
    cout << "2. Dang ky tai khoan moi\n";
    cout << "0. Thoat\n";
    cout << "Lua chon: ";
}

// Ham dang nhap
User* login(unordered_map<string, User>& users) 
{
    string userID, password;
    cout << "Nhap ID nguoi dung: ";
    cin >> userID;

    // Kiem tra nguoi dung co ton tai khong
    auto it = users.find(userID);
    if (it != users.end()) { // Neu tim thay ID trong danh sach nguoi dung
        User& user = it->second;
        cout << "Nhap mat khau: ";
        cin >> password;

        // Kiem tra mat khau
        if (user.authenticate(password)) {
            cout << "Dang nhap thanh cong! Xin chao, " << userID << ".\n";
            return &user; // Tra ve con tro den doi tuong User
        }
        else {
            cout << "Sai mat khau. Vui long thu lai.\n";
        }
    } else { // Neu ID khong ton tai
        cout << "Nguoi dung khong ton tai.\n";
    }

    return nullptr; // Tra ve nullptr neu khong dang nhap thanh cong
}

// Ham dang ky tai khoan moi
void registerUser(unordered_map<string, User>& users) {
    string userID, userName, password;
    cout << "Nhap ID nguoi dung: ";
    cin >> userID;

    // Kiem tra neu ID da ton tai
    if (users.find(userID) != users.end()) {
        cout << "ID nay da ton tai. Vui long chon ID khac.\n";
        return;
    }

    cout << "Nhap ten nguoi dung: ";
    cin.ignore();
    getline(cin, userName);
    cout << "Nhap mat khau: ";
    cin >> password;

    // Them nguoi dung moi vao danh sach
    users.emplace(userID, User(userID, userName, password));
    cout << "Tai khoan da duoc tao thanh cong! Ban co the dang nhap ngay bay gio.\n";
}

// Hien thi menu chinh
void showMainMenu() 
{
    cout << "\n====================\n";
    cout << "1. Quan ly tai khoan\n";
    cout << "2. Quan ly giao dich\n";
    cout << "3. Quan ly khoan vay\n";
    cout << "4. Quan ly khoan cho vay\n";
    cout << "5. Bao cao tai chinh\n";
    cout << "0. Dang xuat\n";
    cout << "====================\n";
    cout << "Lua chon: ";
}

// Ham quan ly giao dich
void manageTransaction(User* currentUser) 
{
    // Kiem tra neu nguoi dung co tai khoan
    if (currentUser->getAccounts().empty()) {
        cout << "Ban khong co tai khoan nao. Vui long tao tai khoan truoc khi thuc hien giao dich.\n";
        return;
    }

    // Hien thi cac tai khoan cua nguoi dung
    cout << "Chon tai khoan giao dich:\n";
    int i = 1;
    for (const auto& acc : currentUser->getAccounts()) {
        cout << i++ << ". " << acc.getAccountName() << " - " << acc.getBalance() << " USD\n";
    }

    int accChoice;
    cout << "Lua chon tai khoan: ";
    cin >> accChoice;
    if (accChoice < 1 || accChoice > currentUser->getAccounts().size()) {
        cout << "Lua chon khong hop le. Vui long thu lai.\n";
        return;
    }
    // Lay tai khoan duoc chon
    Account& selectedAccount = currentUser->getAccounts()[accChoice - 1];
    
    string transID, date, type, category, note;
    double amount;

    // Nhap thong tin giao dich
    cout << "Nhap ID giao dich: ";
    cin >> transID;
    cout << "Nhap so tien: ";
    cin >> amount;
    cout << "Nhap ngay giao dich (yyyy-mm-dd): ";
    cin >> date;
    cout << "Loai giao dich (1: Thu / 0: Chi): ";
    cin >> type;
    cout << "Danh muc: ";
    cin.ignore();
    getline(cin, category);
    cout << "Ghi chu: ";
    getline(cin, note);

    // Kiem tra loai giao dich va cap nhat so du tai khoan
    if (type == "1") {
        // Giao dich thu (Income), cong tien vao tai khoan
        selectedAccount.updateBalance(amount);
        currentUser->updateBalance(amount);
        cout << "Giao dich thu da duoc them thanh cong. So du tai khoan da duoc cap nhat!\n";
    } else if (type == "0") {
        // Giao dich chi (Expense), tru tien tu tai khoan
        if (selectedAccount.getBalance() >= amount) {
            selectedAccount.updateBalance(-amount);
            currentUser->updateBalance(-amount);
            cout << "Giao dich chi da duoc them thanh cong. So du tai khoan da duoc cap nhat!\n";
        } 
        else 
        {
            cout << "So du tai khoan khong du de thuc hien giao dich chi!\n";
            return;
        }
    } 
    else {
        cout << "Loai giao dich khong hop le. Vui long nhap 1 cho 'Thu' hoac 0 cho 'Chi'.\n";
        return;
    }

    // Them giao dich vao lich su giao dich
    currentUser->addTrans(Transaction(transID, amount, date, type, category, note));
}

int main() 
{
    unordered_map<string, User> users; // Luu danh sach nguoi dung
    User* currentUser = nullptr;      // Nguoi dung hien tai
    int choice;

    do {
        // Neu chua dang nhap, hien thi menu dang nhap
        if (currentUser == nullptr) 
        {
            showLoginMenu();
            cin >> choice;
            switch (choice) 
            {
                case 1: // Dang nhap
                    currentUser = login(users);
                    break;
                case 2: // Dang ky tai khoan moi
                    registerUser(users);
                    break;
                case 0: // Thoat
                    cout << "Thoat chuong trinh...\n";
                    return 0;
                default:
                    cout << "Lua chon khong hop le. Vui long thu lai.\n";
            }
        } 
        else 
        {
            // Neu da dang nhap, hien thi menu chinh
            showMainMenu();
            cin >> choice;

            switch (choice) 
            {
                case 1: 
                { // Quan ly tai khoan
                    int accountChoice;
                    cout << "1. Them tai khoan\n2. Xoa tai khoan\n3. Cap nhat so du tai khoan\n0. Quay lai\nLua chon: ";
                    cin >> accountChoice;

                    if (accountChoice == 1) { // Them tai khoan
                        string accID, accName;
                        double balance;
                        cout << "Nhap ID tai khoan: ";
                        cin >> accID;
                        cout << "Nhap ten tai khoan: ";
                        cin.ignore();
                        getline(cin, accName);
                        cout << "Nhap so du ban dau: ";
                        cin >> balance;
                        currentUser->addAccount(Account(accID, accName, balance));
                        cout << "Tai khoan da duoc them thanh cong!\n";
                    } 
                    else if (accountChoice == 2) 
                    { // Xoa tai khoan
                        string accID;
                        cout << "Nhap ID tai khoan can xoa: ";
                        cin >> accID;
                        currentUser->removeAccount(accID);
                        cout << "Tai khoan da duoc xoa (neu ton tai).\n";
                    } else if (accountChoice == 3) { // Cap nhat so du
                        string accID;
                        double newBalance;
                        cout << "Nhap ID tai khoan can cap nhat: ";
                        cin >> accID;
                        cout << "Nhap so du moi: ";
                        cin >> newBalance;
                        currentUser->updateAccount(accID, newBalance);
                        cout << "So du tai khoan da duoc cap nhat.\n";
                    }
                    break;
                }
                case 2: // Quan ly giao dich
                {
                    int transChoice;
                    cout << "1. Them giao dich\n";
                    cout << "2. Xem lich su giao dich\n";
                    cout << "3. Cap nhat giao dich\n";
                    cout << "4. Xoa giao dich\n";
                    cout << "0. Quay lai\n";
                    cout << "Lua chon: ";
                    cin >> transChoice;

                    switch (transChoice) {
                        case 1: 
                        { // Them giao dich
                            manageTransaction(currentUser);
                            break;
                        }
                        case 2: 
                        { // Xem lich su giao dich
                            currentUser->transHistory();
                            break;
                        }      
                        case 3: 
                        { // Cap nhat giao dich
                            string transID;
                            double newAmount;
                            string newNote;

                            cout << "Nhap ID giao dich can cap nhat: ";
                            cin >> transID;
                            cout << "Nhap so tien moi: ";
                            cin >> newAmount;
                            cout << "Nhap ghi chu moi: ";
                            cin.ignore();  // De co the nhap ghi chu dang chuoi
                            getline(cin, newNote);

                            currentUser->updateTrans(transID, newAmount, newNote);
                            break;
                        }
                        case 4: { // Xoa giao dich
                            string transID;
                            cout << "Nhap ID giao dich can xoa: ";
                            cin >> transID;
                            currentUser->removeTrans(transID);
                            break;
                        }
                        case 0:
                            break;
                        default:
                            cout << "Lua chon khong hop le. Vui long thu lai.\n";
                            break;
                    }
                    break;
                }
                case 3: // Quan ly khoan vay
                {
                    int loanChoice;
                    cout << "1. Them khoan vay\n";
                    cout << "2. Xem danh sach khoan vay\n";
                    cout << "3. Cap nhat khoan vay\n";
                    cout << "4. Xoa khoan vay\n";
                    cout << "0. Quay lai\n";
                    cout << "Lua chon: ";
                    cin >> loanChoice;

                    switch (loanChoice) {
                        case 1: 
                        { // Them khoan vay
                            string lenderName, dueDate;
                            double amount, interestRate;
                            bool status;

                            cout << "Nhap ten nguoi cho vay: ";
                            cin.ignore();
                            getline(cin, lenderName);
                            cout << "Nhap so tien vay: ";
                            cin >> amount;
                            cout << "Nhap lai suat (%): ";
                            cin >> interestRate;
                            cout << "Nhap ngay den han (yyyy-mm-dd): ";
                            cin >> dueDate;
                            cout << "Trang thai (0: Chua tra, 1: Da tra): ";
                            cin >> status;
                            currentUser->addLoan(Loan(lenderName, amount, interestRate, dueDate, status));
                            cout << "Khoan vay da duoc them thanh cong!\n";
                            currentUser->updateBalance(amount);
                            break;
                        }
                        case 2: 
                        { // Xem danh sach khoan vay
                            cout << "Danh sach khoan vay:\n";
                            for (const auto& loan : currentUser->getLoans()) 
                            {
                                cout << loan.getLoanDetail() << endl;
                            }
                            break;
                        }
                        case 3: //Cap nhat khoan vay  
                        { 
                            
                            string lenderName;
                            double newRate;
                            string newDueDate;
                            bool newStatus;
                            cout << "Nhap ten nguoi cho vay de cap nhat: ";
                            cin.ignore();
                            getline(cin, lenderName);

                            // Tim khoan vay theo ten nguoi cho vay
                            for (auto& loan : currentUser->getLoans()) 
                            {
                                if (loan.getLoanDetail().find(lenderName) != string::npos) 
                                {
                                    cout << "Nhap lai suat moi (%): ";
                                    cin >> newRate;
                                    cout << "Nhap ngay den han moi (yyyy-mm-dd): ";
                                    cin >> newDueDate;
                                    cout << "Nhap trang thai moi (0: Chua tra, 1: Da tra): ";
                                    cin >> newStatus;
                                    currentUser->updateLoan(lenderName, newRate, newDueDate, newStatus);
                                    break;      
                                }
                            }
                            break;
                        }
                        case 4: 
                        { // Xoa khoan vay
                            string lenderName;
                            cout << "Nhap ten nguoi cho vay de xoa khoan vay: ";
                            cin.ignore();
                            getline(cin, lenderName);

                            currentUser->removeLoan(lenderName);
                            cout << "Khoan vay da duoc xoa (neu co).\n";
                            break;
                        }
                        case 0:
                            break;
                        default:
                            cout << "Lua chon khong hop le. Vui long thu lai.\n";
                            break;
                    }
                    break;
                }
                case 4: // Quan ly khoan cho vay
                {
                    int lendChoice;
                    cout << "1. Them khoan cho vay\n";
                    cout << "2. Xem danh sach khoan cho vay\n";
                    cout << "3. Cap nhat khoan cho vay\n";
                    cout << "4. Xoa khoan cho vay\n";
                    cout << "0. Quay lai\n";
                    cout << "Lua chon: ";
                    cin >> lendChoice;

                    switch (lendChoice) 
                    {
                        case 1: { // Them khoan cho vay
                            string debtorName, dueDate, createDate;
                            double amount, interestRate;
                            bool status;

                            cout << "Nhap ten nguoi vay: ";
                            cin.ignore();
                            getline(cin, debtorName);
                            cout << "Nhap so tien cho vay: ";
                            cin >> amount;
                            cout << "Nhap lai suat (%): ";
                            cin >> interestRate;
                            cout << "Nhap ngay tao (yyyy-mm-dd): ";
                            cin >> createDate;
                            cout << "Nhap ngay den han (yyyy-mm-dd): ";
                            cin >> dueDate;
                            cout << "Trang thai (0: Chua tra, 1: Da tra): ";
                            cin >> status;

                            // Them khoan cho vay vao danh sach
                            currentUser->addLend(Lend(debtorName, amount, interestRate, dueDate, createDate, status));
                            cout << "Khoan cho vay da duoc them thanh cong!\n";
                            currentUser->updateBalance(-amount); // Giam so du khi cho vay
                            break;
                        }
                        case 2: { // Xem danh sach khoan cho vay
                            cout << "Danh sach khoan cho vay:\n";
                            for (const auto& lend : currentUser->getLends()) {
                                cout << lend.getBorrowDetail() << endl;
                            }
                            break;
                        }
                        case 3: 
                        { // Cap nhat khoan cho vay
                            string debtorName;
                            double newRate;
                            string newDueDate;
                            bool newStatus;
                            cout << "Nhap ten nguoi vay de cap nhat: ";
                            cin.ignore();
                            getline(cin, debtorName);

                            // Tim khoan cho vay theo ten nguoi vay
                            for (auto& lend : currentUser->getLends()) 
                            {
                                if (lend.getBorrowDetail().find(debtorName) != string::npos) 
                                {
                                    cout << "Nhap lai suat moi (%): ";
                                    cin >> newRate;
                                    cout << "Nhap ngay den han moi (yyyy-mm-dd): ";
                                    cin >> newDueDate;
                                    cout << "Nhap trang thai moi (0: Chua tra, 1: Da tra): ";
                                    cin >> newStatus;

                                    currentUser->updateLend(debtorName, newRate, newDueDate, newStatus);
                                    break;
                                }
                            }
                            break;
                        }
                        case 4: 
                        { // Xoa khoan cho vay
                            string debtorName;
                            cout << "Nhap ten nguoi vay de xoa khoan cho vay: ";
                            cin.ignore();
                            getline(cin, debtorName);

                            currentUser->removeLend(debtorName);
                            cout << "Khoan cho vay da duoc xoa (neu co).\n";
                            break;
                        }
                        case 0:
                            break;
                        default:
                            cout << "Lua chon khong hop le. Vui long thu lai.\n";
                            break;
                    }
                    break;
                }
                case 5: // Bao cao tai chinh
                {
                    cout << "\n--- Bao cao tai chinh ---\n";
                    cout << "1. Bao cao tong so du\n";
                    cout << "2. Bao cao tai khoan\n";
                    cout << "3. Bao cao khoan vay va khoan cho vay\n";
                    cout << "4. Bao cao thu chi\n";
                    cout << "0. Quay lai\n";
                    cout << "Lua chon: ";
                    cin >> choice;

                    switch (choice) {
                        case 1: { // Bao cao tong so du
                            cout << "Tong so du cua ban la: " << currentUser->getBalance() << " USD\n";
                            break;
                        }
                        case 2: { // Bao cao tai khoan
                            currentUser->reportAcc();
                            break;
                        }
                        case 3: { // Bao cao khoan vay va khoan cho vay
                            currentUser->reportLoanAndLend();
                            break;
                        }
                        case 4: 
                        { 
                            // Bao cao thu chi
                            currentUser->reportInAndOut();
                            break;
                        }
                        case 0:
                            break;
                        default:
                            cout << "Lua chon khong hop le. Vui long thu lai.\n";
                            break;
                    }
                    break;
                }
                case 0: // Dang xuat
                    cout << "Dang xuat thanh cong!\n";
                    currentUser = nullptr; // Xoa trang thai dang nhap
                    break;
                default:
                    cout << "Lua chon khong hop le. Vui long thu lai.\n";
            }
        }
    } while (true);

    return 0;
}
