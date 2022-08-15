// Author: Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_PATHUTILS_H
#define TMS_EXPRESS_PATHUTILS_H

#include <string>
#include <vector>

class PathUtils {
public:
    explicit PathUtils(std::string filepath);

    bool fileExists() const;
    bool isDirectory() const;
    std::vector<std::string> getPaths();
    std::vector<std::string> getFilenames();

private:
    std::string srcPath;
    std::string fileExtension;
    bool exists;
    bool fileIsDirectory;
    std::vector<std::string> paths;
    std::vector<std::string> filenames;

    static std::string extractFilenameFromPath(const std::string &path);
    static std::vector<std::string> splitString(const std::string& str, const std::string& delim);
};


#endif //TMS_EXPRESS_PATHUTILS_H
