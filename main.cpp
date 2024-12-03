#include <iostream>
#include <unordered_map> // Luu tru danh sach nguoi dung
#include "User.h"        // Tep tieu de chua lop User va cac lop lien quan
#include <fstream>  // Để sử dụng ofstream
#include <ctime>    // Dùng cho xử lý ngày tháng
#include <chrono> // Dùng cho xử lý thời gian
#include <cmath>
#include <sstream>
#include <iomanip>

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

// Lam tron so thap phan
double roundToTwoDecimal(double amount) 
{
    return round(amount * 100.0) / 100.0; // Làm tròn tới 2 chữ số thập phân
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
    cout << "6. Cac tinh nang dac biet\n";
    cout << "0. Dang xuat\n";
    cout << "====================\n";
    cout << "Lua chon: ";
}

// Hàm chuyển đổi từ string sang std::tm
std::tm convertStringToDate(const std::string& dateStr) 
{
    std::tm date = {};
    std::istringstream ss(dateStr);
    ss >> std::get_time(&date, "%Y-%m-%d");
    if (ss.fail()) {
        throw std::invalid_argument("Ngay thang khong hop le: " + dateStr);
    }
    return date;
}

// Hàm để yêu cầu người dùng nhập ngày đúng định dạng và kiểm tra điều kiện
std::tm getValidDateFromUser(const std::string& prompt, int allowFutureDate) 
{
    std::string dateStr;
    std::tm date;

    while (true)
     {
        std::cout << prompt;
        std::cin >> dateStr;

        try {
            date = convertStringToDate(dateStr); // Kiểm tra và chuyển đổi ngày tháng

            // Nếu không cho phép ngày trong tương lai
            if (allowFutureDate == 0) {
                std::time_t currentTime = std::time(nullptr); // Lấy thời gian hiện tại
                std::tm* currentDate = std::localtime(&currentTime); // Đổi sang std::tm

                // So sánh ngày nhập vào với ngày hiện tại
                if (date.tm_year > currentDate->tm_year || 
                    (date.tm_year == currentDate->tm_year && date.tm_mon > currentDate->tm_mon) ||
                    (date.tm_year == currentDate->tm_year && date.tm_mon == currentDate->tm_mon && date.tm_mday > currentDate->tm_mday)) {
                    throw std::invalid_argument("Ngày không thể là trong tương lai.");
                }
            }

            break;  // Nếu không có lỗi, thoát vòng lặp
        } catch (const std::invalid_argument& e) {
            std::cout << "Lỗi: " << e.what() << ". Vui lòng nhập lại ngày theo định dạng yyyy-mm-dd.\n";
        }
    }

    return date;
}

// Ham quan ly giao dich

