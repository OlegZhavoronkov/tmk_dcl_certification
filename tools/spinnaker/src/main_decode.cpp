//------------------------------------------------------------------------------
//  Created     : 06.08.21
//  Author      : Victor Kats
//  Description : Import Spinnnaker-based capturer into project (see TMKDCL-380)
//------------------------------------------------------------------------------

//
// Created by victor on 05.02.2020.
//
#include <Spinnaker.h>

#include <getopt.h>

#include <iostream>
#include <filesystem>
#include <string>
#include <vector>

std::string inFile;
std::string outFile;

uint height;
uint width;
Spinnaker::PixelFormatEnums pixelFormat;

void printHelp() {
    std::cout << "Options are:" << std::endl
              << "\t -h or --help - This help" << std::endl
              << "\t -i <file> or --if <file>- Input (.raw) frame file" << std::endl
              << "\t -o <file> or --of <file>- Output file name" << std::endl
              << "\t -f <height>x<width>@<pixelFormat> or --format <height>x<width>@<pixelFormat> - Format of image" << std::endl;
}

bool checkInFile(const std::string& fileName) {
    std::filesystem::path fp(fileName);
    std::error_code ec;
    if(!std::filesystem::exists(fp, ec)) {
        std::cout << "Input file is not exists or is unacceptable" << std::endl;
        std::cout << "Error code is " << ec.value() << std::endl;
        return false;
    }
    if(!std::filesystem::is_regular_file(fp, ec)) {
        std::cout << "Input file path is not pointing to the file" << std::endl;
        std::cout << "Error code is " << ec.value() << std::endl;
        return false;
    }
    inFile = std::filesystem::canonical(fp, ec);
    return true;
}

bool checkOutFile(const std::string& fileName) {
    std::filesystem::path fullPath(fileName);
    std::error_code ec;

    if(!(fullPath.has_filename() && fullPath.has_extension())) {
        std::cout << "Output file name mast point on non-existing regular file with proper suffix" << std::endl;
        return false;
    }

    std::filesystem::path fp =  std::filesystem::absolute(fullPath, ec);
    fp = fp.remove_filename();
    if(fp.string().empty())
        fp = std::filesystem::path(".");

    if(!std::filesystem::exists(fp, ec)) {
        std::cout << "Directory to emplace decoded file is not exists or is unacceptable" << std::endl;
        std::cout << "Error code is " << ec.value() << std::endl;
        return false;
    }
    if(!std::filesystem::is_directory(fp, ec)) {
        std::cout << "Specified path to emplace decoded file is not pointing to the directory" << std::endl;
        std::cout << "Error code is " << ec.value() << std::endl;
        return false;
    }
    outFile = fullPath;
    return true;
}

bool parseFormat(const std::string& formatString) {
    auto xPos = formatString.find('x');
    auto atPos = formatString.find('@');
    if(xPos == std::string::npos || xPos < 1 || atPos == std::string::npos|| atPos < xPos) {
        std::cout << "Expected format string is <height>x<width>@<pixelFormat>" << std::endl;
        return false;
    }
    //auto sHeight = formatString.substr(0, xPos);
    //auto sWidth = formatString.substr((xPos + 1, 3));
    //auto sFormat = formatString.substr(atPos+1, formatString.length() - atPos);
    try {

        pixelFormat = (Spinnaker::PixelFormatEnums)std::stoi(formatString.substr(atPos + 1, formatString.length() - atPos));
        height = std::stoi(formatString.substr(0, xPos));
        width = std::stoi(formatString.substr(xPos + 1, atPos - xPos - 1));

    }
    catch(...) {
        std::cout << "Expected format string is <height>x<width>@<pixelFormat>" << std::endl;
        return false;
    }
    return true;
}

int main(int argc, char **argv) {

    const char* shortOptions = "?hi:o:f:";

    const struct option longOptions[] = {
            {"help",   no_argument, NULL,               'h'},
            {"if",     required_argument, NULL, 'i'},
            {"of",     required_argument, NULL, 'o'},
            {"format", required_argument, NULL, 'f'},
            {NULL, 0,               NULL,               0}
    };

    int option;
    int optionIndex;

    bool hasIf = false;
    bool hasOf = false;
    bool hasFormat = false;
    while ((option=getopt_long(argc, argv, shortOptions,
                               longOptions, &optionIndex)) != -1) {

        switch (option) {
            case 'h':
            case '?': {
                printHelp();
                return 0;
                break;
            };
            case 'i': {
                hasIf = true;
                if(!checkInFile(optarg))
                    return 1;
                break;
            };

            case 'o': {
                hasOf = true;
                if(!checkOutFile(optarg))
                    return 1;
                break;
            };
            case 'f': {
                hasFormat = true;
                if(!parseFormat(optarg))
                    return 1;
                break;
            }
            default: {
                printf("found unknown option\n");
                printHelp();
                return 1;
            };
        };
    };

    if(!hasIf || !hasOf || ! hasFormat){
        printHelp();
        return 1;
    }

    std::cout << inFile << std::endl;
    std::cout << outFile << std::endl;
    std::cout << height << "x" << width << "@" << pixelFormat;
    //return 0;
    float bpp = 1;
    if (pixelFormat == Spinnaker::PixelFormatEnums::PixelFormat_Mono16 || pixelFormat == Spinnaker::PixelFormatEnums::PixelFormat_Mono12)
        bpp = 2;
    if (pixelFormat == Spinnaker::PixelFormatEnums::PixelFormat_Mono12Packed || pixelFormat == Spinnaker::PixelFormatEnums::PixelFormat_Mono12p)
        bpp = 1.5;

    int offlineOffsetX = 0;
    int offlineOffsetY = 0;
    unsigned char* offlineData;

    offlineData = (unsigned char*)malloc(sizeof(unsigned char) * height * width * bpp);

    Spinnaker::ImagePtr loadImage = Spinnaker::Image::Create(width, height, offlineOffsetX, offlineOffsetY, pixelFormat, offlineData);

    FILE* dataFile;

    dataFile = fopen(inFile.c_str(), "rb");
    auto bytes = fread(offlineData, 1, height * width * bpp, dataFile);
    if(bytes ==  height * width * bpp) {
        if(pixelFormat == Spinnaker::PixelFormatEnums::PixelFormat_Mono8
        || pixelFormat == Spinnaker::PixelFormatEnums::PixelFormat_Mono8s
        || pixelFormat == Spinnaker::PixelFormatEnums::PixelFormat_Mono12
        || pixelFormat == Spinnaker::PixelFormatEnums::PixelFormat_Mono12p
        || pixelFormat == Spinnaker::PixelFormatEnums::PixelFormat_Mono12Packed
        || pixelFormat == Spinnaker::PixelFormatEnums::PixelFormat_Mono16
        || pixelFormat == Spinnaker::PixelFormatEnums::PixelFormat_Mono16s
        ) {
            //Spinnaker::ImagePtr mono12Image = loadImage->Convert(Spinnaker::PixelFormatEnums::PixelFormat_Mono16);
            loadImage->Save(outFile.c_str());
        }
        else {
            Spinnaker::ImagePtr rgbImage = loadImage->Convert(Spinnaker::PixelFormatEnums::PixelFormat_BGR8, Spinnaker::ColorProcessingAlgorithm::WEIGHTED_DIRECTIONAL_FILTER);
            rgbImage->Save(outFile.c_str());
        }
        return 0;
    }
    else {
        std::cout << "Incorrect input file length" << std::endl;
        return 1;
    }
}
