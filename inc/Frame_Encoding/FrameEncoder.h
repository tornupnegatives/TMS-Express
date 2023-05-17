// Author: Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_FRAMEENCODER_H
#define TMS_EXPRESS_FRAMEENCODER_H

#include "Frame.h"
#include <string>

class FrameEncoder {
public:
    explicit FrameEncoder(bool includeHexPrefix = false, char separator = ',');
    explicit FrameEncoder(const std::vector<Frame> &initialFrames, bool includeHexPrefix = false, char separator = ',');

    // Append functions
    void append(Frame frame);
    void append(const std::vector<Frame> &newFrames);

    // Serialization
    std::string toHex(bool shouldAppendStopFrame = true);
    std::string toJSON();

private:
    std::vector<std::string> bytes;
    char byteSeparator;
    std::vector<Frame> frames;
    bool shouldIncludeHexPrefix;

    void appendStopFrame();
    [[nodiscard]] std::string byteToHex(const std::string& byte) const;
};

#endif //TMS_EXPRESS_FRAMEENCODER_H
