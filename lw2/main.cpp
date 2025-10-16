#include <format>
#include <iostream>
#include <string>
#include <ctime>
#include "BmpImage.h" 
#include <functional>

struct Args
{
    std::string inputFilePath;
    std::string outputFilePath;
    unsigned threadCount;
    unsigned coreCount;
};

static Args ParseArgs(const int argc, char** argv)
{
    Args args;

    if (argc != 5)
    {
        throw std::invalid_argument(std::format("Usage: {} <input.bmp> <output.bmp> <threads> <cores>", argv[0]));
    }

    args.inputFilePath = argv[1];
    args.outputFilePath = argv[2];

    try
    {
        args.threadCount = static_cast<unsigned>(std::stoi(argv[3]));
        args.coreCount = static_cast<unsigned>(std::stoi(argv[4]));
    }
    catch(...)
    {
        throw std::invalid_argument("Failed to parse int value");
    }
    
    return args;
}

void SetCoreCount(unsigned count)
{
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    unsigned maxCoreCount = info.dwNumberOfProcessors;

    if (count == 0 || count > maxCoreCount)
    {
        count = maxCoreCount;
    }

    HANDLE hProcess = GetCurrentProcess();
    DWORD_PTR processAffinityMask = 0;
    DWORD_PTR systemAffinityMask = 0;

    if (GetProcessAffinityMask(hProcess, &processAffinityMask, &systemAffinityMask))
    {
        DWORD_PTR newMask = (1 << count) - 1;
        newMask &= systemAffinityMask;
        SetProcessAffinityMask(hProcess, newMask);
    }
}

int main(const int argc, char** argv)
{
    try
    {
        auto args = ParseArgs(argc, argv);
        SetCoreCount(args.coreCount);

        const clock_t startTime = clock();

        auto bmpFile = BMPImage(args.inputFilePath);
        bmpFile.Blur(args.threadCount);
        bmpFile.Save(args.outputFilePath);

        std::cout << args.coreCount << '\t' << args.threadCount << '\t' << clock() - startTime << std::endl;
    }
    catch (const std::exception& e) 
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}