void manageTransaction(User* currentUser) 
{
    // Kiểm tra nếu người dùng có tài khoản
    if (currentUser->getAccounts().empty()) {
        cout << "Bạn không có tài khoản nào. Vui lòng tạo tài khoản trước khi thực hiện giao dịch.\n";
        return;
    }

    // Hiển thị các tài khoản của người dùng
    cout << "Chọn tài khoản giao dịch:\n";
    int i = 1;
    for (const auto& acc : currentUser->getAccounts()) {
        cout << i++ << ". " << acc.getAccountName() << " - " << acc.getBalance() << " USD\n";
    }

    int accChoice;
    cout << "Lựa chọn tài khoản: ";
    cin >> accChoice;
    if (accChoice < 1 || accChoice > currentUser->getAccounts().size()) {
        cout << "Lựa chọn không hợp lệ. Vui lòng thử lại.\n";
        return;
    }

    // Lấy tài khoản được chọn
    Account& selectedAccount = currentUser->getAccounts()[accChoice - 1];
    
    string transID, date, type, category, note;
    double amount;

    // Nhập thông tin giao dịch
    cout << "Nhập ID giao dịch: ";
    cin >> transID;
    cout << "Nhập số tiền: ";
    cin >> amount;

    // Nhập ngày giao dịch, sử dụng hàm getValidDateFromUser để đảm bảo ngày hợp lệ
    std::tm transDate = getValidDateFromUser("Nhập ngày giao dịch (yyyy-mm-dd): ",0);

    cout << "Loại giao dịch (1: Thu / 0: Chi): ";
    cin >> type;
    cout << "Danh mục: ";
    cin.ignore();  // Để bỏ qua ký tự '\n' còn lại trong bộ đệm
    getline(cin, category);
    cout << "Ghi chú: ";
    getline(cin, note);

    // Kiểm tra loại giao dịch và cập nhật số dư tài khoản
    if (type == "1") {
        // Giao dịch thu (Income), cộng tiền vào tài khoản
        selectedAccount.updateBalance(amount);
        currentUser->updateBalance(amount);
        cout << "Giao dịch thu đã được thêm thành công. Số dư tài khoản đã được cập nhật!\n";
    } else if (type == "0") {
        // Giao dịch chi (Expense), trừ tiền từ tài khoản
        if (selectedAccount.getBalance() >= amount) {
            selectedAccount.updateBalance(-amount);
            currentUser->updateBalance(-amount);
            cout << "Giao dịch chi đã được thêm thành công. Số dư tài khoản đã được cập nhật!\n";
        } else {
            cout << "Số dư tài khoản không đủ để thực hiện giao dịch chi!\n";
            return;
        }
    } else {
        cout << "Loại giao dịch không hợp lệ. Vui lòng nhập 1 cho 'Thu' hoặc 0 cho 'Chi'.\n";
        return;
    }

    // Thêm giao dịch vào lịch sử giao dịch
    currentUser->addTrans(Transaction(transID, amount, transDate, type, category, note));
}

