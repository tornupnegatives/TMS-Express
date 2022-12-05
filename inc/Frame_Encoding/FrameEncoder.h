// Author: Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_FRAMEENCODER_H
#define TMS_EXPRESS_FRAMEENCODER_H

#include "Frame.h"
#include <string>

class FrameEncoder {
public:
    explicit FrameEncoder(bool hexPrefix = false, char separator = ',');
    explicit FrameEncoder(const std::vector<Frame> &initFrames, bool hexPrefix = false, char separator = ',');

    void appendFrame(Frame frame);
    void appendFrames(const std::vector<Frame> &initFrames);
    std::string toHex(bool shouldAppendStopFrame = true);
    std::string toJSON();

private:
    bool includeHexPrefix;
    char byteSeparator;
    std::vector<Frame> frames;
    std::vector<std::string> bytes;

    void appendStopFrame();
    std::string byteToHex(const std::string& byte) const;
};

#endif //TMS_EXPRESS_FRAMEENCODER_H
