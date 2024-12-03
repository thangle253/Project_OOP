#include <iostream>
#include <unordered_map> // For storing user list
#include "User.h"        // Header file for User class and related classes
#include <fstream>  // For using ofstream
#include <ctime>    // For handling date and time
#include <chrono> // For handling time
#include <cmath>
#include <sstream>
#include <iomanip>

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
    cout << "6. Special features\n";
    cout << "0. Logout\n";
    cout << "====================\n";
    cout << "Choose an option: ";
}

// Convert string to std::tm for date
std::tm convertStringToDate(const std::string& dateStr) 
{
    std::tm date = {};
    std::istringstream ss(dateStr);
    ss >> std::get_time(&date, "%Y-%m-%d");
    if (ss.fail()) {
        throw std::invalid_argument("Invalid date: " + dateStr);
    }
    return date;
}

// Function to get a valid date from the user, with condition for future dates
std::tm getValidDateFromUser(const std::string& prompt, int allowFutureDate) 
{
    std::string dateStr;
    std::tm date;

    while (true)
     {
        std::cout << prompt;
        std::cin >> dateStr;

        try {
            date = convertStringToDate(dateStr); // Check and convert date string

            // If future dates are not allowed
            if (allowFutureDate == 0) {
                std::time_t currentTime = std::time(nullptr); // Get current time
                std::tm* currentDate = std::localtime(&currentTime); // Convert to std::tm

                // Compare input date with current date
                if (date.tm_year > currentDate->tm_year || 
                    (date.tm_year == currentDate->tm_year && date.tm_mon > currentDate->tm_mon) ||
                    (date.tm_year == currentDate->tm_year && date.tm_mon == currentDate->tm_mon && date.tm_mday > currentDate->tm_mday)) {
                    throw std::invalid_argument("Date cannot be in the future.");
                }
            }

            break;  // Exit loop if no errors
        } catch (const std::invalid_argument& e) {
            std::cout << "Error: " << e.what() << ". Please re-enter the date in the format yyyy-mm-dd.\n";
        }
    }

    return date;
}

void manageTransaction(User* currentUser) 
{
    // Check if user has any account
    if (currentUser->getAccounts().empty()) 
    {
        cout << "You do not have any accounts. Please create an account first to perform a transaction.\n";
        return;
    }

    // Show user accounts
    cout << "Choose a transaction account:\n";
    int i = 1;
    for (const auto& acc : currentUser->getAccounts())
    {
        cout << i++ << ". " << acc.getAccountName() << " - " << acc.getBalance() << " VND\n";
    }

    int accChoice;
    cout << "Choose an account: ";
    cin >> accChoice;
    if (accChoice < 1 || accChoice > currentUser->getAccounts().size()) {
        cout << "Invalid choice. Please try again.\n";
        return;
    }
    // Get the selected account
    Account& selectedAccount = currentUser->getAccounts()[accChoice - 1];
    
    double amount;
    string date, type, category, note;
    
    // Get transaction details
    // Sử dụng to_string(currentUser->getTransactions().size() + 1) để tạo ID giao dịch tự động
    string transID = to_string(currentUser->getTransactions().size() + 1);
    cout << "Transaction ID: " << transID << endl; // Automatically generated ID

    cout << "Enter amount: ";
    cin >> amount;

    // Get transaction date using getValidDateFromUser to ensure valid date
    std::tm transDate = getValidDateFromUser("Enter transaction date (yyyy-mm-dd): ", 0);

    cout << "Transaction type (1: Income / 0: Expense): ";
    cin >> type;
    cout << "Category: ";
    cin.ignore();  // To clear the newline character in the buffer
    getline(cin, category);
    cout << "Note: ";
    getline(cin, note);

    // Process the transaction based on type and update account balance
    if (type == "1") {
        // Income transaction, add money to account
        selectedAccount.updateBalance(amount);
        currentUser->updateBalance(amount);
        cout << "Income transaction added successfully. Account balance updated!\n";
    } else if (type == "0") {
        // Expense transaction, subtract money from account
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

    // Add the transaction to the transaction history with the automatically generated ID
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
                            
                            cout << "Enter loan amount: ";
                            cin >> amount;

                            cout << "Enter interest rate (%): ";
                            cin >> interestRate;

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
                                    cout << "Enter new interest rate (%): ";
                                    cin >> newRate;

                                    // Input new due date using getValidDateFromUser
                                    tm newDueDate = getValidDateFromUser("Enter new due date (yyyy-mm-dd): ", 1);
                                    
                                    // Input new status (0: Unpaid, 1: Paid)
                                    cout << "Enter new status (0: Unpaid, 1: Paid): ";
                                    cin >> newStatus;

                                    // Update loan details
                                    currentUser->updateLoan(lenderName, newRate, newDueDate, newStatus);
                                    cout << "Loan of " << lenderName << " updated successfully.\n";
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
                            cout << "Enter lend amount: ";
                            cin >> amount;

                            // Enter interest rate
                            cout << "Enter interest rate (%): ";
                            cin >> interestRate;

                            // Enter and validate creation date
                            std::tm createDate = getValidDateFromUser("Enter creation date (yyyy-mm-dd): ", 0);

                            // Enter and validate due date
                            std::tm dueDate = getValidDateFromUser("Enter due date (yyyy-mm-dd): ", 1);

                            // Enter lend status
                            cout << "Status (0: Unpaid, 1: Paid): ";
                            cin >> status;

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
                                    cout << "Enter new interest rate (%): ";
                                    cin >> newRate;

                                    // Input new due date and validate
                                    cout << "Enter new due date (yyyy-mm-dd): ";
                                    cin >> newDueDateStr;
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
                            cout << "Lend has been removed (if it existed).\n";
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
                    cout << "1. Export report to CSV file\n";
                    cout << "0. Go back\n";
                    cout << "Choose option: ";
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
                            cout << "Invalid choice. Please try again.\n";
                    }
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

