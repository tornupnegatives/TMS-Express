//
// Created by Joseph Bellahcen on 4/16/22.
//

#ifndef TMS_EXPRESS_LOWERVOCALTRACTANALYZER_H
#define TMS_EXPRESS_LOWERVOCALTRACTANALYZER_H

#define VOICED true
#define UNVOICED false

typedef bool Voicing;

class LowerVocalTractAnalyzer {
public:
    explicit LowerVocalTractAnalyzer(int samplesPerSegment, int sampleRate = 8000, int minPitchHz = 50, int maxPitchHz = 500, float unvoicedThreshold = 0.3);

    int estimatePitch(float *xcorr);
    Voicing detectVoicing(float *segment, float energy, float *xcorr, float gain, int pitchPeriod);

private:
    int samplesPerSegment;
    int minPitchPeriod;
    int maxPitchPeriod;
    float unvoicedThreshold;
};

#endif //TMS_EXPRESS_LOWERVOCALTRACTANALYZER_H
