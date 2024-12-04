#include <iostream>
#include <unordered_map> // For storing user list
#include "User.h"        // Header file for User class and related classes
#include <fstream>  // For using ofstream
#include <ctime>    // For handling date and time
#include <chrono> // For handling time
#include <cmath>
#include <sstream>
#include <iomanip>
#include <string>
#include <stdexcept>
using namespace std;

// Show login menu
void showLoginMenu() 
{
    cout << "\n--- Financial Management ---\n";
    cout << "1. Login\n";
    cout << "2. Register a new account\n";
    cout << "0. Exit\n";
    cout << "Choose an option: ";
}

// Round to two decimal places
double roundToTwoDecimal(double amount) 
{
    return round(amount * 100.0) / 100.0; // Round to two decimal places
}

// Login function
User* login(unordered_map<string, User>& users) 
{
    string userID, password;
    cout << "Enter user ID: ";
    cin >> userID;

    // Check if user exists
    auto it = users.find(userID);
    if (it != users.end()) { // If ID is found in the user list
        User& user = it->second;
        cout << "Enter password: ";
        cin >> password;

        // Check password
        if (user.authenticate(password)) {
            cout << "Login successful! Welcome, " << userID << ".\n";
            return &user; // Return pointer to User object
        }
        else {
            cout << "Incorrect password. Please try again.\n";
        }
    } else { // If ID does not exist
        cout << "User does not exist.\n";
    }

    return nullptr; // Return nullptr if login fails
}

// Register new user
void registerUser(unordered_map<string, User>& users) {
    string userID, userName, password;
    cout << "Enter user ID: ";
    cin >> userID;

    // Check if ID already exists
    if (users.find(userID) != users.end()) {
        cout << "This ID is already taken. Please choose another one.\n";
        return;
    }

    cout << "Enter user name: ";
    cin.ignore();
    getline(cin, userName);
    cout << "Enter password: ";
    cin >> password;

    // Add new user to the list
    users.emplace(userID, User(userID, userName, password));
    cout << "Account created successfully! You can log in now.\n";
}

// Show main menu
void showMainMenu() 
{
    cout << "\n====================\n";
    cout << "1. Manage accounts\n";
    cout << "2. Manage transactions\n";
    cout << "3. Manage loans\n";
    cout << "4. Manage lends\n";
    cout << "5. Financial report\n";
    cout << "6. Export report to file CSV\n";
    cout << "0. Logout\n";
    cout << "====================\n";
    cout << "Choose an option: ";
}


// Kiểm tra ngày hợp lệ
bool isValidDate(int year, int month, int day) {
    if (year < 0 || month < 1 || month > 12 || day < 1) return false;

    // Số ngày trong từng tháng
    const int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // Kiểm tra năm nhuận
    bool isLeapYear = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));

    int maxDay = daysInMonth[month - 1];
    if (month == 2 && isLeapYear) maxDay = 29; // Tháng 2 năm nhuận

    return day <= maxDay;
}

// Chuyển chuỗi ngày sang std::tm
std::tm convertStringToDate(const std::string& dateStr) {
    std::tm date = {};
    std::istringstream ss(dateStr);
    char delimiter;

    // Định dạng: yyyy-mm-dd
    ss >> date.tm_year >> delimiter >> date.tm_mon >> delimiter >> date.tm_mday;

    // Kiểm tra định dạng
    if (ss.fail() || delimiter != '-' || date.tm_year < 1900) {
        throw std::invalid_argument("Invalid date format. Use yyyy-mm-dd.");
    }

    // Chỉnh lại tm_year và tm_mon (theo std::tm)
    date.tm_year -= 1900;
    date.tm_mon -= 1;

    // Kiểm tra tính hợp lệ của ngày
    if (!isValidDate(date.tm_year + 1900, date.tm_mon + 1, date.tm_mday)) {
        throw std::invalid_argument("Invalid date: The specified date does not exist.");
    }

    return date;
}

