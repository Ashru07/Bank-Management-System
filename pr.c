#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ACCOUNTS 10000
#define MAX_USERS 10000
#define ID_EDIT_NAME 101
#define ID_EDIT_ACCOUNT 102
#define ID_EDIT_BALANCE 103
#define ID_BUTTON_CREATE 104
#define ID_BUTTON_DEPOSIT 105
#define ID_BUTTON_WITHDRAW 106
#define ID_BUTTON_DISPLAY 107
#define ID_LIST_ACCOUNTS 108
#define ID_BUTTON_TRANSFER 109
#define ID_EDIT_TARGET_ACCOUNT 110
#define ID_EDIT_USERNAME 111
#define ID_EDIT_PASSWORD 112
#define ID_BUTTON_LOGIN 113
#define ID_BUTTON_LOGOUT 114
#define ID_BUTTON_REGISTER 115
#define ID_EDIT_CONFIRM_PASSWORD 116
#define ID_BUTTON_CREATE_USER 117
#define ID_BUTTON_BACK_TO_LOGIN 118
#define ID_BUTTON_DELETE 119
#define ID_BUTTON_PROFIT 120
#define USER_DATA_FILE "users.txt"
#define ACCOUNT_DATA_FILE "accounts.txt"

typedef struct
{
    char name[100];
    int accountNumber;
    double balance;
} BankAccount;

typedef struct
{
    char username[50];
    char password[50];
    int isAdmin;
    int accountNumber;
} User;

BankAccount accounts[MAX_ACCOUNTS];
User users[MAX_USERS];
int accountCount = 0;
int userCount = 0;
int loggedIn = 0;
int currentUserIsAdmin = 0;
int currentUserAccountNumber = -1;

void SaveDataToFile()
{
    FILE *fUsers = fopen(USER_DATA_FILE, "w");
    FILE *fAccounts = fopen(ACCOUNT_DATA_FILE, "w");

    if (fUsers)
    {
        for (int i = 0; i < userCount; i++)
        {
            fprintf(fUsers, "%s,%s,%d,%d\n",
                    users[i].username,
                    users[i].password,
                    users[i].isAdmin,
                    users[i].accountNumber);
        }
        fclose(fUsers);
    }

    if (fAccounts)
    {
        for (int i = 0; i < accountCount; i++)
        {
            fprintf(fAccounts, "%s,%d,%.2lf,%.2lf\n",
                    accounts[i].name,
                    accounts[i].accountNumber,
                    accounts[i].balance);
        }
        fclose(fAccounts);
    }
}

void LoadDataFromFile()
{
    FILE *fUsers = fopen(USER_DATA_FILE, "r");
    FILE *fAccounts = fopen(ACCOUNT_DATA_FILE, "r");
    char line[256];

    if (fUsers)
    {
        while (fgets(line, sizeof(line), fUsers))
        {
            char *token = strtok(line, ",");
            if (token)
            {
                strcpy(users[userCount].username, token);

                token = strtok(NULL, ",");
                if (token)
                    strcpy(users[userCount].password, token);

                token = strtok(NULL, ",");
                if (token)
                    users[userCount].isAdmin = atoi(token);

                token = strtok(NULL, ",\n");
                if (token)
                    users[userCount].accountNumber = atoi(token);

                userCount++;
            }
        }
        fclose(fUsers);
    }

    if (fAccounts)
    {
        while (fgets(line, sizeof(line), fAccounts))
        {
            char *token = strtok(line, ",");
            if (token)
            {
                strcpy(accounts[accountCount].name, token);

                token = strtok(NULL, ",");
                if (token)
                    accounts[accountCount].accountNumber = atoi(token);

                token = strtok(NULL, ",");
                if (token)
                    accounts[accountCount].balance = atof(token);

                accountCount++;
            }
        }
        fclose(fAccounts);
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void InitializeUsers();
void ShowLoginControls(HWND hwnd);
void ShowBankingControls(HWND hwnd);
void ShowRegistrationControls(HWND hwnd);
void HideAllControls(HWND hwnd);
void AddAccountToList(HWND hList, BankAccount account);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    const char CLASS_NAME[] = "BankingSystemClass";

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "Banking Management System",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 700, 550,
        NULL,
        NULL,
        hInstance,
        NULL);

    if (hwnd == NULL)
    {
        return 0;
    }

    InitializeUsers();
    ShowWindow(hwnd, nCmdShow);

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

void InitializeUsers()
{
    LoadDataFromFile();
    if (userCount == 0)
    {
        strcpy(users[0].username, "admin");
        strcpy(users[0].password, "admin123");
        users[0].isAdmin = 1;
        users[0].accountNumber = -1;
        userCount = 1;
        SaveDataToFile();
    }
}

void AddAccountToList(HWND hList, BankAccount account)
{
    char buffer[256];

    double profit = account.balance + (account.balance * 0.10);

    sprintf(buffer, "Account: %d | Name: %s | Balance: %.2f tk | Balance(10%% per year): %.2f tk",
            account.accountNumber, account.name, account.balance, profit);

    SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)buffer);
}


