#ifndef TMS_EXPRESS_FILEUTILS_H
#define TMS_EXPRESS_FILEUTILS_H

#include <string>
#include <vector>

class FileUtils {
public:
    FileUtils(std::string filepath);
    ~FileUtils();

    bool fileExists();
    std::string getFilename();

private:
    FILE *file;
    std::string path;
    std::string fileExtension;

    std::vector<std::string> splitString(std::string str, std::string delim);
};


#endif //TMS_EXPRESS_FILEUTILS_H
