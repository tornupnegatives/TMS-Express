//
// Created by Joseph Bellahcen on 4/16/22.
//

#ifndef TMS_EXPRESS_LOWERVOCALTRACTANALYZER_H
#define TMS_EXPRESS_LOWERVOCALTRACTANALYZER_H

class LowerVocalTractAnalyzer {
public:
    enum voicing {UNVOICED, VOICED};
    LowerVocalTractAnalyzer(int numSegments, int samplesPerSegment, float unvoicedThreshold = 0.3);

    int estimatePitch(float *xcorr);
    voicing detectVoicing(int pitch, float xcorr_0);

private:
    int samplesPerSegment;
    float unvoicedThreshold;
};

#endif //TMS_EXPRESS_LOWERVOCALTRACTANALYZER_H
