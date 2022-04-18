//
// Created by Joseph Bellahcen on 4/16/22.
//

#ifndef TMS_EXPRESS_UPPERVOCALTRACTANALYZER_H
#define TMS_EXPRESS_UPPERVOCALTRACTANALYZER_H

class UpperVocalTractAnalyzer {
public:
    UpperVocalTractAnalyzer(int samplesPerSegment, int order);

    float *lpcCoefficients(float *xcorr);
    float energy(float *segment);

private:
    int samplesPerSegment;
    int order;
};

#endif //TMS_EXPRESS_UPPERVOCALTRACTANALYZER_H
