#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <random>
#include <string>
#include <cstdint>
#include <chrono>
#include <windows.h>

#pragma pack(push, 1)
struct BMPFileHeader {
    uint16_t fileType{ 0x4D42 };
    uint32_t fileSize{ 0 };
    uint16_t reserved1{ 0 };
    uint16_t reserved2{ 0 };
    uint32_t dataOffset{ 0 };
};

struct BMPInfoHeader {
    uint32_t headerSize{ 0 };
    int32_t width{ 0 };
    int32_t height{ 0 };
    uint16_t planes{ 1 };
    uint16_t bitCount{ 0 };
    uint32_t compression{ 0 };
    uint32_t imageSize{ 0 };
    int32_t xPixelsPerMeter{ 0 };
    int32_t yPixelsPerMeter{ 0 };
    uint32_t colorsUsed{ 0 };
    uint32_t importantColors{ 0 };
};
#pragma pack(pop)

struct Pixel {
    uint8_t blue, green, red;
};

class BMPImage {
public:
    BMPImage() = default;

    BMPImage(const std::string& inputFilePath)
    {
        Load(inputFilePath);
    }

    void Blur(unsigned threadCount = 1)
    {
        std::vector<std::vector<Pixel>> tempPixels = pixels;

        std::vector<std::thread> threads;
        int rowsPerThread = height / threadCount;
        int remainingRows = height % threadCount;

        for (unsigned i = 0; i < threadCount; ++i) 
        {
            int startRow = i * rowsPerThread;
            int endRow = startRow + rowsPerThread;
            
            if (i == threadCount - 1) 
            {
                endRow += remainingRows;
            }

            threads.emplace_back(&BMPImage::ProcessRows, this, startRow, endRow, std::ref(tempPixels));
        }

        for (auto& thread : threads)
        {
            thread.join();
        }

        pixels = std::move(tempPixels);
    }

    void Save(const std::string& outputFilePath) const {
        std::ofstream outFile(outputFilePath, std::ios::binary);
        if (!outFile) throw std::runtime_error("Could not open file for writing.");

        int rowSize = (width * 3 + 3) & ~3;
        int imageDataSize = rowSize * height;

        BMPFileHeader outFileHeader = fileHeader;
        BMPInfoHeader outInfoHeader = infoHeader;

        outInfoHeader.imageSize = imageDataSize;
        outFileHeader.fileSize = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + imageDataSize;
        outFileHeader.dataOffset = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);

        outFile.write(reinterpret_cast<const char*>(&outFileHeader), sizeof(outFileHeader));
        outFile.write(reinterpret_cast<const char*>(&outInfoHeader), sizeof(outInfoHeader));

        int padding = rowSize - width * 3;
        char paddingBytes[3] = { 0, 0, 0 };

        for (int y = 0; y < height; ++y)
        {
            outFile.write(reinterpret_cast<const char*>(pixels[y].data()), width * sizeof(Pixel));
            if (padding > 0)
            {
                outFile.write(paddingBytes, padding);
            }
        }
    }

    int GetWidth() const { return width; }
    int GetHeight() const { return height; }

private:
    int width, height;
    BMPFileHeader fileHeader;
    BMPInfoHeader infoHeader;
    std::vector<std::vector<Pixel>> pixels;

    void Load(const std::string& filePath)
    {
        std::ifstream inFile(filePath, std::ios::binary);
        if (!inFile) throw std::runtime_error("Could not open file for reading.");

        inFile.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
        inFile.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));

        if (fileHeader.fileType != 0x4D42)
        {
            throw std::runtime_error("Not a BMP file.");
        }

        if (infoHeader.bitCount != 24)
        {
            throw std::runtime_error("Only 24-bit BMP files are supported.");
        }

        width = infoHeader.width;
        height = std::abs(infoHeader.height);
        if (width <= 0 || height <= 0)
        {
            throw std::runtime_error("Invalid BMP dimensions.");
        }

        int rowSize = (width * 3 + 3) & ~3;
        int padding = rowSize - width * 3;

        inFile.seekg(fileHeader.dataOffset, std::ios::beg);
        pixels.resize(height, std::vector<Pixel>(width));

        for (int y = 0; y < height; ++y)
        {
            inFile.read(reinterpret_cast<char*>(pixels[y].data()), width * sizeof(Pixel));
            if (padding > 0)
            {
                inFile.seekg(padding, std::ios::cur);
            }
        }
    }

    void ProcessRows(int startRow, int endRow, std::vector<std::vector<Pixel>>& tempPixels)
    {
        for (int y = startRow; y < endRow; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                tempPixels[y][x] = CalculateAverageColor(x, y);
            }
        }
    }

    Pixel CalculateAverageColor(int x, int y) const
    {
        int red = 0, green = 0, blue = 0, count = 0;

        for (int dy = -1; dy <= 1; ++dy)
        {
            for (int dx = -1; dx <= 1; ++dx)
            {
                int nx = x + dx;
                int ny = y + dy;

                if (nx >= 0 && nx < width && ny >= 0 && ny < height)
                {
                    red += pixels[ny][nx].red;
                    green += pixels[ny][nx].green;
                    blue += pixels[ny][nx].blue;
                    ++count;
                }
            }
        }

        if (count == 0)
        {
            return pixels[y][x];
        }

        return Pixel{
            static_cast<uint8_t>(blue / count),
            static_cast<uint8_t>(green / count),
            static_cast<uint8_t>(red / count)
        };
    }
};