void exportReportToCSV(User* currentUser) 
{
    // Mở file CSV để ghi dữ liệu
    ofstream reportFile("report.csv");

    if (!reportFile) {
        cout << "Khong the mo file de ghi!\n";
        return;
    }

    // Ghi tiêu đề báo cáo
    reportFile << "Tong So Du," << fixed << setprecision(2) << currentUser->getBalance() << " USD\n";

    // Báo cáo tài khoản
    reportFile << "\n--- Danh Sach Tai Khoan ---\n";
    reportFile << "Tai Khoan,So Du\n";  // Tiêu đề cột cho các tài khoản
    for (const auto& acc : currentUser->getAccounts()) {
        reportFile << acc.getAccountName() 
                   << "," << fixed << setprecision(2) << acc.getBalance() << " USD\n";
    }

    // Báo cáo khoản vay
    reportFile << "\n--- Danh Sach Khoan Vay ---\n";
    reportFile << "Lender,Money,Rate,Due Date,Status\n";  // Tiêu đề cột
    for (const auto& loan : currentUser->getLoans()) 
    {
        // Sử dụng getLoanDetail() để lấy thông tin chi tiết về khoản vay
        string loanDetail = loan.getLoanDetail();
        reportFile << loanDetail << endl; // Ghi thông tin khoản vay vào file
    }

    // Báo cáo khoản cho vay
    reportFile << "\n--- Danh Sach Khoan Cho Vay ---\n";
    reportFile << "Debtor,Money,Rate,Due Date,Status\n";  // Tiêu đề cột
    for (const auto& lend : currentUser->getLends()) 
    {
        // Sử dụng getBorrowDetail() để lấy thông tin chi tiết về khoản cho vay
        string borrowDetail = lend.getBorrowDetail();
        reportFile << borrowDetail << endl; // Ghi thông tin khoản cho vay vào file
    }

    // Báo cáo thu chi
    double totalIncome = 0.0;
    double totalExpense = 0.0;
    reportFile << "\n--- Bao Cao Thu Chi ---\n";
    reportFile << "Type,ID,Category,Date,Amount\n";  // Tiêu đề cột
    for (const auto& trans : currentUser->getTransactions()) {
        if (trans.getType() == "1") 
        {  // Khoan thu
            totalIncome += trans.getAmount();
            reportFile << "Income," << trans.getID() 
                       << "," << trans.getCategory() 
                       << "," << trans.getDate() 
                       << "," << trans.getAmount() << " USD\n";
        } 
        else if (trans.getType() == "0") 
        {  // Khoan chi
            totalExpense += trans.getAmount();
            reportFile << "Expense," << trans.getID() 
                       << "," << trans.getCategory() 
                       << "," << trans.getDate() 
                       << "," << trans.getAmount() << " USD\n";
        }
    }
    reportFile << "\nTong Thu," << totalIncome << " USD\n";
    reportFile << "Tong Chi," << totalExpense << " USD\n";
    reportFile << "Chenh Lech Thu Chi," << totalIncome - totalExpense << " USD\n";

    // Đóng file
    reportFile.close();
    cout << "Bao cao da duoc xuat ra file report.csv\n";
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

                    switch (loanChoice) 
                    {
                        case 1: 
                        { // Thêm khoản cho vay
                            string debtorName;
                            double amount, interestRate;
                            bool status;

                            cout << "Nhập tên người cho bạn vay: ";
                            cin.ignore(); // Đảm bảo không bỏ qua dấu cách khi nhập tên
                            getline(cin, debtorName);
                            
                            cout << "Nhập số tiền vay: ";
                            cin >> amount;

                            cout << "Nhập lãi suất (%): ";
                            cin >> interestRate;

                            // Sử dụng hàm getValidDateFromUser để nhập và kiểm tra ngày tạo và ngày đến hạn
                            std::tm createDate = getValidDateFromUser("Nhập ngày tạo (yyyy-mm-dd): ",0);
                            std::tm dueDate = getValidDateFromUser("Nhập ngày đến hạn (yyyy-mm-dd): ",1);

                            cout << "Trạng thái (0: Chưa trả, 1: Đã trả): ";
                            cin >> status;

                            // Thêm khoản  vay vào danh sách
                            currentUser->addLoan(Loan(debtorName, amount, interestRate, dueDate, createDate, status));
                            cout << "Khoản vay đã được thêm thành công!\n";

                            // Cập nhật số dư khi  vay
                            currentUser->updateBalance(-amount);
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
                        case 3: 
                        { 
                            // Cập nhật khoản vay
                            string lenderName;
                            double newRate;
                            bool newStatus;

                            cout << "Nhập tên người cho vay để cập nhật: ";
                            cin.ignore();
                            getline(cin, lenderName);

                            // Tìm khoản vay theo tên người cho vay
                            bool loanFound = false;
                            for (auto& loan : currentUser->getLoans()) 
                            {
                                if (loan.getLoanDetail().find(lenderName) != string::npos) 
                                {
                                    loanFound = true;

                                    // Nhập lãi suất mới
                                    cout << "Nhập lãi suất mới (%): ";
                                    cin >> newRate;

                                    // Nhập ngày đến hạn mới bằng hàm getValidDateFromUser
                                    tm newDueDate = getValidDateFromUser("Nhập ngày đến hạn mới (yyyy-mm-dd): ",1);
                                    
                                    // Nhập trạng thái mới (0: Chưa trả, 1: Đã trả)
                                    cout << "Nhập trạng thái mới (0: Chưa trả, 1: Đã trả): ";
                                    cin >> newStatus;

                                    // Cập nhật thông tin khoản vay
                                    currentUser->updateLoan(lenderName, newRate, newDueDate, newStatus);
                                    cout << "Khoản vay của " << lenderName << " đã được cập nhật thành công.\n";
                                    break;  // Thoát khỏi vòng lặp sau khi cập nhật thành công
                                }
                            }

                            if (!loanFound) {
                                cout << "Không tìm thấy khoản vay của người cho vay " << lenderName << ".\n";
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
                        case 1: 
                        { 
                            // Thêm khoản cho vay
                            string debtorName, dueDateStr, createDateStr;
                            double amount, interestRate;
                            bool status;

                            // Nhập tên người vay
                            cout << "Nhập tên người vay: ";
                            cin.ignore();  // Để bỏ qua ký tự '\n' còn lại trong bộ đệm
                            getline(cin, debtorName);

                            // Nhập số tiền cho vay
                            cout << "Nhập số tiền cho vay: ";
                            cin >> amount;

                            // Nhập lãi suất
                            cout << "Nhập lãi suất (%): ";
                            cin >> interestRate;

                            // Nhập và kiểm tra ngày tạo
                            std::tm createDate = getValidDateFromUser("Nhập ngày tạo (yyyy-mm-dd): ",0);

                            // Nhập và kiểm tra ngày đến hạn
                            std::tm dueDate = getValidDateFromUser("Nhập ngày đến hạn (yyyy-mm-dd): ",1);

                            // Nhập trạng thái khoản cho vay
                            cout << "Trạng thái (0: Chưa trả, 1: Đã trả): ";
                            cin >> status;

                            // Thêm khoản cho vay vào danh sách
                            currentUser->addLend(Lend(debtorName, amount, interestRate, dueDate, createDate, status));
                            cout << "Khoản cho vay đã được thêm thành công!\n";

                            // Cập nhật số dư khi cho vay
                            currentUser->updateBalance(-amount); 
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
                        { 
                            // Cập nhật khoản cho vay
                            string debtorName;
                            double newRate;
                            string newDueDateStr;
                            bool newStatus;

                            cout << "Nhập tên người vay để cập nhật: ";
                            cin.ignore();
                            getline(cin, debtorName);

                            // Tìm khoản cho vay theo tên người vay
                            bool lendFound = false;
                            for (auto& lend : currentUser->getLends()) 
                            {
                                if (lend.getBorrowDetail().find(debtorName) != string::npos) 
                                {
                                    lendFound = true;

                                    // Nhập lại lãi suất mới
                                    cout << "Nhập lãi suất mới (%): ";
                                    cin >> newRate;

                                    // Nhập ngày đến hạn mới và kiểm tra ngày hợp lệ
                                    cout << "Nhập ngày đến hạn mới (yyyy-mm-dd): ";
                                    cin >> newDueDateStr;
                                    // Kiểm tra và chuyển đổi ngày tháng sang std::tm
                                    std::tm newDueDate = getValidDateFromUser("Nhập ngày đến hạn mới (yyyy-mm-dd): ",1);
                                    // Nhập trạng thái mới (0: Chưa trả, 1: Đã trả)
                                    cout << "Nhập trạng thái mới (0: Chưa trả, 1: Đã trả): ";
                                    cin >> newStatus;
                                    // Cập nhật thông tin khoản vay
                                    currentUser->updateLend(debtorName, newRate, newDueDate, newStatus);
                                    cout << "Khoản cho vay của " << debtorName << " đã được cập nhật thành công.\n";
                                    break;  // Thoát khỏi vòng lặp sau khi cập nhật thành công
                                }
                            }
                            if (!lendFound) {
                                cout << "Không tìm thấy khoản cho vay của người vay " << debtorName << ".\n";
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
                    do
                    {     
                        cin >> choice;
                        switch (choice) 
                        {
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
                    } while(choice != 0);
                    break;
                }
                case 6: 
                {
                    cout << "1. Xuat bao cao ra file CSV\n";
                
                    cout << "0. Quay lai\n";
                    cout << "Lua chon: ";
                    cin >> choice;
                    switch (choice) 
                    {
                        case 1: 
                        {
                            exportReportToCSV(currentUser);
                            break;
                        }
                        case 0: 
                            break;
                        default:
                            cout << "Lua chon khong hop le. Vui long thu lai.\n";
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
