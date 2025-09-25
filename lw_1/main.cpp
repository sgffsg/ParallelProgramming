#include <iostream>
#include <string>
#include "windows.h"
#include <functional>

static int ParseInput(int argc, char* argv[])
{
    int threadsCount = 0;
    if (argc < 2)
    {
        std::cout << "Enter threads count:" << std::endl;
        std::cin >> threadsCount;
    }
    else
    {
        if (argc != 2)
        {
            throw std::invalid_argument("Invalid argument count\nUsage: threads.exe <threads count>");
        }

        try
        {
            threadsCount = std::stoi(argv[1]);
        }
        catch (...)
        {
            throw std::invalid_argument("Invalid thread count argument: must be an integer");
        }
    }

    return threadsCount;
}

DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
int threadNumber = *(int*)lpParam;
std::cout << "Executing thread#" << threadNumber << std::endl;
ExitThread(0);
}

int main(int argc, char* argv[])
{
    try
    {
        int threadsCount = ParseInput(argc, argv);

        HANDLE* handles = new HANDLE[threadsCount];
        int* threadNumbers = new int[threadsCount];
        for (int i = 0; i < threadsCount; i++)
        {
            threadNumbers[i] = i + 1;
            std::cout << "Create thread#" << i+1 << std::endl;
            handles[i] = CreateThread(nullptr, 0, &ThreadProc, &threadNumbers[i], CREATE_SUSPENDED, nullptr);

            if (handles[i] == nullptr) {
                std::cout << "Failed to create thread#" << i + 1 << std::endl;

                delete[] handles;
                delete[] threadNumbers;

                return 1;
            }
        }

        for (int i = 0; i < threadsCount; i++)
        {
            ResumeThread(handles[i]);
        }

        WaitForMultipleObjects(threadsCount, handles, TRUE, INFINITE);
        std::cout << "All threads are finished" << std::endl;

        delete[] handles;
        delete[] threadNumbers;
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << "\n";
        return 1;
    }

    return 0;
}
