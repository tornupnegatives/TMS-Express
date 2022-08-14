//
// Created by Joseph Bellahcen on 7/23/22.
//

#include "Interfaces/FileUtils.h"
#include <string>
#include <vector>

FileUtils::FileUtils(std::string filepath) {
    file = fopen(filepath.c_str(), "r");
    path = filepath;
}

FileUtils::~FileUtils() {
    fclose(file);
}

bool FileUtils::fileExists() {
    bool exists = (file != nullptr);
    return exists;
}

std::string FileUtils::getFilename() {
    // Get the lowest element in the path hierarchy
    auto fullFilename = splitString(path, "/").back();

    // Remove the extension
    auto filename = splitString(fullFilename, ".").at(0);

    return filename;
}

std::vector<std::string> FileUtils::splitString(std::string str, std::string delim) {
    auto result = std::vector<std::string>();

    int delimSize = delim.length();

    int start = 0;
    int end = str.find(delim);

    while (end != std::string::npos) {
        auto substr = str.substr(start, end - start);
        result.push_back(substr);

        start = end + delimSize;
        end = str.find(delim, start);
    }

    result.push_back(str.substr(start, end - start));

    return result;
}
