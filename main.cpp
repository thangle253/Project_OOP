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

void Add_Transaction(User* currentUser) 
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
    do {
        cout << "Enter amount: ";
        cin >> amount;

        if (amount <= 0) 
            cout << "Amount cannot be negative. Please enter a positive value.\n";
        else if(amount > selectedAccount.getBalance())
            cout << "Amount cannot be greater than your balance. Please enter a smaller value.\n";
        else
            break;
    } while (true);

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
    string filename;
    filename = "user/" + currentUser->getID() + ".csv";
    ofstream reportFile(filename);

    if (!reportFile) {
        cout << "Cannot open file for writing!\n";
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
    cout << "The report has been exported to a file \""<<filename<<"\"";
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
                    cout << "1. Add account\n2. Remove account\n3. Update account balance\n4. Banking\n0. Go back\nChoose option: ";
                    cin >> accountChoice;

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
                        do {
                            cout << "Enter the initial balance: ";
                            cin >> balance;
                            if (balance <= 0)
                                    cout << "Initial balance cannot be negative. Please enter a positive value.\n";
                        } while (balance <= 0);
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
                    else if (accountChoice == 3) 
                    { // Update account balance
                        string accID;
                        double newBalance;
                        cout << "Enter the account ID to update: ";
                        cin >> accID;
                        do {
                                cout << "Enter the new balance: ";
                                cin >> newBalance;
                                if (newBalance <= 0)
                                    cout << "Newbalance cannot be negative. Please enter a positive value.\n";
                            } while (newBalance <= 0);
                        currentUser->updateAccount(accID, newBalance);
                        
                    }
                    else if (accountChoice == 4) // Banking (Chuyển tiền)
                    { 
                        string fromAccID, toAccID;
                        double amount;
                        // Hiển thị danh sách tài khoản người dùng có
                        if (currentUser->getAccounts().empty()) 
                        {
                            cout << "No accounts available.\n";
                        } 
                        else 
                        {
                            cout << "List of Accounts:\n";
                            for (size_t i = 0; i < currentUser->getAccounts().size(); ++i) {
                                cout << i + 1 << ". Account ID: " << currentUser->getAccounts()[i].getAccountID() << "\n";
                                cout << "   Account Name: " << currentUser->getAccounts()[i].getAccountName() << "\n";
                                cout << "   Balance: " << currentUser->getAccounts()[i].getBalance() << " VND\n";
                                cout << "-----------------------------\n";
                            }

                            cout << "Enter the Account ID to transfer from: ";
                            cin >> fromAccID;
                            cout << "Enter the Account ID to transfer to: ";
                            cin >> toAccID;
                            cout << "Enter the amount to transfer: ";
                            cin >> amount;
                            // Gọi hàm banking để thực hiện chuyển tiền
                            currentUser->banking(fromAccID, toAccID, amount);
                        }
                    }
                    else if (accountChoice == 0) 
                    { // Go back
                        break;
                    }
                    else 
                    {
                        cout << "Invalid choice. Please try again.\n";
                    }
                    break;
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
                            Add_Transaction(currentUser);
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
                        {
                            // Add loan
                            string lenderName;
                            double amount, interestRate;
                            bool status = 0;  // Loan is initially unpaid
                            
                            // Display list of accounts
                            if (currentUser->getAccounts().empty()) 
                            {
                                cout << "No accounts available.\n";
                                break;
                            }
                            else 
                            {
                                cout << "List of Accounts:\n";
                                for (size_t i = 0; i < currentUser->getAccounts().size(); ++i) 
                                {
                                    cout << i + 1 << ". Account ID: " << currentUser->getAccounts()[i].getAccountID() << "\n";
                                    cout << "   Account Name: " << currentUser->getAccounts()[i].getAccountName() << "\n";
                                    cout << "   Balance: " << currentUser->getAccounts()[i].getBalance() << " VND\n";
                                    cout << "-----------------------------\n";
                                }

                                int accChoice;
                                // Choose an account to borrow from
                                cout << "Choose an account to borrow from: ";
                                cin >> accChoice;

                                // Validate account choice
                                if (accChoice < 1 || accChoice > currentUser->getAccounts().size()) 
                                {
                                    cout << "Invalid choice. Please try again.\n";
                                    break;
                                }

                                // Get the selected account
                                Account& selectedAccount = currentUser->getAccounts()[accChoice - 1];

                                // Enter lender's name
                                cout << "Enter lender's name: ";
                                cin.ignore();  // To skip the leftover '\n' character
                                getline(cin, lenderName);

                                // Enter loan amount
                                do {
                                    cout << "Enter loan amount: ";
                                    cin >> amount;

                                    // Check if amount is valid
                                    if (amount <= 0) 
                                        cout << "Amount cannot be negative. Please enter a positive value.\n";
                                    else
                                        break;
                                } while (true);

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

                                // Add the loan to the list
                                currentUser->addLoan(Loan(lenderName, amount, interestRate, dueDate, createDate, status));
                                cout << "Loan added successfully!\n";

                                // Update balance when borrowing (add amount to the user's total balance)
                                currentUser->updateBalance(amount);
                                selectedAccount.updateBalance(amount); // Update balance of the selected account
                                cout << "Amount added to your account.\n";
                                break;
                            }
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
                            tm newDueDate; // Move the variable outside of the switch statement

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

                                    // Ask the user which field they want to update
                                    cout << "What would you like to update?\n";
                                    cout << "1. Interest rate\n";
                                    cout << "2. Due date\n";
                                    cout << "3. Loan status\n";
                                    cout << "Enter the number of the field to update: ";
                                    int choice;
                                    cin >> choice;

                                    switch (choice) 
                                    {
                                        case 1: 
                                            // Input new interest rate
                                            do {
                                                cout << "Enter new interest rate (%): ";
                                                cin >> newRate;
                                                if (newRate < 0)
                                                    cout << "Interest rate cannot be negative. Please enter a positive value.\n";
                                                else if (newRate > 100)
                                                    cout << "Interest rate cannot be greater than 100%. Please enter a smaller value.\n";
                                            } while (newRate < 0 || newRate > 100);

                                            currentUser->updateLoan(lenderName, newRate, loan.getDueDate(), loan.getStatus());
                                            cout << "Interest rate updated successfully.\n";
                                            break;

                                        case 2: 
                                            // Input new due date using getValidDateFromUser
                                            newDueDate = getValidDateFromUser("Enter new due date (yyyy-mm-dd): ", 1); // Allow future date
                                            currentUser->updateLoan(lenderName, loan.getInterestRate(), newDueDate, loan.getStatus());
                                            cout << "Due date updated successfully.\n";
                                            break;

                                        case 3: 
                                        {
                                            // Input new status (0: Unpaid, 1: Paid)
                                            cout << "Enter new status (0: Unpaid, 1: Paid): ";
                                            cin >> newStatus;

                                            // Hiển thị danh sách tài khoản của người dùng để chọn tài khoản trừ tiền
                                            cout << "Choose an account to deduct money from for the loan:\n";
                                            int accChoice;
                                            for (size_t i = 0; i < currentUser->getAccounts().size(); ++i) {
                                                cout << i + 1 << ". Account ID: " << currentUser->getAccounts()[i].getAccountID() << " - "
                                                    << currentUser->getAccounts()[i].getAccountName() << " - Balance: "
                                                    << currentUser->getAccounts()[i].getBalance() << " VND\n";
                                            }

                                            // Người dùng chọn tài khoản
                                            cin >> accChoice;
                                            if (accChoice < 1 || accChoice > currentUser->getAccounts().size()) {
                                                cout << "Invalid choice. Please try again.\n";
                                                break;
                                            }

                                            // Lấy tài khoản đã chọn
                                            Account& selectedAccount = currentUser->getAccounts()[accChoice - 1];

                                            if (newStatus != loan.getStatus()) 
                                            {
                                                // Nếu trạng thái là "Paid", trừ tiền từ tài khoản
                                                if (newStatus == 1) 
                                                {
                                                    double loanAmount = loan.calculateCompoundInterest(); // Lấy số tiền khoản cho vay + lãi
                                                    // Kiểm tra xem tài khoản có đủ tiền không
                                                    if (selectedAccount.getBalance() < loanAmount) 
                                                    {
                                                        cout << "Insufficient funds in the selected account.\n";
                                                        break;
                                                    }
                                                    selectedAccount.updateBalance(-loanAmount); // Trừ tiền từ tài khoản
                                                    currentUser->updateBalance(-loanAmount); // Cập nhật số dư người dùng
                                                    cout << "Loan status updated to 'Paid'. Amount of " << loanAmount << " VND deducted from the selected account.\n";
                                                    loan.setStatus(newStatus); // Cập nhật trạng thái khoản cho vay
                                                } 
                                                else 
                                                {
                                                    double loanAmount = loan.getAmount(); // Lấy số tiền khoản cho vay
                                                    // Kiểm tra xem tài khoản có đủ tiền không
                                                    if (selectedAccount.getBalance() < loanAmount) 
                                                    {
                                                        cout << "Insufficient funds in the selected account.\n";
                                                        break;
                                                    }
                                                    selectedAccount.updateBalance(loanAmount); // Trừ tiền từ tài khoản
                                                    currentUser->updateBalance(loanAmount); // Cập nhật số dư người dùng
                                                    cout << "Loan status updated to 'Unpaid'. Amount of " << loanAmount << " VND received from the selected account.\n";
                                                    loan.setStatus(newStatus); // Cập nhật trạng thái khoản cho vay
                                                }

                                                // Cập nhật trạng thái khoản vay
                                                currentUser->updateLoan(lenderName, loan.getInterestRate(), loan.getDueDate(), newStatus);
                                            }
                                            break; // Exit the loop after successful update
                                        }

                                        default:
                                            cout << "Invalid choice. No updates were made.\n";
                                            break;
                                    }
                                    break;  // Exit the loop after successful update
                                }
                            }

                            if (!loanFound) {
                                cout << "No loan found for lender " << lenderName << ".\n";
                            }
                            break;
                        }

                        case 4: 
                        { 
                            // Remove loan
                            string lenderName;
                            cout << "Enter lender's name to remove loan: ";
                            cin.ignore();
                            getline(cin, lenderName);

                            // Tìm kiếm khoản vay theo tên người cho vay
                            bool loanFound = false;
                            for (auto& loan : currentUser->getLoans()) 
                            {
                                if (loan.getLoanDetail().find(lenderName) != string::npos) 
                                {
                                    loanFound = true;

                                    // Nếu khoản vay có trạng thái "chưa trả" (status = 0), cộng lại tiền vào tài khoản
                                    double loanAmount = loan.getAmount();
                                    Account& defaultAccount = currentUser->getAccounts()[0]; // Giả sử tài khoản đầu tiên là tài khoản mặc định

                                    // Nếu khoản vay chưa được trả, cộng số tiền vào tài khoản
                                    if (loan.getStatus() == 0) 
                                    {
                                        defaultAccount.updateBalance(-loanAmount); // Trừ tiền vào tài khoản đầu tiên
                                        cout << "Amount " << loanAmount << " VND has been added back to your account.\n";
                                    }
                                    // Xóa khoản vay khỏi danh sách
                                    currentUser->removeLoan(lenderName);
                                    cout << "Loan from " << lenderName << " has been removed.\n";
                                    break;
                                }
                            }

                            if (!loanFound) 
                            {
                                cout << "No loan found for lender " << lenderName << ".\n";
                            }

                            break;
                        }

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

                            // Display list of accounts
                            if (currentUser->getAccounts().empty()) 
                            {
                                cout << "No accounts available.\n";
                                break;
                            } 
                            else 
                            {
                                cout << "List of Accounts:\n";
                                for (size_t i = 0; i < currentUser->getAccounts().size(); ++i) 
                                {
                                    cout << i + 1 << ". Account ID: " << currentUser->getAccounts()[i].getAccountID() << "\n";
                                    cout << "   Account Name: " << currentUser->getAccounts()[i].getAccountName() << "\n";
                                    cout << "   Balance: " << currentUser->getAccounts()[i].getBalance() << " VND\n";
                                    cout << "-----------------------------\n";
                                }

                                int accChoice;
                                // Chọn tài khoản để thực hiện vay
                                cout << "Choose an account to lend from: ";
                                cin >> accChoice;

                                // Kiểm tra lựa chọn tài khoản hợp lệ
                                if (accChoice < 1 || accChoice > currentUser->getAccounts().size()) 
                                {
                                    cout << "Invalid choice. Please try again.\n";
                                    break;
                                }

                                // Lấy tài khoản đã chọn
                                Account& selectedAccount = currentUser->getAccounts()[accChoice - 1];

                                // Nhập tên người vay
                                cout << "Enter debtor's name: ";
                                cin.ignore();  // Để bỏ qua ký tự '\n' còn lại
                                getline(cin, debtorName);

                                // Nhập số tiền vay
                                do {
                                    cout << "Enter lend amount: ";
                                    cin >> amount;

                                    // Kiểm tra số tiền vay hợp lệ
                                    if (amount <= 0) 
                                        cout << "Amount cannot be negative. Please enter a positive value.\n";
                                    else if (amount > selectedAccount.getBalance()) // Kiểm tra số dư tài khoản
                                        cout << "Amount cannot be greater than your account balance. Please enter a smaller value.\n";
                                    else
                                        break;
                                } while (true);

                                // Nhập lãi suất
                                do {
                                    cout << "Enter interest rate (%): ";
                                    cin >> interestRate;
                                    if (interestRate < 0) {
                                        cout << "Interest rate cannot be negative. Please enter a positive value.\n";
                                    }
                                } while (interestRate < 0);

                                // Nhập ngày tạo khoản vay và ngày đáo hạn
                                std::tm createDate = getValidDateFromUser("Enter creation date (yyyy-mm-dd): ", 0);
                                std::tm dueDate = getValidDateFromUser("Enter due date (yyyy-mm-dd): ", 1);

                                status = 0;  // Khoản vay ban đầu là chưa trả

                                // Thêm khoản vay vào danh sách
                                currentUser->addLend(Lend(debtorName, amount, interestRate, dueDate, createDate, status));
                                cout << "Lend added successfully!\n";
                                // Cập nhật số dư tài khoản sau khi cho vay
                                selectedAccount.updateBalance(-amount);
                                currentUser->updateBalance(-amount);
                                cout << "Amount deducted from the selected account.\n";
                                break;
                            }
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
                            bool newStatus;
                            std::tm newDueDate; // Move the variable outside of the switch

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

                                    // Ask the user which field they want to update
                                    cout << "What would you like to update?\n";
                                    cout << "1. Interest rate\n";
                                    cout << "2. Due date\n";
                                    cout << "3. Lend status\n";                             
                                    cout << "Enter the number of the field to update: ";
                                    int choice;
                                    cin >> choice;

                                    switch (choice) 
                                    {
                                        case 1: 
                                            // Input new interest rate
                                            do {
                                                cout << "Enter new interest rate (%): ";
                                                cin >> newRate;
                                                if (newRate < 0) {
                                                    cout << "Interest rate cannot be negative. Please enter a positive value.\n";
                                                } else if (newRate > 100) {
                                                    cout << "Interest rate cannot be greater than 100%. Please enter a smaller value.\n";
                                                } else {
                                                    break;
                                                }
                                            } while (true);

                                            currentUser->updateLend(lend.getDebtorName(), newRate, lend.getDueDate(), lend.getStatus()); // Update interest rate only
                                            cout << "Interest rate updated successfully.\n";
                                            break;

                                        case 2: 
                                            // Input new due date using getValidDateFromUser
                                            newDueDate = getValidDateFromUser("Enter new due date (yyyy-mm-dd): ", 1); // Allow future date
                                            currentUser->updateLend(lend.getDebtorName(), lend.getInterestRate(), newDueDate, lend.getStatus()); // Update due date only
                                            cout << "Due date updated successfully.\n";
                                            break;

                                        case 3: 
                                        {
                                            // Input new status (0: Unpaid, 1: Paid)
                                            cout << "Enter new status (0: Unpaid, 1: Paid): ";
                                            cin >> newStatus;

                                            // Kiểm tra trạng thái hợp lệ
                                            if (newStatus != 0 && newStatus != 1) 
                                            {
                                                cout << "Invalid status entered. Please enter 0 for Unpaid or 1 for Paid.\n";
                                                break; // Không cập nhật nếu nhập sai
                                            }

                                            // Hiển thị danh sách tài khoản của người dùng để chọn tài khoản trừ tiền
                                            cout << "Choose an account to deduct money from for the loan:\n";
                                            int accChoice;
                                            for (size_t i = 0; i < currentUser->getAccounts().size(); ++i) {
                                                cout << i + 1 << ". Account ID: " << currentUser->getAccounts()[i].getAccountID() << " - "
                                                    << currentUser->getAccounts()[i].getAccountName() << " - Balance: "
                                                    << currentUser->getAccounts()[i].getBalance() << " VND\n";
                                            }

                                            // Người dùng chọn tài khoản
                                            cin >> accChoice;
                                            if (accChoice < 1 || accChoice > currentUser->getAccounts().size()) {
                                                cout << "Invalid choice. Please try again.\n";
                                                break;
                                            }

                                            // Lấy tài khoản đã chọn
                                            Account& selectedAccount = currentUser->getAccounts()[accChoice - 1];

                                            if (newStatus != lend.getStatus()) 
                                            {
                                                // Nếu trạng thái là "Paid", trừ tiền từ tài khoản
                                                if (newStatus == 1) 
                                                {
                                                    double lendAmount = lend.calculateCompoundInterest(); // Lấy số tiền khoản cho vay + lãi
                                                    // Kiểm tra xem tài khoản có đủ tiền không
                                                        selectedAccount.updateBalance(lendAmount); // Cộng số tiền từ tài khoản
                                                        currentUser->updateBalance(lendAmount); // Cập nhật số dư người dùng
                                                        cout << "Loan status updated to 'Paid'. Amount of " << lendAmount << " VND deducted from the selected account.\n";
                                                        lend.setStatus(newStatus); // Cập nhật trạng thái khoản cho vay
                                                } 
                                                else 
                                                {
                                                    double lendAmount = lend.getAmount(); // Lấy số tiền khoản cho vay
                                                    // Kiểm tra xem tài khoản có đủ tiền không                                              
                                                    selectedAccount.updateBalance(-lendAmount); // Trừ số tiền từ tài khoản
                                                    currentUser->updateBalance(-lendAmount); // Cập nhật số dư người dùng

                                                    cout << "Loan status updated to 'UnPaid'. Amount of " << lendAmount << " VND received from the selected account.\n";
                                                    lend.setStatus(newStatus); // Cập nhật trạng thái khoản cho vay


                                                }
                                                // Cập nhật trạng thái khoản cho vay
                                                currentUser->updateLend(lend.getDebtorName(), lend.getInterestRate(), lend.getDueDate(), newStatus);
                                            }                                  
                                            break; // Exit the loop after successful update
                                        }

                                        default:
                                            cout << "Invalid choice. No updates were made.\n";
                                            break;
                                    }
                                    break;  // Exit the loop after successful update
                                }
                            }

                            if (!lendFound) {
                                cout << "No lend found for debtor " << debtorName << ".\n";
                            }
                            break;
                        }

                        case 4:
                        {
                            // Remove lend
                            string debtorName;
                            cout << "Enter debtor's name to remove lend: ";
                            cin.ignore();  // To skip the leftover '\n' character
                            getline(cin, debtorName);

                            // Tìm kiếm khoản vay theo tên người vay
                            bool lendFound = false;
                            for (auto& lend : currentUser->getLends()) 
                            {
                                if (lend.getBorrowDetail().find(debtorName) != string::npos)  // Tìm kiếm theo tên người vay
                                {
                                    lendFound = true;
                                    // Lấy số tiền từ khoản vay và cộng lại vào tài khoản
                                    double lendAmount = lend.getAmount();
                                    Account& defaultAccount = currentUser->getAccounts()[0]; // Giả sử tài khoản đầu tiên là tài khoản mặc định
                                    //Nếu mà status = 0 thì cộng tiền vào tài khoản
                                    if (lend.getStatus() == 0) 
                                    {
                                        defaultAccount.updateBalance(lendAmount); // Cộng tiền vào tài khoản đầu tiên
                                        cout << "Amount " << lendAmount << " VND has been added back to your account.\n";
                                    }
                                    // Xóa khoản cho vay khỏi danh sách
                                    currentUser->removeLend(debtorName);
                                    cout << "Lend to " << debtorName << " has been removed.\n";
                                    break;
                                }
                            }

                            if (!lendFound)  // Nếu không tìm thấy khoản cho vay
                            {
                                cout << "No lend found for debtor " << debtorName << ".\n";
                            }
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

