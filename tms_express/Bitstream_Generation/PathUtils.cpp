// Copyright (C) 2023 Joseph Bellahcen <joeclb@icloud.com>

#include "Bitstream_Generation/PathUtils.hpp"

#include <filesystem>
#include <string>
#include <utility>
#include <vector>

namespace tms_express {

///////////////////////////////////////////////////////////////////////////////
// Initializers ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

PathUtils::PathUtils(const std::string &filepath) {
    // Gather file metadata
    exists_ = std::filesystem::exists(filepath);
    is_directory_ = std::filesystem::is_directory(filepath);

    // Traverse directory (if applicable)
    paths_ = std::vector<std::string>();

    if (!is_directory_) {
        paths_.push_back(filepath);

    } else {
        auto iterator = std::filesystem::directory_iterator(filepath);
        for (const auto& entry : iterator) {
            auto path = entry.path().string();
            paths_.push_back(path);
        }
    }

    // Extract filenames
    filenames_ = std::vector<std::string>();
    for (const auto& path : paths_) {
        auto filename = extractFilenameFromPath(path);
        filenames_.push_back(filename);
    }
}

///////////////////////////////////////////////////////////////////////////////
// Accessors //////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

std::vector<std::string> PathUtils::getPaths() const {
    return paths_;
}

std::vector<std::string> PathUtils::getFilenames() const {
    return filenames_;
}

///////////////////////////////////////////////////////////////////////////////
// Metadata ///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool PathUtils::exists() const {
    return exists_;
}

bool PathUtils::isDirectory() const {
    return is_directory_;
}

///////////////////////////////////////////////////////////////////////////////
// Static Helpers /////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

std::string PathUtils::extractFilenameFromPath(const std::string &path) {
    // Get the lowest element in the path hierarchy
    auto filename_with_extension = splitString(path, "/").back();

    // Remove the extension
    auto filename = splitString(filename_with_extension, ".").at(0);

    return filename;
}

std::vector<std::string> PathUtils::splitString(const std::string& str,
    const std::string& delim) {
    auto result = std::vector<std::string>();
    auto delim_size = delim.length();

    auto start = 0;
    auto end = str.find(delim);

    while (end != std::string::npos) {
        auto substr = str.substr(start, end - start);
        result.push_back(substr);

        start = end + delim_size;
        end = str.find(delim, start);
    }

    result.push_back(str.substr(start, end - start));

    return result;
}

};  // namespace tms_express
