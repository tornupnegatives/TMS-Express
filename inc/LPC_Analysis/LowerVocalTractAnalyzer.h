//
// Created by Joseph Bellahcen on 4/16/22.
//

#ifndef TMS_EXPRESS_LOWERVOCALTRACTANALYZER_H
#define TMS_EXPRESS_LOWERVOCALTRACTANALYZER_H

class LowerVocalTractAnalyzer {
public:
    enum voicing {UNVOICED, VOICED};
    LowerVocalTractAnalyzer(int samplesPerSegment, int sampleRate = 8000, int minPitchHz = 50, int maxPitchHz = 500, float unvoicedThreshold = 0.25);

    int estimatePitch(float *xcorr);
    voicing detectVoicing(int pitch, float *xcorr);

private:
    int samplesPerSegment;
    int minPitchPeriod;
    int maxPitchPeriod;
    float unvoicedThreshold;
};

#endif //TMS_EXPRESS_LOWERVOCALTRACTANALYZER_H