// Hàm nhập ngày hợp lệ
std::tm getValidDateFromUser(const std::string& prompt, int allowFutureDate) {
    std::string dateStr;
    std::tm date;

    while (true) {
        std::cout << prompt;
        std::cin >> dateStr;

        try {
            date = convertStringToDate(dateStr); // Kiểm tra và chuyển chuỗi ngày

            // Nếu không cho phép nhập ngày tương lai
            if (allowFutureDate == 0) {
                std::time_t currentTime = std::time(nullptr); // Lấy thời gian hiện tại
                std::tm* currentDate = std::localtime(&currentTime); // Chuyển sang std::tm

                // So sánh ngày nhập với ngày hiện tại
                if (date.tm_year > currentDate->tm_year || 
                    (date.tm_year == currentDate->tm_year && date.tm_mon > currentDate->tm_mon) ||
                    (date.tm_year == currentDate->tm_year && date.tm_mon == currentDate->tm_mon && date.tm_mday > currentDate->tm_mday)) {
                    throw std::invalid_argument("Date cannot be in the future.");
                }
            }
            // Nếu cho phép nhập ngày quá khứ
            else if (allowFutureDate == 1) {
                std::time_t currentTime = std::time(nullptr); // Lấy thời gian hiện tại
                std::tm* currentDate = std::localtime(&currentTime); // Chuyển sang std::tm

                // So sánh ngày nhập với ngày hiện tại
                if (date.tm_year < currentDate->tm_year || 
                    (date.tm_year == currentDate->tm_year && date.tm_mon < currentDate->tm_mon) ||
                    (date.tm_year == currentDate->tm_year && date.tm_mon == currentDate->tm_mon && date.tm_mday < currentDate->tm_mday)) {
                    throw std::invalid_argument("Date cannot be in the past.");
                    }

                }




            break; // Thoát vòng lặp nếu không có lỗi
        } catch (const std::invalid_argument& e) {
            std::cout << "Error: " << e.what() << ". Please re-enter the date in the format yyyy-mm-dd.\n";
        }
    }
    return date;
}

