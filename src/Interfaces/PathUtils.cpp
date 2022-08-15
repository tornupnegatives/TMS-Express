///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class: PathUtils
//
// Description: The PathUtils class provides basic file metadata. It can crawl directories for files as well as isolate
//              filenames from other path components
//
// Author: Joseph Bellahcen <joeclb@icloud.com>
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
    exists = fs::exists(srcPath);
    fileIsDirectory = fs::is_directory(srcPath);

    // Traverse directory (if applicable)
    paths = std::vector<std::string>();
    if (!fileIsDirectory) {
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

// Check if file exists
bool PathUtils::fileExists() const {
    return exists;
}

// Check if file is directory
bool PathUtils::isDirectory() const {
    return fileIsDirectory;
}

// Get vector of paths pointed to by file
//
// Returns vector with a single element if the file is not a directory
std::vector<std::string> PathUtils::getPaths() {
    return paths;
}

// Return vector of filenames residing at path
//
//Returns vector with a single element if the file is not a directory
std::vector<std::string> PathUtils::getFilenames() {
    return filenames;
}

// Isolate filename from other path components, including extensions
std::string PathUtils::extractFilenameFromPath(const std::string &path) {
    // Get the lowest element in the path hierarchy
    auto fullFilename = splitString(path, "/").back();

    // Remove the extension
    auto filename = splitString(fullFilename, ".").at(0);

    return filename;
}

// Split string at delimiter into vector of substrings
//
// Does not include delimiter in substrings
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
