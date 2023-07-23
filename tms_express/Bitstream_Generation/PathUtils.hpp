// Copyright (C) 2023 Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_BITSTREAM_GENERATION_PATHUTILS_HPP_
#define TMS_EXPRESS_BITSTREAM_GENERATION_PATHUTILS_HPP_

#include <string>
#include <vector>

namespace tms_express {

/// @brief Provides basic filesystem functionality, including directory
//          crawling, file metadata, and path component separation
class PathUtils {
 public:
    ///////////////////////////////////////////////////////////////////////////
    // Initializers ///////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Creates new Path Utils instance for inspecting given path
    /// @param filepath Path to analyze
    explicit PathUtils(const std::string &filepath);

    ///////////////////////////////////////////////////////////////////////////
    // Accessors //////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Crawls directory at path for files
    /// @return Vector of files in directory
    /// @note If path does not point to directory, returns single-element
    ///         vector containing path
    std::vector<std::string> getPaths() const;

    /// @brief Crawls directory at path for files and gathers their names
    /// @return Vector of filenames in directory, without path components or
    ///         file extensions
    /// @note If path does not point to directory, returns single-element
    ///         vector containing filename
    std::vector<std::string> getFilenames() const;

    ///////////////////////////////////////////////////////////////////////////
    // Metadata ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Checks if path corresponds to existing file or directory
    /// @return true if file or directory exists at path, false otherwise
    bool exists() const;

    /// @brief Checks if path corresponds to directory
    /// @return true if path points to directory, false otherwise
    bool isDirectory() const;

 private:
    ///////////////////////////////////////////////////////////////////////////
    // Static Helpers /////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Strips path components and file extension from path string
    /// @param path Path string
    /// @return Filename component of path
    static std::string extractFilenameFromPath(const std::string &path);

    /// @brief Splits string at delimiter
    /// @param str Source string
    /// @param delim Delimeter
    /// @return Vector of substrings, not including delimeter
    static std::vector<std::string> splitString(const std::string& str,
        const std::string& delim);

    ///////////////////////////////////////////////////////////////////////////
    // Members ////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief true if file or directory exists at path, false otherwise
    bool exists_;

    /// @brief true if path points to directory, false otherwise
    bool is_directory_;

    /// @brief Collection of paths at directory if path is directory,
    ///         single-element vector with original path otherwise
    std::vector<std::string> paths_;

    /// @brief Collection of filenames from path directory, excluding
    ///         path components or filename extensions. If path is not
    ///         a directory, hold single filename of original path
    std::vector<std::string> filenames_;
};

};  // namespace tms_express

#endif  //  TMS_EXPRESS_BITSTREAM_GENERATION_PATHUTILS_HPP_