void manageTransaction(User* currentUser) 
{
    // Kiểm tra xem người dùng có tài khoản hay không
    if (currentUser->getAccounts().empty()) 
    {
        cout << "You do not have any accounts. Please create an account first to perform a transaction.\n";
        return;
    }

    // Hiển thị các tài khoản người dùng có
    cout << "Choose a transaction account:\n";
    int i = 1;
    for (const auto& acc : currentUser->getAccounts()) {
        cout << i++ << ". " << acc.getAccountName() << " - " << acc.getBalance() << " VND\n";
    }

    int accChoice;
    cout << "Choose an account: ";
    cin >> accChoice;
    if (accChoice < 1 || accChoice > currentUser->getAccounts().size()) {
        cout << "Invalid choice. Please try again.\n";
        return;
    }

    // Lấy tài khoản đã chọn
    Account& selectedAccount = currentUser->getAccounts()[accChoice - 1];
    
    double amount;
    string date, type, category, note;
    
    // Tạo ID giao dịch tự động
    string transID = to_string(currentUser->getTransactions().size() + 1);  
    cout << "Transaction ID: " << transID << endl;

    cout << "Enter amount: ";
    cin >> amount;

    // Lấy ngày giao dịch từ người dùng
    std::tm transDate = getValidDateFromUser("Enter transaction date (yyyy-mm-dd): ", 0);

    cout << "Transaction type (1: Income / 0: Expense): ";
    cin >> type;

    // Hiển thị danh sách category dựa trên loại giao dịch
    if (type == "1") {
        // Nếu là thu, hiển thị danh mục thu
        cout << "Choose category for Income:\n";
        cout << "1. Salary\n";
        cout << "2. Gift\n";
        cout << "3. Bonus\n";
        cout << "4. Other Income\n";
    } else if (type == "0") {
        // Nếu là chi, hiển thị danh mục chi
        cout << "Choose category for Expense:\n";
        cout << "1. Shopping\n";
        cout << "2. Food\n";
        cout << "3. Bills\n";
        cout << "4. Entertainment\n";
        cout << "5. Other Expenses\n";
    } else {
        cout << "Invalid transaction type. Please enter 1 for 'Income' or 0 for 'Expense'.\n";
        return;
    }

    // Nhận số chọn từ người dùng để xác định category
    int categoryChoice;
    cout << "Enter the number corresponding to the category: ";
    cin >> categoryChoice;

    // Gán category dựa trên số người dùng chọn
    if (type == "1") {  // Thu
        switch (categoryChoice) 
        {
            case 1: category = "Salary"; break;
            case 2: category = "Gift"; break;
            case 3: category = "Bonus"; break;
            case 4: category = "Other Income"; break;
            default: 
                cout << "Invalid choice. Defaulting to 'Other Income'.\n";
                category = "Other Income";
                break;
        }
    } else if (type == "0") 
    {  // Chi
        switch (categoryChoice) {
            case 1: category = "Shopping"; break;
            case 2: category = "Food"; break;
            case 3: category = "Bills"; break;
            case 4: category = "Entertainment"; break;
            case 5: category = "Other Expenses"; break;
            default: 
                cout << "Invalid choice. Defaulting to 'Other Expenses'.\n";
                category = "Other Expenses";
                break;
        }
    }

    // Nhập ghi chú giao dịch
    cout << "Note: ";
    cin.ignore();  // Đảm bảo không bỏ qua ký tự '\n' còn lại
    getline(cin, note);

    // Xử lý giao dịch và cập nhật số dư tài khoản
    if (type == "1") {
        // Giao dịch thu, cộng tiền vào tài khoản
        selectedAccount.updateBalance(amount);
        currentUser->updateBalance(amount);
        cout << "Income transaction added successfully. Account balance updated!\n";
    } else if (type == "0") {
        // Giao dịch chi, trừ tiền từ tài khoản
        if (selectedAccount.getBalance() >= amount) {
            selectedAccount.updateBalance(-amount);
            currentUser->updateBalance(-amount);
            cout << "Expense transaction added successfully. Account balance updated!\n";
        } else {
            cout << "Insufficient funds to perform this expense transaction!\n";
            return;
        }
    } else {
        cout << "Invalid transaction type. Please enter 1 for 'Income' or 0 for 'Expense'.\n";
        return;
    }

    // Thêm giao dịch vào lịch sử giao dịch với ID tự động
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
    reportFile << "Tong So Du," << fixed << setprecision(2) << currentUser->getBalance() << " VND\n";

    // Báo cáo tài khoản
    reportFile << "\n--- Danh Sach Tai Khoan ---\n";
    reportFile << "Tai Khoan,So Du\n";  // Tiêu đề cột cho các tài khoản
    for (const auto& acc : currentUser->getAccounts()) {
        reportFile << acc.getAccountName() 
                   << "," << fixed << setprecision(2) << acc.getBalance() << " VND\n";
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
                       << "," << trans.getAmount() << " VND\n";
        } 
        else if (trans.getType() == "0") 
        {  // Khoan chi
            totalExpense += trans.getAmount();
            reportFile << "Expense," << trans.getID() 
                       << "," << trans.getCategory() 
                       << "," << trans.getDate() 
                       << "," << trans.getAmount() << " VND\n";
        }
    }
    reportFile << "\nTong Thu," << totalIncome << " VND\n";
    reportFile << "Tong Chi," << totalExpense << " VND\n";
    reportFile << "Chenh Lech Thu Chi," << totalIncome - totalExpense << " VND\n";

    // Đóng file
    reportFile.close();
    cout << "Bao cao da duoc xuat ra file report.csv\n";
}