void HideAllControls(HWND hwnd)
{
    HWND child = GetWindow(hwnd, GW_CHILD);
    while (child)
    {
        DestroyWindow(child);
        child = GetWindow(hwnd, GW_CHILD);
    }
}

void ShowLoginControls(HWND hwnd)
{
    HideAllControls(hwnd);

    CreateWindow("STATIC", "Username:", WS_VISIBLE | WS_CHILD,
                 50, 50, 100, 20, hwnd, NULL, NULL, NULL);
    CreateWindow("STATIC", "Password:", WS_VISIBLE | WS_CHILD,
                 50, 80, 100, 20, hwnd, NULL, NULL, NULL);

    CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                 160, 50, 200, 20, hwnd, (HMENU)ID_EDIT_USERNAME, NULL, NULL);
    CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD,
                 160, 80, 200, 20, hwnd, (HMENU)ID_EDIT_PASSWORD, NULL, NULL);

    CreateWindow("BUTTON", "Login", WS_VISIBLE | WS_CHILD,
                 160, 120, 100, 30, hwnd, (HMENU)ID_BUTTON_LOGIN, NULL, NULL);
    CreateWindow("BUTTON", "Register", WS_VISIBLE | WS_CHILD,
                 270, 120, 100, 30, hwnd, (HMENU)ID_BUTTON_REGISTER, NULL, NULL);
}

void ShowRegistrationControls(HWND hwnd)
{
    HideAllControls(hwnd);

    CreateWindow("STATIC", "Username:", WS_VISIBLE | WS_CHILD,
                 50, 30, 180, 20, hwnd, NULL, NULL, NULL);
    CreateWindow("STATIC", "Password:", WS_VISIBLE | WS_CHILD,
                 50, 60, 180, 20, hwnd, NULL, NULL, NULL);
    CreateWindow("STATIC", "Confirm Password:", WS_VISIBLE | WS_CHILD,
                 50, 90, 180, 20, hwnd, NULL, NULL, NULL);
    CreateWindow("STATIC", "Full Name:", WS_VISIBLE | WS_CHILD,
                 50, 120, 180, 20, hwnd, NULL, NULL, NULL);
    CreateWindow("STATIC", "Initial Deposit:", WS_VISIBLE | WS_CHILD,
                 50, 150, 180, 20, hwnd, NULL, NULL, NULL);

    CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                 180, 30, 200, 20, hwnd, (HMENU)ID_EDIT_USERNAME, NULL, NULL);
    CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD,
                 180, 60, 200, 20, hwnd, (HMENU)ID_EDIT_PASSWORD, NULL, NULL);
    CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD,
                 180, 90, 200, 20, hwnd, (HMENU)ID_EDIT_CONFIRM_PASSWORD, NULL, NULL);
    CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                 180, 120, 200, 20, hwnd, (HMENU)ID_EDIT_NAME, NULL, NULL);
    CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                 180, 150, 200, 20, hwnd, (HMENU)ID_EDIT_BALANCE, NULL, NULL);

    CreateWindow("BUTTON", "Create Account", WS_VISIBLE | WS_CHILD,
                 180, 190, 120, 30, hwnd, (HMENU)ID_BUTTON_CREATE_USER, NULL, NULL);
    CreateWindow("BUTTON", "Back to Login", WS_VISIBLE | WS_CHILD,
                 310, 190, 120, 30, hwnd, (HMENU)ID_BUTTON_BACK_TO_LOGIN, NULL, NULL);
}

