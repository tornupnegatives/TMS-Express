// Author: Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_FRAMEPOSTPROCESSOR_H
#define TMS_EXPRESS_FRAMEPOSTPROCESSOR_H

#include "Frame.h"
#include <vector>

namespace tms_express {

class FramePostprocessor {
public:
    explicit FramePostprocessor(std::vector<Frame> *frames, float maxVoicedGainDB = 37.5, float maxUnvoicedGainDB = 37.5);

    // Getters & setters
    float getMaxUnvoicedGainDB() const;
    void setMaxUnvoicedGainDB(float gainDB);

    float getMaxVoicedGainDB() const;
    void setMaxVoicedGainDB(float gainDB);

    // Frame table manipulation
    int detectRepeatFrames();
    void normalizeGain();
    void shiftGain(int offset);
    void shiftPitch(int offset);
    void overridePitch(int index);
    // TODO: void interpolatePitch();

    // Utility
    void reset();

private:
    std::vector<Frame> originalFrameTable;
    std::vector<Frame> *frameTable;
    float maxUnvoicedGain;
    float maxVoicedGain;

    void normalizeGain(bool normalizeVoicedFrames);
};

};  // namespace tms_express

#endif //TMS_EXPRESS_FRAMEPOSTPROCESSOR_H