int main() 
{
    unordered_map<string, User> users; // Store the list of users
    User* currentUser = nullptr;      // Current logged-in user
    int choice;

    do {
        // If not logged in, show login menu
        if (currentUser == nullptr) 
        {
            showLoginMenu();
            cin >> choice;
            switch (choice) 
            {
                case 1: // Login
                    currentUser = login(users);
                    break;
                case 2: // Register new account
                    registerUser(users);
                    break;
                case 0: // Exit
                    cout << "Exiting the program...\n";
                    return 0;
                default:
                    cout << "Invalid choice. Please try again.\n";
            }
        } 
        else 
        {
            // If logged in, show the main menu
            showMainMenu();
            cin >> choice;

            switch (choice) 
            {
                case 1: 
                { // Manage accounts
                    int accountChoice;
                    cout << "1. Add account\n2. Remove account\n3. Update account balance\n0. Go back\nChoose option: ";
                    cin >> accountChoice;

                    if (accountChoice == 1) 
                    { // Add account
                        if (accountChoice == 1) 
                        { // Add account
                        string accName;
                        double balance;
                        
                        // Tạo ID tự động dựa vào số lượng tài khoản hiện tại
                        string accID = to_string(currentUser->getAccounts().size() + 1);  // Tạo ID tự động từ số lượng tài khoản

                        cout << "Account ID: " << accID << endl;  // Hiển thị ID tự động

                        cout << "Enter account name: ";
                        cin.ignore();  // Đảm bảo không bỏ qua ký tự '\n' còn lại
                        getline(cin, accName);
                        cout << "Enter initial balance: ";
                        cin >> balance;

                        // Add new account with generated ID
                        currentUser->addAccount(Account(accID, accName, balance));
                        cout << "Account has been added successfully!\n";
                    }
                    else if (accountChoice == 2) 
                    { // Remove account
                        string accID;
                        cout << "Enter the account ID to remove: ";
                        cin >> accID;
                        currentUser->removeAccount(accID);
                        cout << "Account has been removed (if it existed).\n";
                    } 
                    else if (accountChoice == 3) { // Update account balance
                        string accID;
                        double newBalance;
                        cout << "Enter the account ID to update: ";
                        cin >> accID;
                        cout << "Enter the new balance: ";
                        cin >> newBalance;
                        currentUser->updateAccount(accID, newBalance);
                        cout << "Account balance has been updated.\n";
                    }
                    break;
                    }
                }
                case 2: // Manage transactions
                {
                    int transChoice;
                    cout << "1. Add transaction\n";
                    cout << "2. View transaction history\n";
                    cout << "3. Update transaction\n";
                    cout << "4. Remove transaction\n";
                    cout << "0. Go back\n";
                    cout << "Choose option: ";
                    cin >> transChoice;

                    switch (transChoice) {
                        case 1: 
                        { // Add transaction
                            manageTransaction(currentUser);
                            break;
                        }
                        case 2: 
                        { // View transaction history
                            currentUser->transHistory();
                            break;
                        }      
                        case 3: 
                        { // Update transaction
                            string transID;
                            double newAmount;
                            string newNote;

                            cout << "Enter transaction ID to update: ";
                            cin >> transID;
                            cout << "Enter the new amount: ";
                            cin >> newAmount;
                            cout << "Enter the new note: ";
                            cin.ignore();  // To clear the newline character
                            getline(cin, newNote);

                            currentUser->updateTrans(transID, newAmount, newNote);
                            break;
                        }
                        case 4: { // Remove transaction
                            string transID;
                            cout << "Enter transaction ID to remove: ";
                            cin >> transID;
                            currentUser->removeTrans(transID);
                            break;
                        }
                        case 0:
                            break;
                        default:
                            cout << "Invalid choice. Please try again.\n";
                            break;
                    }
                    break;
                }
                case 3: // Manage loans
                {
                    int loanChoice;
                    cout << "1. Add loan\n";
                    cout << "2. View loan list\n";
                    cout << "3. Update loan\n";
                    cout << "4. Remove loan\n";
                    cout << "0. Go back\n";
                    cout << "Choose option: ";
                    cin >> loanChoice;

                    switch (loanChoice) 
                    {
                        case 1: 
                        { // Add loan
                            string debtorName;
                            double amount, interestRate;
                            bool status;

                            cout << "Enter lender's name: ";
                            cin.ignore(); // To ensure no space is skipped when entering name
                            getline(cin, debtorName);
                            //Enter loan amount 
                            do {
                                cout << "Enter loan amount: ";
                                cin >> amount;
                                if (amount <= 0) {
                                    cout << "Amount cannot be negative. Please enter a positive value.\n";
                                }
                            } while (amount <= 0);

                            // Enter interest rate
                            do {
                                cout << "Enter interest rate (%): ";
                                cin >> interestRate;
                                if (interestRate < 0) {
                                    cout << "Interest rate cannot be negative. Please enter a positive value.\n";
                                }
                            } while (interestRate < 0);

                            // Use getValidDateFromUser function to input and validate creation and due dates
                            std::tm createDate = getValidDateFromUser("Enter creation date (yyyy-mm-dd): ", 0);
                            std::tm dueDate = getValidDateFromUser("Enter due date (yyyy-mm-dd): ", 1);

                            cout << "Status (0: Unpaid, 1: Paid): ";
                            cin >> status;

                            // Add the loan to the list
                            currentUser->addLoan(Loan(debtorName, amount, interestRate, dueDate, createDate, status));
                            cout << "Loan added successfully!\n";

                            // Update balance when borrowing
                            currentUser->updateBalance(-amount);
                            break;
                        }

                        case 2: 
                        { // View loan list
                            cout << "Loan list:\n";
                            for (const auto& loan : currentUser->getLoans()) 
                            {
                                cout << loan.getLoanDetail() << endl;
                            }
                            break;
                        }
                        case 3: 
                        { 
                            // Update loan
                            string lenderName;
                            double newRate;
                            bool newStatus;

                            cout << "Enter lender's name to update: ";
                            cin.ignore();
                            getline(cin, lenderName);

                            // Find the loan by lender's name
                            bool loanFound = false;
                            for (auto& loan : currentUser->getLoans()) 
                            {
                                if (loan.getLoanDetail().find(lenderName) != string::npos) 
                                {
                                    loanFound = true;

                                    // Input new interest rate
                                    do {
                                        cout << "Enter new interest rate (%): ";
                                        cin >> newRate;
                                        if (newRate < 0) {
                                            cout << "Interest rate cannot be negative. Please enter a positive value.\n";
                                        }
                                    } while (newRate < 0);

                                    // Input new due date using getValidDateFromUser
                                    tm newDueDate = getValidDateFromUser("Enter new due date (yyyy-mm-dd): ", 1);
                                    
                                    // Input new status (0: Unpaid, 1: Paid)
                                    cout << "Enter new status (0: Unpaid, 1: Paid): ";
                                    cin >> newStatus;

                                    // Update loan details
                                    currentUser->updateLoan(lenderName, newRate, newDueDate, newStatus);
                                    cout << "The loan of " << lenderName << " has been successfully updated.\n";
                                    break;  // Exit the loop after successful update
                                }
                            }

                            if (!loanFound) {
                                cout << "No loan found for lender " << lenderName << ".\n";
                            }
                            break;
                        }

                        case 4: 
                        { // Remove loan
                            string lenderName;
                            cout << "Enter lender's name to remove loan: ";
                            cin.ignore();
                            getline(cin, lenderName);
                            currentUser->removeLoan(lenderName);
                            break;
                        }
                        case 0:
                            break;
                        default:
                            cout << "Invalid choice. Please try again.\n";
                            break;
                    }
                    break;
                }
                case 4: // Manage lends
                {
                    int lendChoice;
                    cout << "1. Add lend\n";
                    cout << "2. View lend list\n";
                    cout << "3. Update lend\n";
                    cout << "4. Remove lend\n";
                    cout << "0. Go back\n";
                    cout << "Choose option: ";
                    cin >> lendChoice;

                    switch (lendChoice) 
                    {
                        case 1: 
                        { 
                            // Add lend
                            string debtorName, dueDateStr, createDateStr;
                            double amount, interestRate;
                            bool status;

                            // Enter debtor's name
                            cout << "Enter debtor's name: ";
                            cin.ignore();  // To skip the leftover '\n' character
                            getline(cin, debtorName);

                            // Enter lend amount
                            do {
                                cout << "Enter lend amount: ";
                                cin >> amount;
                                if (amount <= 0) {
                                    cout << "Amount cannot be negative. Please enter a positive value.\n";
                                }
                            } while (amount <= 0);

                            // Enter interest rate
                            do {
                                cout << "Enter interest rate (%): ";
                                cin >> interestRate;
                                if (interestRate < 0) {
                                    cout << "Interest rate cannot be negative. Please enter a positive value.\n";
                                }
                            } while (interestRate < 0);

                            // Enter and validate creation date
                            std::tm createDate = getValidDateFromUser("Enter creation date (yyyy-mm-dd): ", 0);

                            // Enter and validate due date
                            std::tm dueDate = getValidDateFromUser("Enter due date (yyyy-mm-dd): ", 1);
                            status = 0;

                            // Add the lend to the list
                            currentUser->addLend(Lend(debtorName, amount, interestRate, dueDate, createDate, status));
                            cout << "Lend added successfully!\n";

                            // Update balance when lending
                            currentUser->updateBalance(-amount); 
                            break;
                        }

                        case 2: { // View lend list
                            cout << "Lend list:\n";
                            for (const auto& lend : currentUser->getLends()) {
                                cout << lend.getBorrowDetail() << endl;
                            }
                            break;
                        }
                        case 3: 
                        { 
                            // Update lend
                            string debtorName;
                            double newRate;
                            string newDueDateStr;
                            bool newStatus;

                            cout << "Enter debtor's name to update: ";
                            cin.ignore();
                            getline(cin, debtorName);

                            // Find lend by debtor's name
                            bool lendFound = false;
                            for (auto& lend : currentUser->getLends()) 
                            {
                                if (lend.getBorrowDetail().find(debtorName) != string::npos) 
                                {
                                    lendFound = true;

                                    // Input new interest rate
                                    do {
                                        cout << "Enter new interest rate (%): ";
                                        cin >> newRate;
                                        if (newRate < 0) {
                                            cout << "Interest rate cannot be negative. Please enter a positive value.\n";
                                        }
                                    } while (newRate < 0);
                                    // Validate and convert newDueDate
                                    std::tm newDueDate = getValidDateFromUser("Enter new due date (yyyy-mm-dd): ", 1);
                                    // Input new status (0: Unpaid, 1: Paid)
                                    cout << "Enter new status (0: Unpaid, 1: Paid): ";
                                    cin >> newStatus;
                                    // Update lend details
                                    currentUser->updateLend(debtorName, newRate, newDueDate, newStatus);
                                    cout << "Lend of " << debtorName << " updated successfully.\n";
                                    break;  // Exit the loop after successful update
                                }
                            }
                            if (!lendFound) {
                                cout << "No lend found for debtor " << debtorName << ".\n";
                            }
                            break;
                        }

                        case 4: 
                        { // Remove lend
                            string debtorName;
                            cout << "Enter debtor's name to remove lend: ";
                            cin.ignore();
                            getline(cin, debtorName);
                            currentUser->removeLend(debtorName);
                            break;
                        }
                        case 0:
                            break;
                        default:
                            cout << "Invalid choice. Please try again.\n";
                            break;
                    }
                    break;
                }
                case 5: // Financial report
                {
                    cout << "\n--- Financial Report ---\n";
                    cout << "1. Total balance report\n";
                    cout << "2. Account report\n";
                    cout << "3. Loan and lend report\n";
                    cout << "4. Income and expense report\n";
                    cout << "0. Go back\n";
                    do
                    {     
                        cout << "Choose option: ";

                        cin >> choice;
                        switch (choice) 
                        {
                            case 1: 
                            { // Total balance report
                                cout << "Your total balance is: " << currentUser->getBalance() << " VND\n";
                                break;
                            }
                            case 2: { // Account report
                                currentUser->reportAcc();
                                break;
                            }
                            case 3: { // Loan and lend report
                                currentUser->reportLoanAndLend();
                                break;
                            }
                            case 4: 
                            { 
                                // Income and expense report
                                currentUser->reportInAndOut();
                                break;
                            }
                            case 0:
                                break;
                            default:
                                cout << "Invalid choice. Please try again.\n";
                                break;
                        }
                    } while(choice != 0);
                    break;
                }
                case 6: 
                {
                    exportReportToCSV(currentUser);
                    break;    
                }
                case 0: // Log out
                    cout << "Successfully logged out!\n";
                    currentUser = nullptr; // Reset the login status
                    break;
                default:
                    cout << "Invalid choice. Please try again.\n";
            }
        }
    } while (true);

    return 0;
}