void ShowBankingControls(HWND hwnd)
{
    HideAllControls(hwnd);

    CreateWindow("STATIC", "Customer Name:", WS_VISIBLE | WS_CHILD,
                 20, 20, 170, 20, hwnd, NULL, NULL, NULL);
    CreateWindow("STATIC", "Account Number:", WS_VISIBLE | WS_CHILD,
                 20, 50, 170, 20, hwnd, NULL, NULL, NULL);
    CreateWindow("STATIC", "Amount:", WS_VISIBLE | WS_CHILD,
                 20, 80, 170, 20, hwnd, NULL, NULL, NULL);
    CreateWindow("STATIC", "Target Account:", WS_VISIBLE | WS_CHILD,
                 20, 110, 170, 20, hwnd, NULL, NULL, NULL);

    CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                 130, 20, 200, 20, hwnd, (HMENU)ID_EDIT_NAME, NULL, NULL);
    CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                 130, 50, 200, 20, hwnd, (HMENU)ID_EDIT_ACCOUNT, NULL, NULL);
    CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                 130, 80, 200, 20, hwnd, (HMENU)ID_EDIT_BALANCE, NULL, NULL);
    CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                 130, 110, 200, 20, hwnd, (HMENU)ID_EDIT_TARGET_ACCOUNT, NULL, NULL);

    if (currentUserIsAdmin)
    {
        CreateWindow("BUTTON", "Create Bank Account", WS_VISIBLE | WS_CHILD,
                     20, 150, 150, 30, hwnd, (HMENU)ID_BUTTON_CREATE, NULL, NULL);
         CreateWindow("BUTTON", "Delete Account", WS_VISIBLE | WS_CHILD,
                     20, 200, 150, 30, hwnd, (HMENU)ID_BUTTON_DELETE, NULL, NULL);
    }
    CreateWindow("BUTTON", "Deposit", WS_VISIBLE | WS_CHILD,
                 180, 150, 80, 30, hwnd, (HMENU)ID_BUTTON_DEPOSIT, NULL, NULL);
    CreateWindow("BUTTON", "Withdraw", WS_VISIBLE | WS_CHILD,
                 270, 150, 80, 30, hwnd, (HMENU)ID_BUTTON_WITHDRAW, NULL, NULL);
    CreateWindow("BUTTON", "Transfer", WS_VISIBLE | WS_CHILD,
                 360, 150, 80, 30, hwnd, (HMENU)ID_BUTTON_TRANSFER, NULL, NULL);
    CreateWindow("BUTTON", "Display All", WS_VISIBLE | WS_CHILD,
                 450, 150, 100, 30, hwnd, (HMENU)ID_BUTTON_DISPLAY, NULL, NULL);
    CreateWindow("BUTTON", "Logout", WS_VISIBLE | WS_CHILD,
                 560, 150, 100, 30, hwnd, (HMENU)ID_BUTTON_LOGOUT, NULL, NULL);


    CreateWindow("LISTBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | LBS_NOTIFY | WS_VSCROLL,
                 20, 250, 640, 180, hwnd, (HMENU)ID_LIST_ACCOUNTS, NULL, NULL);

    if (!currentUserIsAdmin && currentUserAccountNumber != -1)
    {
        char accountStr[20];
        sprintf(accountStr, "%d", currentUserAccountNumber);
        SetWindowText(GetDlgItem(hwnd, ID_EDIT_ACCOUNT), accountStr);

        for (int i = 0; i < accountCount; i++)
        {
            if (accounts[i].accountNumber == currentUserAccountNumber)
            {
                SetWindowText(GetDlgItem(hwnd, ID_EDIT_NAME), accounts[i].name);
                break;
            }
        }
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static HWND hEditName, hEditAccount, hEditBalance, hEditTargetAccount;
    static HWND hEditUsername, hEditPassword, hEditConfirmPassword;
    static HWND hButtonCreate, hButtonDeposit, hButtonWithdraw, hButtonDisplay, hButtonTransfer;
    static HWND hButtonLogin, hButtonLogout, hButtonRegister, hButtonCreateUser, hButtonBackToLogin;

    switch (uMsg)
    {
    case WM_CREATE:
    {
        ShowLoginControls(hwnd);
        break;
    }

    case WM_COMMAND:
    {
        if (LOWORD(wParam) == ID_BUTTON_LOGIN && !loggedIn)
        {
            char username[50], password[50];
            int i;

            hEditUsername = GetDlgItem(hwnd, ID_EDIT_USERNAME);
            hEditPassword = GetDlgItem(hwnd, ID_EDIT_PASSWORD);

            GetWindowText(hEditUsername, username, 50);
            GetWindowText(hEditPassword, password, 50);

            for (i = 0; i < userCount; i++)
            {
                if (strcmp(users[i].username, username) == 0 &&
                    strcmp(users[i].password, password) == 0)
                {
                    loggedIn = 1;
                    currentUserIsAdmin = users[i].isAdmin;
                    currentUserAccountNumber = users[i].accountNumber;

                    ShowBankingControls(hwnd);

                    MessageBox(hwnd, "Login successful!", "Success", MB_OK);
                    return 0;
                }
            }

            MessageBox(hwnd, "Invalid username or password!", "Error", MB_OK | MB_ICONERROR);
        }
        else if (LOWORD(wParam) == ID_BUTTON_REGISTER && !loggedIn)
        {
            ShowRegistrationControls(hwnd);
        }
        else if (LOWORD(wParam) == ID_BUTTON_BACK_TO_LOGIN && !loggedIn)
        {
            ShowLoginControls(hwnd);
        }
        else if (LOWORD(wParam) == ID_BUTTON_CREATE_USER && !loggedIn)
        {
            char username[50], password[50], confirmPassword[50], name[100], balanceStr[20];
            double initialDeposit;
            int i, accountNumber;

            hEditUsername = GetDlgItem(hwnd, ID_EDIT_USERNAME);
            hEditPassword = GetDlgItem(hwnd, ID_EDIT_PASSWORD);
            hEditConfirmPassword = GetDlgItem(hwnd, ID_EDIT_CONFIRM_PASSWORD);
            hEditName = GetDlgItem(hwnd, ID_EDIT_NAME);
            hEditBalance = GetDlgItem(hwnd, ID_EDIT_BALANCE);

            GetWindowText(hEditUsername, username, 50);
            GetWindowText(hEditPassword, password, 50);
            GetWindowText(hEditConfirmPassword, confirmPassword, 50);
            GetWindowText(hEditName, name, 100);
            GetWindowText(hEditBalance, balanceStr, 20);

            if (strlen(username) == 0 || strlen(password) == 0 || strlen(name) == 0 || strlen(balanceStr) == 0)
            {
                MessageBox(hwnd, "All fields are required!", "Error", MB_OK | MB_ICONERROR);
                return 0;
            }

            if (strcmp(password, confirmPassword) != 0)
            {
                MessageBox(hwnd, "Passwords do not match!", "Error", MB_OK | MB_ICONERROR);
                return 0;
            }

            for (i = 0; i < userCount; i++)
            {
                if (strcmp(users[i].username, username) == 0)
                {
                    MessageBox(hwnd, "Username already exists!", "Error", MB_OK | MB_ICONERROR);
                    return 0;
                }
            }

            initialDeposit = atof(balanceStr);
            if (initialDeposit < 0)
            {
                MessageBox(hwnd, "Initial deposit cannot be negative!", "Error", MB_OK | MB_ICONERROR);
                return 0;
            }

            if (accountCount >= MAX_ACCOUNTS)
            {
                MessageBox(hwnd, "Maximum account limit reached!", "Error", MB_OK | MB_ICONERROR);
                return 0;
            }

            accountNumber = 1000 + accountCount;

            strcpy(accounts[accountCount].name, name);
            accounts[accountCount].accountNumber = accountNumber;
            accounts[accountCount].balance = initialDeposit;
            accountCount++;

            strcpy(users[userCount].username, username);
            strcpy(users[userCount].password, password);
            users[userCount].isAdmin = 0;
            users[userCount].accountNumber = accountNumber;
            userCount++;

            SaveDataToFile();
            MessageBox(hwnd, "Account created successfully!\nYour account number is displayed when you login.", "Success", MB_OK);
            ShowLoginControls(hwnd);
        }
        else if (LOWORD(wParam) == ID_BUTTON_LOGOUT && loggedIn)
        {
            loggedIn = 0;
            currentUserIsAdmin = 0;
            currentUserAccountNumber = -1;
            ShowLoginControls(hwnd);
            MessageBox(hwnd, "Logged out successfully!", "Success", MB_OK);
        }
        else if (LOWORD(wParam) == ID_BUTTON_CREATE && loggedIn && currentUserIsAdmin)
        {
            char name[100], accountStr[20], balanceStr[20];
            int accountNumber;
            double balance;

            hEditName = GetDlgItem(hwnd, ID_EDIT_NAME);
            hEditAccount = GetDlgItem(hwnd, ID_EDIT_ACCOUNT);
            hEditBalance = GetDlgItem(hwnd, ID_EDIT_BALANCE);

            GetWindowText(hEditName, name, 100);
            GetWindowText(hEditAccount, accountStr, 20);
            GetWindowText(hEditBalance, balanceStr, 20);

            accountNumber = atoi(accountStr);
            balance = atof(balanceStr);

            if (accountCount < MAX_ACCOUNTS)
            {
                for (int i = 0; i < accountCount; i++)
                {
                    if (accounts[i].accountNumber == accountNumber)
                    {
                        MessageBox(hwnd, "Account number already exists!", "Error", MB_OK | MB_ICONERROR);
                        return 0;
                    }
                }

                strcpy(accounts[accountCount].name, name);
                accounts[accountCount].accountNumber = accountNumber;
                accounts[accountCount].balance = balance;
                accountCount++;
                SaveDataToFile();

                MessageBox(hwnd, "Bank account created successfully!", "Success", MB_OK);

                SetWindowText(hEditName, "");
                SetWindowText(hEditAccount, "");
                SetWindowText(hEditBalance, "");
            }
            else
            {
                MessageBox(hwnd, "Maximum account limit reached!", "Error", MB_OK | MB_ICONERROR);
            }
        }
        else if (LOWORD(wParam) == ID_BUTTON_DEPOSIT && loggedIn)
        {
            char accountStr[20], amountStr[20];
            int accountNumber, i;
            double amount;

            hEditAccount = GetDlgItem(hwnd, ID_EDIT_ACCOUNT);
            hEditBalance = GetDlgItem(hwnd, ID_EDIT_BALANCE);

            GetWindowText(hEditAccount, accountStr, 20);
            GetWindowText(hEditBalance, amountStr, 20);

            accountNumber = atoi(accountStr);
            amount = atof(amountStr);

            if (!currentUserIsAdmin && accountNumber != currentUserAccountNumber)
            {
                MessageBox(hwnd, "You can only deposit to your own account!", "Error", MB_OK | MB_ICONERROR);
                return 0;
            }

            if (amount <= 0)
            {
                MessageBox(hwnd, "Deposit amount must be positive!", "Error", MB_OK | MB_ICONERROR);
                return 0;
            }

            for (i = 0; i < accountCount; i++)
            {
                if (accounts[i].accountNumber == accountNumber)
                {
                    accounts[i].balance += amount;
                    SaveDataToFile();
                    MessageBox(hwnd, "Deposit successful!", "Success", MB_OK);

                    SetWindowText(hEditBalance, "");
                    return 0;
                }
            }

            MessageBox(hwnd, "Account not found!", "Error", MB_OK | MB_ICONERROR);
        }
        else if (LOWORD(wParam) == ID_BUTTON_WITHDRAW && loggedIn)
        {
            char accountStr[20], amountStr[20];
            int accountNumber, i;
            double amount;

            hEditAccount = GetDlgItem(hwnd, ID_EDIT_ACCOUNT);
            hEditBalance = GetDlgItem(hwnd, ID_EDIT_BALANCE);

            GetWindowText(hEditAccount, accountStr, 20);
            GetWindowText(hEditBalance, amountStr, 20);

            accountNumber = atoi(accountStr);
            amount = atof(amountStr);

            if (!currentUserIsAdmin && accountNumber != currentUserAccountNumber)
            {
                MessageBox(hwnd, "You can only withdraw from your own account!", "Error", MB_OK | MB_ICONERROR);
                return 0;
            }

            if (amount <= 0)
            {
                MessageBox(hwnd, "Withdrawal amount must be positive!", "Error", MB_OK | MB_ICONERROR);
                return 0;
            }

            for (i = 0; i < accountCount; i++)
            {
                if (accounts[i].accountNumber == accountNumber)
                {
                    if (accounts[i].balance >= amount)
                    {
                        accounts[i].balance -= amount;
                        SaveDataToFile();
                        MessageBox(hwnd, "Withdrawal successful!", "Success", MB_OK);
                    }
                    else
                    {
                        MessageBox(hwnd, "Insufficient balance!", "Error", MB_OK | MB_ICONERROR);
                    }

                    SetWindowText(hEditBalance, "");
                    return 0;
                }
            }

            MessageBox(hwnd, "Account not found!", "Error", MB_OK | MB_ICONERROR);
        }
        else if (LOWORD(wParam) == ID_BUTTON_TRANSFER && loggedIn)
        {
            char sourceAccountStr[20], targetAccountStr[20], amountStr[20];
            int sourceAccount, targetAccount, i, j;
            double amount;
            int sourceFound = 0, targetFound = 0;

            hEditAccount = GetDlgItem(hwnd, ID_EDIT_ACCOUNT);
            hEditTargetAccount = GetDlgItem(hwnd, ID_EDIT_TARGET_ACCOUNT);
            hEditBalance = GetDlgItem(hwnd, ID_EDIT_BALANCE);

            GetWindowText(hEditAccount, sourceAccountStr, 20);
            GetWindowText(hEditTargetAccount, targetAccountStr, 20);
            GetWindowText(hEditBalance, amountStr, 20);

            sourceAccount = atoi(sourceAccountStr);
            targetAccount = atoi(targetAccountStr);
            amount = atof(amountStr);

            if (!currentUserIsAdmin && sourceAccount != currentUserAccountNumber)
            {
                MessageBox(hwnd, "You can only transfer from your own account!", "Error", MB_OK | MB_ICONERROR);
                return 0;
            }

            if (sourceAccount == targetAccount)
            {
                MessageBox(hwnd, "Cannot transfer to the same account!", "Error", MB_OK | MB_ICONERROR);
                return 0;
            }

            if (amount <= 0)
            {
                MessageBox(hwnd, "Transfer amount must be positive!", "Error", MB_OK | MB_ICONERROR);
                return 0;
            }

            for (i = 0; i < accountCount; i++)
            {
                if (accounts[i].accountNumber == sourceAccount)
                {
                    sourceFound = 1;
                    if (accounts[i].balance < amount)
                    {
                        MessageBox(hwnd, "Insufficient balance for transfer!", "Error", MB_OK | MB_ICONERROR);
                        return 0;
                    }

                    for (j = 0; j < accountCount; j++)
                    {
                        if (accounts[j].accountNumber == targetAccount)
                        {
                            targetFound = 1;
                            accounts[i].balance -= amount;
                            accounts[j].balance += amount;
                            SaveDataToFile();

                            MessageBox(hwnd, "Transfer successful!", "Success", MB_OK);

                            SetWindowText(hEditTargetAccount, "");
                            SetWindowText(hEditBalance, "");
                            return 0;
                        }
                    }
                }
            }

            if (!sourceFound)
            {
                MessageBox(hwnd, "Source account not found!", "Error", MB_OK | MB_ICONERROR);
            }
            else if (!targetFound)
            {
                MessageBox(hwnd, "Target account not found!", "Error", MB_OK | MB_ICONERROR);
            }
        }
        else if (LOWORD(wParam) == ID_BUTTON_DISPLAY && loggedIn)
        {
            int i;
            HWND hListAccounts = GetDlgItem(hwnd, ID_LIST_ACCOUNTS);
            SendMessage(hListAccounts, LB_RESETCONTENT, 0, 0);

            if (currentUserIsAdmin)
            {
                for (i = 0; i < accountCount; i++)
                {
                    AddAccountToList(hListAccounts, accounts[i]);
                }
            }
            else
            {
                for (i = 0; i < accountCount; i++)
                {
                    if (accounts[i].accountNumber == currentUserAccountNumber)
                    {
                        AddAccountToList(hListAccounts, accounts[i]);
                        break;
                    }
                }
            }
        }
        else if (LOWORD(wParam) == ID_BUTTON_DELETE && loggedIn && currentUserIsAdmin)
        {
            char targetAccountStr[20];
            int targetAccount, i, j;

            hEditTargetAccount = GetDlgItem(hwnd, ID_EDIT_TARGET_ACCOUNT);
            GetWindowText(hEditTargetAccount, targetAccountStr, 20);
            targetAccount = atoi(targetAccountStr);

            for (i = 0; i < accountCount; i++)
            {
                if (accounts[i].accountNumber == targetAccount)
                {
                    for (j = i; j < accountCount - 1; j++)
                    {
                        accounts[j] = accounts[j + 1];
                    }
                    accountCount--;

                    for (i = 0; i < userCount; i++)
                    {
                        if (users[i].accountNumber == targetAccount)
                        {
                            for (j = i; j < userCount - 1; j++)
                            {
                                users[j] = users[j + 1];
                            }
                            userCount--;
                            break;
                        }
                    }

                    SaveDataToFile();
                    MessageBox(hwnd, "Account deleted successfully!", "Success", MB_OK);
                    return 0;
                }
            }

            MessageBox(hwnd, "Account not found!", "Error", MB_OK | MB_ICONERROR);
        }

        break;
    }

    case WM_DESTROY:
    {
        PostQuitMessage(0);
        break;
    }

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}
