// Author: Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_SYNTHESIZER_H
#define TMS_EXPRESS_SYNTHESIZER_H

#include "Frame_Encoding/Frame.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace tms_express {

class Synthesizer {
public:
    explicit Synthesizer(int sampleRateHz = 8000, float frameRateMs = 25.0f);

    std::vector<float> synthesize(const std::vector<Frame>& frames);
    void render(const std::string& path);
    std::vector<float> samples();

private:
    int sampleRate;
    float windowWidth;
    int samplesPerFrame;

    float synthEnergy, synthPeriod;
    float synthK1, synthK2, synthK3, synthK4, synthK5, synthK6, synthK7, synthK8, synthK9, synthK10;
    float x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, u0;
    int synthRand, periodCounter;

    std::vector<float> synthesizedSamples;

    void reset();

    bool updateSynthTable(Frame frame);
    bool updateNoiseGenerator();
    float updateLatticeFilter();
};

};  // namespace tms_express

#endif //TMS_EXPRESS_SYNTHESIZER_H
