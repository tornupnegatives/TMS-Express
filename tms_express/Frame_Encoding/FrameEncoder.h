// Author: Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_FRAMEENCODER_H
#define TMS_EXPRESS_FRAMEENCODER_H

#include "Frame.hpp"
#include <cstddef>
#include <string>
#include <vector>

namespace tms_express {

class FrameEncoder {
public:
    explicit FrameEncoder(bool includeHexPrefix = false, char separator = ',');
    explicit FrameEncoder(const std::vector<Frame> &initialFrames, bool includeHexPrefix = false, char separator = ',');

    // Append functions
    void append(Frame frame);
    void append(const std::vector<Frame> &newFrames);

    // Import functions
    size_t importFromAscii(const std::string &path);
    //size_t importFromBin(const std::string &path);
    //size_t importFromEmbedded(std::string path);
    //int importFromJson(std::string path) {};

    // Serialization
    std::string toHex(bool shouldAppendStopFrame = true);
    std::vector<std::byte> toBin(bool shouldAppendStopFrame = true);

    std::string toJSON();
    std::vector<Frame> frameTable();

    // De-serialization
    size_t parseAsciiBitstream(std::string flatBitstream);

private:
    std::vector<std::string> binary;
    char byteSeparator;
    std::vector<Frame> frames;
    bool shouldIncludeHexPrefix;

    void appendStopFrame();
    [[nodiscard]] std::string byteToHex(const std::string &byte) const;
};

};  // namespace tms_express

#endif //TMS_EXPRESS_FRAMEENCODER_H
