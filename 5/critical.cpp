#include <windows.h>
#include <string>
#include <fstream>
#include <iostream>

CRITICAL_SECTION FileLockingCriticalSection;

int ReadFromFile() {
    std::fstream myfile("balance.txt", std::ios_base::in);
    int result = 0;
    myfile >> result;
    myfile.close();
    return result;
}

void WriteToFile(int data) {
    std::fstream myfile("balance.txt", std::ios_base::out);
    myfile << data << std::endl;
    myfile.close();
}

void Deposit(int money) {
    EnterCriticalSection(&FileLockingCriticalSection);
    int balance = ReadFromFile();  // Читаем актуальный баланс
    balance += money;
    WriteToFile(balance);          // Записываем новый баланс
    printf("Balance after deposit: %d\n", balance);
    LeaveCriticalSection(&FileLockingCriticalSection);
}

void Withdraw(int money) {
    EnterCriticalSection(&FileLockingCriticalSection);
    int balance = ReadFromFile();  // Читаем актуальный баланс

    if (balance < money) {
        printf("Cannot withdraw money, balance lower than %d\n", money);
    }
    else {
        Sleep(20);
        balance -= money;
        WriteToFile(balance);      // Записываем новый баланс
        printf("Balance after withdraw: %d\n", balance);
    }
    LeaveCriticalSection(&FileLockingCriticalSection);
}

int GetBalance() {
    return ReadFromFile();
}

DWORD WINAPI DoDeposit(CONST LPVOID lpParameter) {
    Deposit(static_cast<int>(reinterpret_cast<intptr_t>(lpParameter)));
    return 0;
}

DWORD WINAPI DoWithdraw(CONST LPVOID lpParameter) {
    Withdraw(static_cast<int>(reinterpret_cast<intptr_t>(lpParameter)));
    return 0;
}

int main() {
    HANDLE handles[50]; // Исправлено: 50 потоков вместо 500
    InitializeCriticalSection(&FileLockingCriticalSection);
    WriteToFile(0);

    SetProcessAffinityMask(GetCurrentProcess(), 1);

    for (int i = 0; i < 50; i++) {
        handles[i] = (i % 2 == 0)
            ? CreateThread(NULL, 0, &DoDeposit, reinterpret_cast<LPVOID>(230), 0, NULL)
            : CreateThread(NULL, 0, &DoWithdraw, reinterpret_cast<LPVOID>(1000), 0, NULL);
    }

    WaitForMultipleObjects(50, handles, TRUE, INFINITE);
    printf("Final Balance: %d\n", GetBalance());

    for (int i = 0; i < 50; i++) {
        CloseHandle(handles[i]);
    }

    DeleteCriticalSection(&FileLockingCriticalSection);

    printf("Press any key to exit...\n");
    char some;
    std::cin >> some;
    return 0;
}