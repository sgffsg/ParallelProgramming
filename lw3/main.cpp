#include <windows.h>
#include <iostream>
#include <string>
#include <sstream>
#include <chrono>

#pragma comment(lib, "winmm.lib")

using namespace std;

const int THREAD_COUNT = 2;
const int OPERATION_COUNT = 21;

struct ThreadData 
{
    int threadNum;
    HANDLE logFileHandle;
    DWORD startTime;
};

void LongOperation()
{
    for (int i = 0; i < 1000000; ++i)
    {
        const auto k = i * i;
        i += k;
        for (int o = 0; o < k; ++o)
        {
            i -= k;
        }
    }
}

DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
    ThreadData* data = static_cast<ThreadData*>(lpParam);
    int num = data->threadNum;
    HANDLE logFileHandle = data->logFileHandle;
    DWORD startTime = data->startTime;

    for (int i = 0; i < OPERATION_COUNT; i++) 
    {
        DWORD currentTime = timeGetTime();
        DWORD elapsedTime = currentTime - startTime;
        LongOperation();
       
        std::ostringstream oss;
        oss << num << "|" << elapsedTime << " \n";
        std::string output = oss.str();

        DWORD bytesWritten;
        WriteFile(logFileHandle, output.c_str(), output.size(), &bytesWritten, NULL);
    }

    ExitThread(0);
}

int main(int argc, char* argv[])
{
    std::cout << "Waiting to start..." << std::endl;
    std::cin.get();
    std::cout << "Processing..." << std::endl;

    HANDLE logFileHandle = CreateFile(L"thread_log.txt", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (logFileHandle == INVALID_HANDLE_VALUE)
    {
        cerr << "Error opening file for logging" << endl;
        return 1;
    }

    HANDLE handles[THREAD_COUNT];
    ThreadData threadData[THREAD_COUNT];
    DWORD startTime = timeGetTime();
   
    for (int i = 0; i < THREAD_COUNT; i++)
    {
        threadData[i] = { i + 1, logFileHandle, startTime };
        handles[i] = CreateThread(NULL, 0, &ThreadProc, &threadData[i], 0, NULL);
        if (handles[i] == NULL) 
        {
            cerr << "Error creating thread" << endl;
            CloseHandle(logFileHandle);
            return 1;
        }
    }

    //SetThreadPriority(handles[0], THREAD_PRIORITY_HIGHEST);

    WaitForMultipleObjects(THREAD_COUNT, handles, TRUE, INFINITE);

    for (int i = 0; i < THREAD_COUNT; i++)
    {
        CloseHandle(handles[i]);
    }

    CloseHandle(logFileHandle);

    cout << "Logging completed. Check 'thread_log.txt' for results." << endl;
    return 0;
}