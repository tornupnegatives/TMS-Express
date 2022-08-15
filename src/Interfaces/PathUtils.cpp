//
// Created by Joseph Bellahcen on 7/23/22.
//

#include "Interfaces/PathUtils.h"
#include <string>
#include <utility>
#include <vector>

#if __APPLE__
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif

PathUtils::PathUtils(std::string filepath) {
    // Gather file metadata
    srcPath = std::move(filepath);
    _exists = fs::exists(srcPath);
    _isDirectory = fs::is_directory(srcPath);

    // Traverse directory (if applicable)
    paths = std::vector<std::string>();
    if (!_isDirectory) {
        paths.push_back(srcPath);
    } else {
        for (const auto& dirEntry : fs::directory_iterator(srcPath)) {
            paths.push_back(dirEntry.path());
        }
    }

    // Extract filenames
    filenames = std::vector<std::string>();
    for (const auto& path : paths) {
        auto filename = extractFilenameFromPath(path);
        filenames.push_back(filename);
    }
}

bool PathUtils::fileExists() const {
    return _exists;
}

bool PathUtils::isDirectory() const {
    return _isDirectory;
}

std::vector<std::string> PathUtils::getPaths() {
    return paths;
}

std::vector<std::string> PathUtils::getFilenames() {
    return filenames;
}

std::string PathUtils::extractFilenameFromPath(const std::string &path) {
    // Get the lowest element in the path hierarchy
    auto fullFilename = splitString(path, "/").back();

    // Remove the extension
    auto filename = splitString(fullFilename, ".").at(0);

    return filename;
}

std::vector<std::string> PathUtils::splitString(const std::string& str, const std::string& delim) {
    auto result = std::vector<std::string>();

    int delimSize = int(delim.length());

    int start = 0;
    int end = int(str.find(delim));

    while (end != std::string::npos) {
        auto substr = str.substr(start, end - start);
        result.push_back(substr);

        start = end + delimSize;
        end = int(str.find(delim, start));
    }

    result.push_back(str.substr(start, end - start));

    return result;
}
