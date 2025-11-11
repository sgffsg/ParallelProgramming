//#include <windows.h>
//#include <string>
//#include <fstream>
//#include <iostream>
//
//HANDLE FileMutex;
//
//int ReadFromFile()
//{
//    std::fstream myfile("balance.txt", std::ios_base::in);
//    int result = 0;
//    myfile >> result;
//    myfile.close();
//    return result;
//}
//
//void WriteToFile(int data)
//{
//    std::fstream myfile("balance.txt", std::ios_base::out);
//    myfile << data << std::endl;
//    myfile.close();
//}
//
//int GetBalance()
//{
//    WaitForSingleObject(FileMutex, INFINITE);
//    int balance = ReadFromFile();
//    ReleaseMutex(FileMutex);
//    return balance;
//}
//
//void Deposit(int money)
//{
//    WaitForSingleObject(FileMutex, INFINITE);
//    int balance = ReadFromFile();
//    balance += money;
//    WriteToFile(balance);
//    printf("Balance after deposit: %d\n", balance);
//    ReleaseMutex(FileMutex);
//}
//
//void Withdraw(int money)
//{
//    WaitForSingleObject(FileMutex, INFINITE);
//    int balance = ReadFromFile();
//
//    if (balance < money)
//    {
//        printf("Cannot withdraw money, balance lower than %d\n", money);
//    }
//    else
//    {
//        Sleep(20);
//        balance -= money;
//        WriteToFile(balance);
//        printf("Balance after withdraw: %d\n", balance);
//    }
//    ReleaseMutex(FileMutex);
//}
//
//DWORD WINAPI DoDeposit(CONST LPVOID lpParameter)
//{
//    Deposit(static_cast<int>(reinterpret_cast<intptr_t>(lpParameter)));
//    return 0;
//}
//
//DWORD WINAPI DoWithdraw(CONST LPVOID lpParameter)
//{
//    Withdraw(static_cast<int>(reinterpret_cast<intptr_t>(lpParameter)));
//    return 0;
//}
//
//int main()
//{
//    HANDLE handles[50];
//
//    FileMutex = CreateMutex(NULL, FALSE, TEXT("Global\\BalanceFileMutex"));
//
//    if (GetLastError() == ERROR_ALREADY_EXISTS) {
//        printf("Another instance is already running\n");
//    }
//
//    // Инициализация файла должна быть защищена мьютексом
//    WaitForSingleObject(FileMutex, INFINITE);
//    WriteToFile(0);
//    ReleaseMutex(FileMutex);
//
//    for (int i = 0; i < 50; i++)
//    {
//        handles[i] = (i % 2 == 0)
//            ? CreateThread(NULL, 0, &DoDeposit, reinterpret_cast<LPVOID>(230), 0, NULL)
//            : CreateThread(NULL, 0, &DoWithdraw, reinterpret_cast<LPVOID>(1000), 0, NULL);
//    }
//
//    WaitForMultipleObjects(50, handles, TRUE, INFINITE);
//
//    // Финальное чтение баланса должно быть защищено
//    WaitForSingleObject(FileMutex, INFINITE);
//    printf("Final Balance: %d\n", GetBalance());
//    ReleaseMutex(FileMutex);
//
//    for (int i = 0; i < 50; i++)
//    {
//        CloseHandle(handles[i]);
//    }
//
//    CloseHandle(FileMutex);
//
//    printf("Press any key to exit...\n");
//    char some;
//    std::cin >> some;
//    return 0;
//}